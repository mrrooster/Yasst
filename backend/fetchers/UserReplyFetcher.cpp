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
/* $Id: UserReplyFetcher.cpp,v 1.3 2010/06/19 20:29:41 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/

//Qt
#include <QMessageBox>
//
#include "UserReplyFetcher.h"
#include "../oauth/OAuth.h"
#include "backend/BirdBox.h"

UserReplyFetcher::UserReplyFetcher(): UserTweetFetcher() {
	type=2;
}

UserReplyFetcher::~UserReplyFetcher() {
}

void UserReplyFetcher::populate() {
	QString endbit="";
	if (firstFetch) {
//		endbit="?count=100";
		firstFetch=false;
	}

	doHttpBit("https://api.twitter.com/statuses/replies.xml"+endbit,user,pass,"");
	emit startFetching();
	inProgressCount++;
}

/*
int UserReplyFetcher::doHttpBit(QString URL,QString user,QString pass, QString data, QString type) {
	QString oauth;
	if (box)
		oauth = box->getOAuth()->getAuthorizationHeader(URL,(data=="")?"GET":"POST",data);

	if (oauth!="") {
		headers.insert("Authorization",oauth);
		//QMessageBox::information(0,URL,oauth);
		//QApplication::exit();
	}
	return HTTPHandler::doHttpBit(URL,"","",data,type);
}
*/
