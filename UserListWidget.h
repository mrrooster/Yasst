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
/* $Id: UserListWidget.h,v 1.3 2009/09/02 20:44:09 ian Exp $

 File       : UserListWidget.h
 Create date: 21:47:51 20-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef USERLISTWIDGET_H_
#define USERLISTWIDGET_H_

#include "tweetlistwidget.h"
#include "backend/Tweet.h"
#include "backend/TweetUser.h"
#include "UserWidget.h"
#include "backend/fetchers/TwitterUserTweetsFetcher.h"

class UserListWidget: public TweetListWidget {
	Q_OBJECT
public:
	UserListWidget(QWidget *parent = 0);
	UserListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~UserListWidget();

	void setUser(TweetUser *u);
	void setUser(QString screen_name);
public slots:
	void writeSettings(QSettings &);
	void readSettings(QSettings &);

protected:
	virtual bool canDoUpdate(Tweet *);
private:
	TweetUser *user;
	UserWidget *userWidget;
	Tweet t;
	TwitterUserTweetsFetcher *fetcher;
	//QString screen_name;
private slots:
	void handleTweetUser(TweetUser*);
	void handleSearch();
};

#endif /* USERLISTWIDGET_H_ */
