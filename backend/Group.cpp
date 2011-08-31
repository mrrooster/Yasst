/*
(c) 2009,2010,2011 Ian Clark

This file is part of Yasst.

Yasst is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Yasst is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Yasst.  If not, see <http://www.gnu.org/licenses/>.

*/
/* $Id: Group.cpp,v 1.9 2010/08/01 18:15:36 ian Exp $

 File       : Group.cpp
 Create date: 00:19:41 6 Dec 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "Group.h"
#include "BirdBox.h"
#include "TweetUser.h"
#include "fetchers/UserGroupTweetFetcher.h"
#include "fetchers/UserGroupFetcher.h"
#include <QUuid>
#include <QTimer>
#include <QMessageBox>
#include <QMutexLocker>

Group::Group(BirdBox *b) : subscriber_count(0),member_count(0),twitterList(false),fetcher(0),refCount(0),populateUsers(false) {
	local_id=QUuid::createUuid().toString();
	tweets=b;
	drainTimer.setInterval(100);
	QObject::connect(&drainTimer,SIGNAL(timeout()),this,SLOT(drainPendingLists()));
}

Group::~Group() {
	QObject::disconnect(&drainTimer,SIGNAL(timeout()),this,SLOT(drainPendingLists()));
}

void Group::updateGroupMembers(QStringList list) {
	if (twitterList) {
		int x;
		for(x=0;x<list.count();x++) {
			if (!groupmembers.contains(list.at(x))) {
				newMembers.append(list.at(x));
			}
		}
		for(x=0;x<groupmembers.count();x++) {
			if (!list.contains(groupmembers.at(x))) {
				removedMembers.append(groupmembers.at(x));
			}
		}
		if (newMembers.count()||removedMembers.count())
			drainTimer.start();
	}
	groupmembers=list;
	updateFetcher();
	emit groupUpdated(this);
}

void Group::setGroupMembers(QStringList g) {
	groupmembers=g;
	updateFetcher();
	emit groupUpdated(this);
}

void Group::updateGroupMembers(QString name) {
	if (groupmembers.contains(name))
		return;
	groupmembers.append(name);
	if (twitterList)
		twitterAddListMember(name);
	updateFetcher();
	emit groupUpdated(this);
}

void Group::removeGroupMember(QString name) {
	if (groupmembers.contains(name)) {
		groupmembers.removeOne(name);
		if (twitterList)
			twitterRemoveListMember(name);
		updateFetcher();
		emit groupUpdated(this);
	}
}

void Group::updateName(QString n) {
	name=n;
	if (twitterList) {
		updateTwitter();
	}
	emit groupUpdated(this);
}

void Group::updateIsPrivate(bool priv) {
	if (priv==privateList)
		return;
	privateList=priv;
	if (twitterList) {
		updateTwitter();
	}
	emit groupUpdated(this);
}

void Group::updateTwitter() {
	TweetFetcher *f = tweets->getUserFetcher();
	if (twitterList && f)
		f->updateListDetails(id,name,"",privateList);
//		doHttpBit("http://api.twitter.com/1/"+user+"/lists/"+id+".xml",user,pass,"name="+QUrl::toPercentEncoding(name)+"&mode="+(privateList?"private":"public"));
}

void Group::drainPendingLists() {
	if (newMembers.count()) {
//		QMessageBox::information(0,"Hi","Add: "+newMembers.at(0));
		twitterAddListMember(newMembers.takeFirst());
	} else if (removedMembers.count()) {
//		QMessageBox::information(0,"Hi","Remove: "+removedMembers.takeFirst());
		twitterRemoveListMember(removedMembers.takeFirst());
	}
	if (!newMembers.count()&&!removedMembers.count())
		drainTimer.stop();

}

void Group::drainWaitingLists() {
	QMutexLocker waitingLocker(&waitingLock);
	newMembers.append(newMembersWaiting);
	removedMembers.append(removedMembersWaiting);
	newMembersWaiting.clear();
	removedMembersWaiting.clear();
	drainTimer.start();
}

void Group::twitterRemoveListMember(QString memberhehehehe) {
	QMutexLocker waitingLocker(&waitingLock);
	TweetUser *u = tweets->getUserByScreenName(memberhehehehe,true);
	TweetFetcher *f = tweets->getUserFetcher();
	if (u && u->id!="" && f)
		f->removeUserFromList(id,u->screen_name);
	else {
		removedMembers.append(memberhehehehe);
		QTimer::singleShot(1000,this,SLOT(drainWaitingLists()));
	}
}

void Group::twitterAddListMember(QString member) {
	QMutexLocker waitingLocker(&waitingLock);
	TweetUser *u = tweets->getUserByScreenName(member,true);
	TweetFetcher *f = tweets->getUserFetcher();
	if (u && u->id!="" && f) {
		f->addUserToList(id,u->screen_name);
	} else {
		newMembers.append(member);
		drainTimer.start();
	}
}

void Group::convertToLocal() {
	if (twitterList) {
		TweetFetcher *f = tweets->getUserFetcher();
		if (f)
			f->removeListBySlug(slug);
//		doHttpBit("http://api.twitter.com/1/"+user+"/lists/"+slug+".xml",user,pass,"_method=DELETE");
		twitterList=false;
		id="";
		emit groupUpdated(this);
	}
}

void Group::updateFetcher() {
	bool needFetcher=false;


	if (!twitterList || refCount==0) {
		if (fetcher) {
			tweets->removeFetcher(fetcher);
			fetcher->deleteLater();
			fetcher=0;
		}
		return;
	}
	for(int x=0;x<groupmembers.count();x++) {
		QString name = groupmembers.at(x);
		if (name == tweets->getAuthenticatingUserScreenName())
			continue;
		TweetUser *u = tweets->getUserByScreenName(name);
		if (u && u->is_friend==false) {
			needFetcher=true;
			break;
		}
	}

	if (needFetcher && !fetcher) {
		fetcher = new UserGroupTweetFetcher();
		fetcher->setGroup(this);
		tweets->addFetcher(fetcher,true);
		QObject::connect(fetcher,SIGNAL(endFetching()),this,SLOT(updateFetcher()));
	} else if (!needFetcher && fetcher) {
		tweets->removeFetcher(fetcher);
		QObject::disconnect(fetcher,SIGNAL(endFetching()),this,SLOT(updateFetcher()));
		fetcher->deleteLater();
		fetcher=0;
	}

}

void Group::hello() {
	if ((refCount++)==0) {
		populateUsers=true;
		updateFetcher();
		updateMembers();
	}
}

void Group::goodbye() {
	if ((--refCount)==0) {
		populateUsers=false;
		updateFetcher();
	}
}

int Group::getRefCount() {
	return refCount;
}

void Group::updateMembers() {
	if (populateUsers && twitterList) {
		UserGroupFetcher *newFetcher = new UserGroupFetcher();
		newFetcher->setGroup(this);
		newFetcher->setUser(user);
//		newFetcher->setPass(pass);
		tweets->addFetcher(newFetcher,true);
		newFetcher->minTimeoutSecs=1;
		newFetcher->activateTimeout(5,0);
	}
}

void Group::refresh() {
	if (fetcher)
		fetcher->populate();
	else {
		TweetFetcher *f = tweets->getFetcher(1);
		if (f)
			f->populate();
	}
}
