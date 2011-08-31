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
/* $Id: UserFavouriteMessageFetcher.cpp,v 1.3 2010/06/23 21:21:41 ian Exp $

 File       : UserFavoriteMessageFetcher.cpp
 Create date: 01:18:07 26-Jul-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "UserFavoriteMessageFetcher.h"
#include <QMessageBox>

UserFavoriteMessageFetcher::UserFavoriteMessageFetcher(): UserTweetFetcher() {
	weight=50;
	type=4;
}

UserFavoriteMessageFetcher::~UserFavoriteMessageFetcher() {
}

void UserFavoriteMessageFetcher::populate() {
	QString endbit="";
	if (firstFetch) {
//		endbit="?count=100";
		firstFetch=false;
	}

	doHttpBit("https://twitter.com/favorites.xml"+endbit,user,pass,"");
	emit startFetching();
	inProgressCount++;
}

