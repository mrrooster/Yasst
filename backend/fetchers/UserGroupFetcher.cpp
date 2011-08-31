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
/* $Id: UserGroupFetcher.cpp,v 1.10 2010/07/20 20:08:15 ian Exp $

 File       : UserGroupFetcher.cpp
 Create date: 15:47:01 6 Dec 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "UserGroupFetcher.h"
#include "../BirdBox.h"
#include <QVariant>
#include <QMessageBox>

UserGroupFetcher::UserGroupFetcher() : UserTweetFetcher(),myGroup(0),state(0) {
	trustUserFollowingStatus=true;
	type=5;
	weight=5;
}

UserGroupFetcher::~UserGroupFetcher() {
}

void UserGroupFetcher::populate() {
//	manager.get(QNetworkRequest(QUrl("http://twitter.com/statuses/public_timeline.xml")));
	QString endBit="";
	if (state) {
		doHttpBit("https://api.twitter.com/1/"+user+"/lists.xml",user,pass,"name="+myGroup->name);
	} else {
		if (cursor!="")
			endBit = "?cursor="+cursor;
		if (!myGroup) {
			doHttpBit("https://api.twitter.com/1/"+user+"/lists.xml"+endBit,user,pass,"");
		} else {
			doHttpBit("https://api.twitter.com/1/"+myGroup->ownerScreenName+"/"+myGroup->slug+"/members.xml"+endBit,user,pass,"");
		}
	}
	emit startFetching();
	inProgressCount++;
}

void UserGroupFetcher::handleXml(QDomDocument *doc,int type,int req) {
	QDomNode node = doc->firstChild().nextSibling();
	if (node.nodeName()=="lists_list") {
		QDomNodeList children = node.childNodes();
		for(unsigned int x=0;x<children.length();x++) {
			QDomNode child = children.at(x);
			if (child.nodeName()=="lists") {
				QDomNodeList lists = child.childNodes();
				for(unsigned int y=0;y<lists.length();y++) {
					handleListXml(lists.at(y));
				}
			} else if (child.nodeName()=="next_cursor") {
				QString next_cursor = child.firstChild().nodeValue();
				if (next_cursor=="0") {
					minTimeoutSecs=3600;
					cursor="";
				} else {
					minTimeoutSecs=10;
					cursor=next_cursor;
				}
			}
		}
	} else if (node.nodeName()=="list") {
		handleListXml(node);
	}else if (node.nodeName()=="users_list") {
		QDomNodeList children = node.childNodes();
		for(unsigned int x=0;x<children.length();x++) {
			QDomNode child = children.at(x);
			if (child.nodeName()=="users") {
				QDomNodeList users = child.childNodes();
				for(unsigned int y=0;y<users.length();y++) {
					QDomNode node = users.at(y); // GCC needs this var to handle the & on the func below, MSVC doesn't.
					TweetUser *u = handleUserXml(node,false,(Tweet*)0);
					if (u && u->screen_name!="") {
						members.append(u->screen_name);
						//QMessageBox::information (0,"",group()->slug+" - "+((u->is_friend)?"yes":"no"));
					}
				}
			} else if (child.nodeName()=="next_cursor") {
				QString next_cursor = child.firstChild().nodeValue();
				if (next_cursor=="0") {
					myGroup->setGroupMembers(members);
					members.clear();
					cursor="";
					emit haveGroup(myGroup);
					//QMessageBox::information (0,QString::number((long)this),myGroup->id+"-"+QString::number((long)myGroup));
					deleted=true;
					emit fetcherExpired(this);// my work here is done. ;)
				} else
					cursor=next_cursor;
			}
		}
	} else {
		//QMessageBox::information(0,"",node.nodeName());
		emit otherError(this);
		isError.insert(req,true);
	}
	emit endFetching();
	if (inProgressCount)
		inProgressCount--;
	if (type==0)//1==send
		emit updateFinished(0,this);
}

void UserGroupFetcher::handleListXml(QDomNode list) {
//	QMessageBox::information (0,"assdf",list->toString());
	QDomNodeList nodes = list.childNodes();
	Group *g = 0;
	bool creatingGroup = false;
	if (myGroup&&state) {
		creatingGroup=true;
		g=myGroup;
	} else
		g = new Group(box);
	g->twitterList=true;
	g->user=user;
	g->pass=pass;

	for(unsigned int x=0;x<nodes.length();x++) {
		QDomNode node = nodes.at(x);
		if (node.nodeName()=="id") {
			QString id = node.firstChild().nodeValue();
			if (box->getGroupById(id)) {
				Group *oldg = box->getGroupById(id);
				oldg->twitterList=true;
				oldg->name=g->name;
				oldg->full_name=g->full_name;
				oldg->description=g->description;
				oldg->member_count=g->member_count;
				oldg->privateList=g->privateList;
				oldg->slug=g->slug;
				oldg->uri=g->uri;
				oldg->subscriber_count=g->subscriber_count;
				g=oldg;
			}
			g->id=id;
		} else if (node.nodeName()=="name") {
			g->name=node.firstChild().nodeValue();
		} else if (node.nodeName()=="full_name") {
			g->full_name=node.firstChild().nodeValue();
		} else if (node.nodeName()=="slug") {
			g->slug=node.firstChild().nodeValue();
		} else if (node.nodeName()=="description") {
			g->description=node.firstChild().nodeValue();
		} else if (node.nodeName()=="subscriber_count") {
			g->subscriber_count=QVariant(node.firstChild().nodeValue()).toInt();
		} else if (node.nodeName()=="member_count") {
			g->member_count=QVariant(node.firstChild().nodeValue()).toInt();
		} else if (node.nodeName()=="uri") {
			g->uri=node.firstChild().nodeValue();
		} else if (node.nodeName()=="mode") {
			g->privateList=(node.firstChild().nodeValue()!="public");
		} else if (node.nodeName()=="user") {
			TweetUser*u = handleUserXml(node,false,0);
			if (u)
				g->ownerScreenName=u->screen_name;
		}
	}
	if (!creatingGroup) {
/*
		UserGroupFetcher *newFetcher = new UserGroupFetcher();
		newFetcher->setGroup(g);
		newFetcher->setUser(user);
		newFetcher->setPass(pass);
		box->addFetcher(newFetcher,true);
		newFetcher->activateTimeout(15,0);
*/
		g->updateMembers();
		emit haveGroup(g);
	} else {
		//QMessageBox::information(0,"",QString::number(myGroup->groupmembers.count()));
		QStringList members = myGroup->groupmembers;
		myGroup->groupmembers.clear();
		myGroup->updateGroupMembers(members);
		deleted=true;
		emit fetcherExpired(this);// my work here is done. ;)
	}
}

void UserGroupFetcher::setGroup(Group *g) {
	myGroup=g;
	weight=200;
	members.clear();
}

void UserGroupFetcher::promoteGroupToList(Group *g) {
//	QMessageBox::information(0,"","https://api.twitter.com/1/"+user+"/lists.xml");
	myGroup=g;
	state=1;
	populate();
}
