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
/* $Id: TwitterUserTweetsFetcher.cpp,v 1.4 2010/06/23 21:21:41 ian Exp $

 File       : TwitterUserTweetsFetcher.cpp
 Create date: 18:58:10 16-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "TwitterUserTweetsFetcher.h"

TwitterUserTweetsFetcher::TwitterUserTweetsFetcher(): UserTweetFetcher() {
	statusUsersAreFriends=false;
	weight=50;
	type=12;
}

TwitterUserTweetsFetcher::~TwitterUserTweetsFetcher() {
}

void TwitterUserTweetsFetcher::populate() {
	QString endbit="";
	if (firstFetch) {
		endbit="?count=50";
		firstFetch=false;
	}
	if (id!="") {
		if (endbit!="")
			endbit+="&";
		else
			endbit="?";
		endbit+="screen_name="+id;
	}

	doHttpBit("https://twitter.com/statuses/user_timeline.xml"+endbit,user,pass,"");
	emit startFetching();
	inProgressCount++;
}

void TwitterUserTweetsFetcher::setFetchScreenName(QString id) {
	this->id=id;
}

