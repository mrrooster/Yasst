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
/* $Id: BirdBox.cpp,v 1.14 2010/07/20 20:08:15 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/

#include "BirdBox.h"
#include "BirdSong.h"
#include "FreeFetcherThread.h"
#include "TweetPrunerThread.h"
#include "fetchers/FriendFetcher.h"
#include <QMessageBox>
#include <QTimer>
#include <QMutexLocker>
#include <QMutableHashIterator>
#include <QHashIterator>
#include <QMetaType>
#include <QUuid>

BirdBox::BirdBox() : pruneCount(0),thread(0),freeThread(0),tweetPruneThread(0),updateCount(0),updateSize(0),totalUpdates(0),myUser(0),fetcherLock(QMutex::Recursive),freeFetcherLock(QMutex::Recursive),tweetsLock(QMutex::Recursive),directMessagesLock(QMutex::Recursive),tweetsIterator(tweets),rememberMe(true),removed(false) {
	apiCount=0;
	apiRefresh=QDateTime::currentDateTime();
	startTime=QDateTime::currentDateTime();
	firstFetchCount=0;
	QObject::connect(&oauth,SIGNAL(PINRequest(OAuth *,QString)),this,SIGNAL(PINRequest(OAuth *,QString)));
	QObject::connect(&oauth,SIGNAL(authenticatedUsername(QString)),this,SLOT(setUser(QString)));
}

BirdBox::~BirdBox() {
	if (thread)
		thread->deleteLater();
	if (freeThread)
		freeThread->deleteLater();
	if (tweetPruneThread)
		tweetPruneThread->deleteLater();
}

void BirdBox::getRate() {
	TweetFetcher *f = getFetcher(8);
	if (f) {
		f->getRate();
	}
}

void BirdBox::addFetcher(TweetFetcher *f,bool populate) {
	QMutexLocker fetcherLocker(&fetcherLock);
	QMutexLocker freeFetcherLocker(&freeFetcherLock);

	QObject::connect(f,SIGNAL(haveTweets(QList<Tweet*>)),this,SLOT(tweetsUpdate(QList<Tweet*>)));
	QObject::connect(f,SIGNAL(haveDirectMessages(QList<Tweet*>)),this,SLOT(directMessagesUpdate(QList<Tweet*>)));
	QObject::connect(f,SIGNAL(haveTweet(Tweet*)),this,SLOT(tweetUpdate(Tweet*)));
	QObject::connect(f,SIGNAL(deletedTweet(Tweet*)),this,SIGNAL(deleteTweet(Tweet*)));
	QObject::connect(f,SIGNAL(haveUser(TweetUser*)),this,SLOT(userUpdate(TweetUser*)));
	QObject::connect(f,SIGNAL(haveUpdatedUser(TweetUser*)),this,SIGNAL(haveUpdatedUser(TweetUser*)));
	QObject::connect(f,SIGNAL(haveAuthenticatingUser(TweetUser*)),this,SLOT(handleAuthenticatingUser(TweetUser*)));
	QObject::connect(f,SIGNAL(authError(HTTPHandler*)),this,SIGNAL(authError(HTTPHandler*)));
	QObject::connect(f,SIGNAL(otherError(HTTPHandler*)),this,SIGNAL(otherError(HTTPHandler*)));
	QObject::connect(f,SIGNAL(updateFinished(int,TweetFetcher*)),this,SLOT(updateFinished(int,TweetFetcher*)));
	QObject::connect(f,SIGNAL(rateUpdate(long,long)),this,SIGNAL(rateUpdate(long,long)));
	QObject::connect(f,SIGNAL(limitExceeded()),this,SIGNAL(limitExceeded()));
	QObject::connect(f,SIGNAL(startFetching()),this,SIGNAL(startFetching()));
	QObject::connect(f,SIGNAL(endFetching()),this,SIGNAL(endFetching()));
	QObject::connect(f,SIGNAL(endFetching()),this,SLOT(handleUpdateFinished()));
	QObject::connect(f,SIGNAL(imageUploadStarted()),this,SIGNAL(imageUploadStarted()));
	QObject::connect(f,SIGNAL(imageUploadEnded()),this,SIGNAL(imageUploadEnded()));
	QObject::connect(f,SIGNAL(failedTweet(QString,QString,QString)),this,SIGNAL(failedTweet(QString,QString,QString)));
	QObject::connect(f,SIGNAL(failedDirectMessage(QString,QString)),this,SIGNAL(failedDirectMessage(QString,QString)));
	QObject::connect(f,SIGNAL(haveGroup(Group*)),this,SLOT(handleGroup(Group*)));
	QObject::connect(f,SIGNAL(fetcherExpired(TweetFetcher *)),this,SLOT(handleDeleteFetcher(TweetFetcher *)));
	f->setBirdBox(this);

	if (f->apiCount==0) {
		freeFetchers.append(f);
		if (!freeThread) {
			freeThread = (QThread*)new FreeFetcherThread(this);
			freeThread->start();
		}
	} else {
		fetchers.append(f);
		if (!thread) {
			thread = (QThread*)new BirdSong(this);
			thread->start();
//			emit rateUpdate(100,(60*60));
		}
		if (username!="") {
			f->setUser(username);
			f->setPass(password);
			f->setOAuth(&oauth);
		}
	}
	if (populate) {
		f->populate();
		firstFetchCount++;
	}
	emit fetcherAdded(f);
}

void BirdBox::removeFetcher(TweetFetcher *f) {
	QMutexLocker fetcherLocker(&fetcherLock);
	QMutexLocker freeFetcherLocker(&freeFetcherLock);
	if (f->apiCount)
		fetchers.removeOne(f);
	else
		freeFetchers.removeOne(f);
	QObject::disconnect(f,SIGNAL(haveTweets(QList<Tweet*>)),this,SLOT(tweetsUpdate(QList<Tweet*>)));
	QObject::disconnect(f,SIGNAL(haveDirectMessages(QList<Tweet*>)),this,SLOT(directMessagesUpdate(QList<Tweet*>)));
	QObject::disconnect(f,SIGNAL(haveTweet(Tweet*)),this,SLOT(tweetUpdate(Tweet*)));
	QObject::disconnect(f,SIGNAL(deletedTweet(Tweet*)),this,SIGNAL(deleteTweet(Tweet*)));
	QObject::disconnect(f,SIGNAL(haveUser(TweetUser*)),this,SLOT(userUpdate(TweetUser*)));
	QObject::disconnect(f,SIGNAL(haveUpdatedUser(TweetUser*)),this,SIGNAL(haveUpdatedUser(TweetUser*)));
	QObject::disconnect(f,SIGNAL(haveAuthenticatingUser(TweetUser*)),this,SLOT(handleAuthenticatingUser(TweetUser*)));
	QObject::disconnect(f,SIGNAL(authError(HTTPHandler*)),this,SIGNAL(authError(HTTPHandler*)));
	QObject::disconnect(f,SIGNAL(otherError(HTTPHandler*)),this,SIGNAL(otherError(HTTPHandler*)));
	QObject::disconnect(f,SIGNAL(updateFinished(int,TweetFetcher*)),this,SLOT(updateFinished(int,TweetFetcher*)));
	QObject::disconnect(f,SIGNAL(rateUpdate(long,long)),this,SIGNAL(rateUpdate(long,long)));
	QObject::disconnect(f,SIGNAL(limitExceeded()),this,SIGNAL(limitExceeded()));
	QObject::disconnect(f,SIGNAL(startFetching()),this,SIGNAL(startFetching()));
	QObject::disconnect(f,SIGNAL(endFetching()),this,SIGNAL(endFetching()));
	QObject::disconnect(f,SIGNAL(endFetching()),this,SLOT(handleUpdateFinished()));
	QObject::disconnect(f,SIGNAL(imageUploadStarted()),this,SIGNAL(imageUploadStarted()));
	QObject::disconnect(f,SIGNAL(imageUploadEnded()),this,SIGNAL(imageUploadEnded()));
	QObject::disconnect(f,SIGNAL(failedTweet(QString,QString,QString)),this,SIGNAL(failedTweet(QString,QString,QString)));
	QObject::disconnect(f,SIGNAL(failedDirectMessage(QString,QString)),this,SIGNAL(failedDirectMessage(QString,QString)));
	QObject::disconnect(f,SIGNAL(haveGroup(Group*)),this,SLOT(handleGroup(Group*)));
	QObject::disconnect(f,SIGNAL(fetcherExpired(TweetFetcher *)),this,SLOT(handleDeleteFetcher(TweetFetcher *)));
}

void BirdBox::updateFinished(int ,TweetFetcher* f) {
	QMutexLocker fetcherLocker(&fetcherLock);
	updateCount--;
//	updateSize+=count;
	if (updateCount<0)
		updateCount=0;
	statusChange(updateCount,fetchers.size()+freeFetchers.size(),updateSize);
	if (updateCount==0) {
		updateSize=0;
		totalUpdates=0;
	}
	if (!f->periodic) {
		removeFetcher(f);
		f->deleteLater();
	}
	if (firstFetchCount) {
		firstFetchCount--;
		if (firstFetchCount==0) {
			emit finishedFirstFetch();
		}
	}
}

void BirdBox::removeAllFetchers() {
	QMutexLocker fetcherLocker(&fetcherLock);
	QMutexLocker freeFetcherLocker(&freeFetcherLock);
	TweetFetcher *f;

	while(fetchers.size()>0) {
		f=fetchers.at(0);
		removeFetcher(f);
		f->deleteLater();
	}
	while(freeFetchers.size()>0) {
		f=freeFetchers.at(0);
		removeFetcher(f);
		f->deleteLater();
	}
}

void BirdBox::tweetUpdate(Tweet *t) {
	pendingTweets.append(t);
	drainTweets();
}

void BirdBox::tweetsUpdate(QList<Tweet*> tweets) {
//	if (pendingTweets.size()==0)
//		QTimer::singleShot(0, this, SLOT(drainTweets()));
	pendingTweets.append(tweets);
	drainTweets();
}

void BirdBox::directMessagesUpdate(QList<Tweet*> tweets) {
//	if (pendingTweets.size()==0)
//		QTimer::singleShot(0, this, SLOT(drainTweets()));
	pendingDirectMessages.append(tweets);
	drainDirectMessages();
}

void BirdBox::drainTweets() {
//	QMessageBox::information (0,"assdf","I'm draining..."+pendingTweets.at(0)->html_message);
	QMutexLocker tweetsLocker(&tweetsLock);
	QList<Tweet*> tweets;

	for (int x=0;x<pendingTweets.size();x++) {
		Tweet *t=pendingTweets.at(x);
		if (!this->tweets.contains(t->id)) {
			this->tweets.insert(t->id,t);
			updateSize++;
		} else {
			Tweet *tweet = this->tweets.value(t->id);
			tweet->message=t->message;
			tweet->html_message=t->html_message;
			tweet->in_reply_to_screen_name=t->in_reply_to_screen_name;
			tweet->in_reply_to_status_id=t->in_reply_to_status_id;
			tweet->in_reply_to_user_id=t->in_reply_to_user_id;
			tweet->user_id=t->user_id;
			tweet->favorite=t->favorite;
			tweet->stamp();
			delete t;
			t=tweet;
		}
		if (!t->deleted)
			tweets.append(t);
	}
	pendingTweets.clear();

	if (!tweetPruneThread) {
		tweetPruneThread=(QThread*)new TweetPrunerThread(this);
		tweetPruneThread->start();
	}

	for(int x=0;x<tweets.size();x++)
		tweets.at(x)->refCount++;
	emit haveTweets(tweets);
	for(int x=0;x<tweets.size();x++)
		tweets.at(x)->refCount--;
//	if (pendingTweets.size()>0)
//		QTimer::singleShot(0, this, SLOT(drainTweets()));
}

void BirdBox::drainDirectMessages() {
	QMutexLocker directMessagesLocker(&directMessagesLock);
//	QMessageBox::information (0,"assdf","I'm draining..."+pendingTweets.at(0)->html_message);
	QList<Tweet*> tweets;

	for (int x=0;x<pendingDirectMessages.size();x++) {
		Tweet *t=pendingDirectMessages.at(x);
		if (!this->directMessages.contains(t->id)) {
			this->directMessages.insert(t->id,t);
			updateSize++;
		} else {
			Tweet *tweet = this->directMessages.value(t->id);
			tweet->message=t->message;
			tweet->html_message=t->html_message;
			tweet->in_reply_to_screen_name=t->in_reply_to_screen_name;
			tweet->in_reply_to_status_id=t->in_reply_to_status_id;
			tweet->in_reply_to_user_id=t->in_reply_to_user_id;
			tweet->user_id=t->user_id;
			delete t;
			t=tweet;
		}
		tweets.append(t);
	}
	pendingDirectMessages.clear();
	emit haveDirectMessages(tweets);

//	if (pendingTweets.size()>0)
//		QTimer::singleShot(0, this, SLOT(drainTweets()));
}

void BirdBox::userUpdate(TweetUser *u) {
	if (usersbyscreename.contains(u->screen_name.toLower())) {
//		delete u;
		return;
	}
	if (u->id!="")
		users.insert(u->id,u);
	usersbyscreename.insert(u->screen_name.toLower(),u);
	emit haveUser(u);
}

void BirdBox::doUpdate() {
	updateCount=0;
	totalUpdates=fetchers.size()+freeFetchers.size();
	for(int x=0;x<fetchers.size();x++) {
		TweetFetcher *f = fetchers[x];
		if (f->deleted) {
			removeFetcher(f);
			f->deleteLater();
			x--;
		}
		if (f->periodic) {
			f->populate();
			updateCount++;
	//		totalUpdates++;
			statusChange(updateCount,fetchers.size()+freeFetchers.size(),updateSize);
		}
	}
}

void BirdBox::doFreeUpdate() {
	totalUpdates=fetchers.size()+freeFetchers.size();
	for(int x=0;x<freeFetchers.size();x++) {
		if (freeFetchers[x]->deleted) {
			TweetFetcher *f = freeFetchers[x];
			removeFetcher(f);
			f->deleteLater();
			x--;
		}
		if (freeFetchers[x]->periodic) {
			freeFetchers[x]->populate();
			updateCount++;
	//		totalUpdates++;
			statusChange(updateCount,fetchers.size()+freeFetchers.size(),updateSize);
		}
	}
}

int BirdBox::activeFetchersCount() {
	int x,count=0;
	for(x=0;x<fetchers.size();x++)
		count+=fetchers[x]->inProgressCount;
	for(x=0;x<freeFetchers.size();x++)
		count+=freeFetchers[x]->inProgressCount;
	return count;
}

void BirdBox::refresh() {
	doUpdate();
	doFreeUpdate();
}

QList<TweetUser*> BirdBox::getUsers() {
	QMap<QString,TweetUser*>::iterator i = usersbyscreename.begin();
	QList<TweetUser*> ret;
	while (i!=usersbyscreename.end()) {
		ret.append(i.value());
		i++;
	}

	return ret;
}

QList<Tweet*> BirdBox::getTweets() {
	QMutexLocker tweetsLocker(&tweetsLock);
	QHashIterator <QString,Tweet*> i(tweets);
	QList<Tweet*> ret;
	while (i.hasNext()) {
		i.next();
		Tweet *t=i.value();
		ret.append(t);
		t->refCount++;
	}
	return ret;
}

QList<Tweet*> BirdBox::getDirectMessages() {
	QMutexLocker directMessagesLocker(&directMessagesLock);
	QHashIterator <QString,Tweet*> i(directMessages);
	QList<Tweet*> ret;
	while (i.hasNext()) {
		i.next();
		Tweet *t=i.value();
		ret.append(i.value());
		t->refCount++;
	}
	return ret;
}

Tweet* BirdBox::getTweet(QString id) {
	QMutexLocker tweetsLocker(&tweetsLock);
	return tweets.value(id);
}

bool BirdBox::hasTweet(QString id) {
	QMutexLocker tweetsLocker(&tweetsLock);
	return tweets.contains(id);
}

TweetUser* BirdBox::getUser(QString id) {
	return users.value(id);
}

TweetUser* BirdBox::getUserByScreenName(QString screenName,bool fetch) {
	TweetUser *u =usersbyscreename.value(screenName.toLower());
	if (!u && fetch) {
		FriendFetcher *f = new FriendFetcher();
		f->setSingleUserInfo(screenName);
		this->addFetcher(f,true);
	}
	return u;
}

unsigned long BirdBox::getUserCount(bool getFriends) {
	if (!getFriends)
		return usersbyscreename.count();
	QMap<QString,TweetUser*>::iterator i = usersbyscreename.begin();
	unsigned long ret=0;
	while (i!=usersbyscreename.end()) {
		if (i.value()->is_friend)
			ret++;
		i++;
	}
	return ret;
}

void BirdBox::handleAuthenticatingUser(TweetUser *u) {
	this->myUser=u;
	emit authChanged();
}

QString BirdBox::getAuthenticatingUserScreenName() {
	if (myUser)
		return myUser->screen_name;
	else return username;
}

QMutableHashIterator <QString,Tweet*>* BirdBox::getTweetsIterator() {
	tweetsLock.lock();
	tweetsIterator.toFront();
	return &tweetsIterator;
}

void BirdBox::releaseTweetsIterator() {
	tweetsLock.unlock();
}

int BirdBox::fetchersCount() {
	QMutexLocker fetcherLocker(&fetcherLock);
	return fetchers.count();
}

int BirdBox::freeFetchersCount() {
	QMutexLocker fetcherLocker(&freeFetcherLock);
	return freeFetchers.count();
}

TweetFetcher* BirdBox::fetchersAt(int x) {
	QMutexLocker fetcherLocker(&fetcherLock);
	return fetchers.at(x);
}

/*
TweetFetcher *BirdBox::getAuthenticatingFetcher() {
	QMutexLocker fetcherLocker(&fetcherLock);
	TweetFetcher *f;
	int count = fetchers.count();
	for (int x=0;myUser&&x<count;x++) {
		f=fetchers.at(x);
		if (f->getUser()==myUser->screen_name)
			return f;
	}
	return 0;
}
*/

/*
void BirdBox::registerGroup(QObject* group) {
	groups.append(group);
}

void BirdBox::unregisterGroup(QObject* group) {
	groups.removeOne(group);
}

*/
QList<Group*> BirdBox::getGroups() {
	QMutexLocker groupsLocker(&groupsLock);
	return groups;
}

void BirdBox::handleGroup(Group *g) {
	QMutexLocker groupsLocker(&groupsLock);

	if (groups.contains(g)) {
		groupsById.remove(groupsById.key(g));
		groupsByLocalId.remove(groupsByLocalId.key(g));
	} else {
		if (groupsById.contains(g->id)) {
			delete g;
			return;
		}
		groups.append(g);
		QObject::connect(g,SIGNAL(groupUpdated(Group*)),this,SIGNAL(groupUpdated(Group*)));
		emit groupUpdated(g);
	}

	if (g->id!="")
		groupsById.insert(g->id,g);
	if (g->local_id!="")
		groupsByLocalId.insert(g->local_id,g);
}

Group* BirdBox::getGroupById(QString id) {
	QMutexLocker groupsLocker(&groupsLock);
	return groupsById.value(id);
}

Group* BirdBox::getGroupByLocalId(QString id) {
	QMutexLocker groupsLocker(&groupsLock);
	return groupsByLocalId.value(id);
}

void BirdBox::removeGroup(Group *g) {
	QMutexLocker groupsLocker(&groupsLock);
	groups.removeOne(g);
	groupsById.remove(g->id);
	groupsByLocalId.remove(g->local_id);
	QObject::disconnect(g,SIGNAL(groupUpdated(Group*)),this,SIGNAL(groupUpdated(Group*)));
}

void BirdBox::handleDeleteFetcher(TweetFetcher *f) {
	removeFetcher(f);
	f->deleteLater();
}

void BirdBox::handleUpdateFinished() {
	TweetFetcher *f = qobject_cast<TweetFetcher*>(sender());
	if (!f->periodic) {
		handleDeleteFetcher(f);
	}
}

void BirdBox::setUser(QString u) {
	QMutexLocker fetcherLocker(&fetcherLock);
	if (u!=username) {
		username = u;
		oauth.setUser(u);
//		oauth.authorize();
		for(int x=0;x<fetchers.count();x++)
			fetchers.at(x)->setUser(u);
		emit authChanged();
	}
}

void BirdBox::setPassword(QString p) {
	if (p!=password) {
		password=p;
//		oauth.setPass(p);
		for(int x=0;x<fetchers.count();x++)
			fetchers.at(x)->setPass(p);
	}
}

QString BirdBox::getUser() {
	return username;
}

QString BirdBox::getPassword() {
	return password;
}

bool BirdBox::hasCredentials() {
	return (username!="" || oauth.getAuthToken()!="");
}

void BirdBox::setRememberMe(bool eh) {
	rememberMe = eh;
}

bool BirdBox::getRememberMe() {
	return rememberMe;
}

UserDirectMessageFetcher* BirdBox::getDirectMessageFetcher() {
	return (UserDirectMessageFetcher*)getFetcher(3);
}

UserTweetFetcher* BirdBox::getUserFetcher() {
	return (UserTweetFetcher*)getFetcher(1);
}

TweetFetcher* BirdBox::getFetcher(int type) {
	QMutexLocker fetcherLocker(&fetcherLock);

	for(int x=0;x<fetchers.count();x++) {
		if (fetchers.at(x)->type==type)
			return fetchers.at(x);
	}
	return 0;
}

void BirdBox::deleteAccount() {
	removed=true;
	emit accountRemoved();
}

bool BirdBox::isDeleted() {
	return removed;
}

QString BirdBox::getId() {
	if (id=="")
		id = QUuid::createUuid().toString();
	return id;
}

void BirdBox::setId(QString newId) {
	id=newId;
}

void BirdBox::pruneReadTweets() {
	QMutableHashIterator <QString,Tweet*> *i=getTweetsIterator();

	while (i->hasNext()) {
		i->next();
		Tweet *t=i->value();
		if (t->read) {
			emit deleteTweet(t);
		}
	}
	releaseTweetsIterator();
}

void BirdBox::addHashTag(QString s) {
	if (!hashtags.contains(s,Qt::CaseInsensitive))
		hashtags.append(s);
}

QStringList BirdBox::getHashTags() {
	return hashtags;
}

OAuth* BirdBox::getOAuth() {
	return &oauth;
}
