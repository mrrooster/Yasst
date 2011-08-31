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
/* $Id: UserTweetFetcher.h,v 1.6 2010/06/23 21:21:41 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/

#ifndef USERTWEETFETCHER_H_
#define USERTWEETFETCHER_H_

#include "TweetFetcher.h"
#include <QString>

class UserTweetFetcher: public TweetFetcher {
	Q_OBJECT
public:
	UserTweetFetcher();
	virtual ~UserTweetFetcher();
	void populate();
	void populateUsers();
	void doLogon();
	void tweet(QString ,QString ="");
	void retweet(QString);
	void tweetDirectMessage(QString ,QString ="");
	void destroyTweet(Tweet*);
	void favoriteTweet(Tweet*);
	void unfavoriteTweet(Tweet*);
	void followUserByScreenName(QString name);
	void unfollowUserByScreenName(QString name);
protected:
	bool firstFetch;
	int pageCount;
	QString tmpLastId;

protected slots:
	virtual void handleUsers(int);
	virtual void httpFinished(int,bool);
private:
	QHash<int,bool> isDirectMessage;
	QHash<int,QString> sentTweet;
	QHash<int,QString> sentInReplyTo;
};


#endif /* USERTWEETFETCHER_H_ */
