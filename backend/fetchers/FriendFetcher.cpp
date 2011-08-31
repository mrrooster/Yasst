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
/* $Id: FriendFetcher.cpp,v 1.10 2010/06/19 20:29:41 ian Exp $

 File       : FriendFetcher.cpp
 Create date: 00:53:52 1 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "FriendFetcher.h"
#include "TweetFetcher.h"
#include "../oauth/OAuth.h"
#include <QMessageBox>

FriendFetcher::FriendFetcher() : TweetFetcher() {
	apiCount=1;
	state=0;
	minTimeoutSecs=10;
	weight=30;
	type=8;
	pageCount=0;
	QObject::connect(this,SIGNAL(gotUsers(int)),this,SLOT(handleUsers(int)));
}

FriendFetcher::~FriendFetcher() {

}

void FriendFetcher::populate() {
	QString endBit="";
	if (cursor=="")
		endBit="?cursor=-1";
	else
		endBit="?cursor="+cursor;

	switch (state) {
		case 0 ://Logon
			doHttpBit("https://twitter.com/account/verify_credentials.xml",user,pass,"");
			break;
		case 1 ://Fetch friends
			doHttpBit("https://twitter.com/statuses/friends.xml"+endBit,user,pass,"");
			break;
		case 2 ://Fetch friends from start
			doHttpBit("https://twitter.com/statuses/friends.xml?cursor=-1",user,pass,"");
			break;
		case 3 ://Fetch single friend by screename
			doHttpBit("https://api.twitter.com/1/users/show.xml?screen_name="+infoScreenName,"","","");
			break;
	}
}

void FriendFetcher::handleUsers(int x) {
	if (x>0&&cursor!="") {
		minTimeoutSecs=300;
		if(state==2)
			state=1;
		else {
			pageCount+=1;
			if (!(pageCount%10))
				state=2;
		}
	} else {
		minTimeoutSecs=3600;
		pageCount=0;
	}
}

void FriendFetcher::handleXml(QDomDocument *doc,int type,int req) {

	QDomNode node = doc->firstChild().nextSibling();
	if (node.nodeName()=="user") {
		if (!state) {
			state=1;
			populate();
		}
	}
	TweetFetcher::handleXml(doc,type,req);
}

void FriendFetcher::setSingleUserInfo(QString screenName) {
	infoScreenName=screenName;
	state=3;
	singleUsersAreFriends=false;
	periodic=false;
}
