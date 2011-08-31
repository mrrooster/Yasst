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
/* $Id: TweetUser.h,v 1.2 2010/04/19 20:40:07 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef TWEETUSER_H_
#define TWEETUSER_H_

#include <QString>
#include <QVariant>

class TweetUser {
public:
	TweetUser();
	virtual ~TweetUser();

	QString id;
	QString search_id;
	QString name;
	QString screen_name;
	QString description;
	QString location;
	QString image_url;
	QString url;
	long followers_count;
	long friends_count;
	bool is_friend;
	bool am_unfollowing;
};
Q_DECLARE_METATYPE(TweetUser)
#endif /* TWEETUSER_H_ */
