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
/* $Id: Tweet.h,v 1.4 2010/03/04 23:48:16 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef TWEET_H_
#define TWEET_H_

#include <QString>
#include <QVariant>
#include <QDateTime>

class Tweet {
public:
	Tweet();
	virtual ~Tweet();
	void stamp();

	QString id;
	QString message;
	QString retweet_message;
	QString html_message;
	QString user_id;
	QString screen_name;
	QString in_reply_to_status_id;
	QString retweet_in_reply_to_status_id;
	QString in_reply_to_user_id;
	QString retweet_in_reply_to_user_id;
	QString in_reply_to_screen_name;
	QString retweet_in_reply_to_screen_name;
	QString image_url;
	QDateTime created_at;
	QString source;
	QString retweet_source;
	bool direct_message;
	bool deleted;
	bool favorite;
	bool retweet;
	bool read;
	QString retweet_screen_name;
	QString fetcher_id;
	QDateTime timestamp;
	int refCount;
	QString getTextSource();
	int type;
};
Q_DECLARE_METATYPE(Tweet)
#endif /* TWEET_H_ */
