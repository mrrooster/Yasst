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
/* $Id: TweetFetcher.h,v 1.11 2010/07/20 20:08:15 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef TWEETFETCHER_H_
#define TWEETFETCHER_H_
#include "backend/HTTPHandler.h"
#include "backend/Tweet.h"
#include "backend/TweetUser.h"
#include "backend/Group.h"
#include "backend/oauth/OAuth.h"
#include <QtXml/QDomDocument>
#include <QDateTime>
#include <QFile>

class BirdBox;

class TweetFetcher : public HTTPHandler {
	Q_OBJECT
	friend class DebugTabPanel;
public:
//	static QFile *file;

	TweetFetcher();
	virtual ~TweetFetcher();
	virtual void populate();
	virtual void doLogon() {};
	virtual void populateUsers() {};
	virtual void tweet(QString ,QString ="") {};
	virtual void tweetDirectMessage(QString ,QString ="") {};
	virtual void destroyTweet(Tweet *) {};
	virtual void followUserByScreenName(QString) {};
	virtual void unfollowUserByScreenName(QString) {};
	void addUserToList(QString listId,QString userId);
	void removeUserFromList(QString listId,QString userId);
	void removeListBySlug(QString listSlug);
	void updateListDetails(QString listId,QString name,QString desc, bool privateList);
	void setOAuth(OAuth *);
	void setUser(QString u);
	void setPass(QString p);
	QString getUser() {return user;};
	QString getPass() {return pass;};
	void setBirdBox(BirdBox *);
	BirdBox* getBirdBox();
	void getRate();
	void activateTimeout(int t,int =10);
	QString getLastId();
	void setLastId(QString);
	void setInitialFetchCount(int);
	bool periodic;
	bool deleted;
	int apiCount;
	int weight;
	/* For ref...
	 * 0 - TweetFetcher
	 * 1 - UserTweetFetcher
	 * 2 - UserReplyFetcher
	 * 3 - UserDirectMessageFetcher
	 * 4 - UserFavouriteMessageFetcher
	 * 5 - UserGroupFetcher
	 * 6 - WebSingleTweetFetcher
	 * 7 - SearchFetcher
	 * 8 - FriendFetcher
	 * 9 - FeedFetcher
	 * 10 - TrendingTopicsFetcher
	 * 11 - TwitrpixImageHandler
	 * 12 - TwitterUserTweetsFetcher
	 * 13 - UserGroupTweetFetcher
	 * 14 - UserGroupFollowingFetcher
	 * 256 - (Yasst)Update check
	 */
	int type;
	int inProgressCount;
	QDateTime nextUpdate;
	bool firstFetch;
	int minTimeoutSecs;
protected:
	struct delayedRequest {
		QUrl url;
		QString data;
		QString mimetype;
		QString user;
		QString method;
		int id;
	};
	virtual void handleXml(QDomDocument *doc,int type, int req);
	void handleUsersXml(QDomDocument *doc,bool are_friends);
	int handleStatusesXml(QDomDocument *doc,int type);
	TweetUser* handleUserXml(QDomNode &child,bool is_friend,Tweet *tweet=0);
	void handleHashXml(QDomNode &child);
	QString nodeName(QDomNode &);
	virtual int doHttpBit(QString URL,QString user="",QString pass="", QString data="", QString = "", QString = "");
	virtual int doHttpBit(QUrl URL,QString user="",QString pass="", QString data="", QString = "",QString = "");
	QDateTime parseTwitterDate(QString );

	QList<TweetFetcher::delayedRequest *> pendingRequests;
	QTimer drainTimer;
	bool statusUsersAreFriends;
	QString lastId;
	int statusPage;
	int firstFetchCount;
	QHash<int,bool> isError;
	bool singleUsersAreFriends;
	QString cursor;

public slots:
	//void handleResponse(QNetworkReply *r);
signals:
	void haveUser(TweetUser *);
	void haveUpdatedUser(TweetUser *);
	void haveAuthenticatingUser(TweetUser *);
	void haveTweet(Tweet *);
	void deletedTweet(Tweet *);
	void haveTweets(QList<Tweet*>);
	void haveDirectMessages(QList<Tweet*>);
	void updateFinished(int,TweetFetcher*);
	void rateUpdate(long count,long secondsToReset);
	void limitExceeded();
	void twitterError(QString);
	void gotUsers(int);
	void startFetching();
	void endFetching();
	void imageUploadStarted();
	void imageUploadEnded();
	void failedDirectMessage(QString,QString);
	void failedTweet(QString,QString,QString);
	void fetcherExpired(TweetFetcher *f);
//	void haveGroup(Group*);
protected:
	//void doHttpBit(QString URL,QString user,QString pass, QString data);
	//QNetworkAccessManager manager;
	QString user;
	QString pass;
	OAuth *oauth;
	BirdBox *box;
	int timeoutSecs;
	bool timerActive;
	bool trustUserFollowingStatus;
	QTimer refreshTimer;

protected slots:
	virtual void handleHttpResponse(QString,QUrl,bool,int);
	virtual void handleDone(bool);
	void handleDrainTimerFired();
private slots:
	void doTimeout();
};

#endif /* TWEETFETCHER_H_ */
