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
/* $Id: UserGroupTweetFetcher.cpp,v 1.2 2010/07/20 20:08:15 ian Exp $

 File       : UserGroupTweetFetcher.cpp
 Create date: 00:27:50 7 Mar 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#include "UserGroupTweetFetcher.h"
#include "../Group.h"
#include <QMessageBox>

UserGroupTweetFetcher::UserGroupTweetFetcher() : UserTweetFetcher() {
	trustUserFollowingStatus=false;
	statusUsersAreFriends=false;
	weight=20;
	type=13;
}

UserGroupTweetFetcher::~UserGroupTweetFetcher() {
}

void UserGroupTweetFetcher::populate() {
	QString endbit;

	if (lastId!="") {
//		endbit="?count=200&since_id="+tmpLastId;
		endbit="?since_id="+lastId;
	}

	if (statusPage>1)
		endbit="?page="+QString::number(statusPage);

	//QMessageBox::information(0,"","https://api.twitter.com/1/"+user+"/lists/"+(list->slug)+"/statuses.xml");

	doHttpBit("https://api.twitter.com/1/"+list->ownerScreenName+"/lists/"+list->slug+"/statuses.xml"+endbit,user,pass,"");
	if (statusPage==1) {
		emit startFetching();
		inProgressCount++;
		tmpLastId=lastId;

	}
	statusPage++;
}

void UserGroupTweetFetcher::setGroup(Group *list) {
	this->list=list;
}

Group* UserGroupTweetFetcher::getGroup() {
	return list;
}
