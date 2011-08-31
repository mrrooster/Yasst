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
/* $Id: BirdBox.h,v 1.16 2010/07/20 20:08:15 ian Exp $
File:   BirdBox.h
(c) 2009 Ian Clark
*/

#ifndef BIRDBOX_H_
#define BIRDBOX_H_

#include <QObject>
#include <QMultiMap>
#include <QHash>
#include <QList>
#include <QThread>
#include <QMutex>
#include "Group.h"
#include "Tweet.h"
#include "TweetUser.h"
#include "fetchers/TweetFetcher.h"
#include "fetchers/UserDirectMessageFetcher.h"
#include "fetchers/UserTweetFetcher.h"
#include "oauth/OAuth.h"

class BirdBox : public QObject{
	Q_OBJECT
	/*friend class TweetPrunerThread;*/
	friend class FreeFetcherThread;
	friend class FetcherThread;
	friend class BirdSong;
	friend class DebugTabPanel;
public:
	BirdBox();
	virtual ~BirdBox();
	void addFetcher(TweetFetcher *,bool populate=false);
	void removeFetcher(TweetFetcher *);
	void setPassword(QString);
	QString getUser();
	QString getPassword();
	bool hasCredentials();
	//TweetFetcher *getAuthenticatingFetcher();
	Tweet *getTweet(QString id);
	bool hasTweet(QString id);
	TweetUser *getUser(QString id);
	TweetUser *getUserByScreenName(QString screenName,bool fetch=false);
	unsigned long getUserCount(bool);
	QList<TweetUser*> getUsers();
	QList<Tweet*> getTweets();
	QList<Tweet*> getDirectMessages();
	void removeAllFetchers();
	void getRate();
	QString getAuthenticatingUserScreenName();
	QMutableHashIterator <QString,Tweet*>* getTweetsIterator();
	void releaseTweetsIterator();
	QDateTime nextFreeUpdate;
	QDateTime nextPrune;
	QDateTime apiRefresh;
	QDateTime startTime;
	int apiCount;
	unsigned long pruneCount;
    int fetchersCount();
    int freeFetchersCount();
    int activeFetchersCount();
    TweetFetcher* fetchersAt(int);
//    void registerGroup(QObject*);
//    void unregisterGroup(QObject*);
    QList<Group*> getGroups();
    Group* getGroupById(QString);
    Group* getGroupByLocalId(QString);
    void removeGroup(Group*);
    void setRememberMe(bool);
    bool getRememberMe();
    UserDirectMessageFetcher *getDirectMessageFetcher();
    UserTweetFetcher* getUserFetcher();
    TweetFetcher* getFetcher(int);
    void deleteAccount();
    bool isDeleted();
    QString getId();
    void setId(QString);
	void pruneReadTweets();
	void addHashTag(QString);
	QStringList getHashTags();
	OAuth* getOAuth();
public slots:
	void setUser(QString);
	void userUpdate(TweetUser *);
	void tweetUpdate(Tweet *);
//	void usersUpdate(QList<TweetUser *>);
	void tweetsUpdate(QList<Tweet*>);
	void directMessagesUpdate(QList<Tweet*>);
	void doUpdate();
	void doFreeUpdate();
	void refresh();
	void handleGroup(Group*);
signals:
	void haveUser(TweetUser *);
	void haveUpdatedUser(TweetUser *);
	void haveTweet(Tweet *);
	void deleteTweet(Tweet *);
	void handleDeleteTweet(Tweet *);
	void setFavorite(Tweet *);
	void clearFavorite(Tweet *);
	void haveTweets(QList<Tweet*>&);
	void haveDirectMessages(QList<Tweet*>&);
	void reply(Tweet *t);
	void retweet(Tweet *t,bool);// true - do a proper retweet, false - do old style 'RT @someone...'
	void directMessage(Tweet *t);
	void authError(HTTPHandler*);
	void otherError(HTTPHandler*);
	void statusChange(int,int,int);
	void rateUpdate(long count,long secondsToReset);
	void limitExceeded();
	void doSearch(QString);
	void followUserByScreenName(QString);
	void unfollowUserByScreenName(QString);
	void startFetching();
	void endFetching();
	void finishedFirstFetch();
	void imageUploadStarted();
	void imageUploadEnded();
	void showUserDetails(TweetUser *);
	void showUserDetails(QString);
	void failedDirectMessage(QString,QString);
	void failedTweet(QString,QString,QString);
	void groupUpdated(Group*);
	void authChanged();
	void accountRemoved();
	void fetcherAdded(TweetFetcher*);
	void tweetUpdated(Tweet*); // This is never emitted by the backend, should be hooked by the UI
	void PINRequest(OAuth *,QString);
private:
	QList<TweetFetcher*> fetchers;
	QList<TweetFetcher*> freeFetchers;
	QStringList hashtags;
	QString username;
	QString password;
	QMultiMap<unsigned int,Tweet*> dateline;
	QThread *thread;
	QThread *freeThread;
	QThread *tweetPruneThread;
	QList<Tweet*> pendingTweets;
	QList<Tweet*> pendingDirectMessages;
	int updateCount;
	int updateSize;
	int totalUpdates;
	TweetUser *myUser;
	QMutex fetcherLock;
	QMutex freeFetcherLock;
	QMutex tweetsLock;
	QMutex directMessagesLock;
	QMutex groupsLock;
	QHash<QString,Tweet*> tweets;
	QHash<QString,Tweet*> directMessages;
	QHash<QString,TweetUser*> users;
	QMap<QString,TweetUser*> usersbyscreename;
	QMutableHashIterator <QString,Tweet*> tweetsIterator;
	int firstFetchCount;
	QList<Group*> groups;
	QHash<QString,Group*> groupsById;
	QHash<QString,Group*> groupsByLocalId;
	bool rememberMe;
	bool removed;
	QString id;
	OAuth oauth;

private slots:
	void updateFinished(int count,TweetFetcher *f);
	void handleAuthenticatingUser(TweetUser*);
	void drainTweets();
	void drainDirectMessages();
	void handleDeleteFetcher(TweetFetcher *f);
	void handleUpdateFinished();
};


#endif /* BIRDBOX_H_ */
