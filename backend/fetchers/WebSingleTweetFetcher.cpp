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
/*
 * WebSingleTweetFetcher.cpp
 *
 *  Created on: 03-Mar-2009
 *      Author: ian
 */

#include "WebSingleTweetFetcher.h"
#include "backend/BirdBox.h"
#include <QMessageBox>

WebSingleTweetFetcher::WebSingleTweetFetcher() : TweetFetcher() {
	periodic=false;
	preText="";
	postText="";
	type=6;
}

WebSingleTweetFetcher::~WebSingleTweetFetcher() {
}

void WebSingleTweetFetcher::populate() {
}

void WebSingleTweetFetcher::fetchSingleTweet(QString user,QString userId,QString id) {
	QString url = "http://twitter.com/"+user+"/status/"+id;

	this->userId=userId;
	userScreenName=user;
	tweetId=id;
	doHttpBit(url,"","","");

}

void WebSingleTweetFetcher::handleHttpResponse(QString page,QUrl ,bool,int ) {
	QRegExp contentStart("<span class=\"entry-content\">");
	QRegExp contentEnd("</span>");
	QRegExp profileImage("class=\"thumb\"><a[^>]+><img[^>]+src=\"([^\"]+)");
	QRegExp protectedProfileImage("class=\"thumb\">[^<]+<img[^>]+src=\"([^\"]+)");
	QRegExp protectedUpdate("<div class=\"protected-box\">");
	Tweet *t=new Tweet();
	TweetUser *u = new TweetUser();
	int pos,endPos;
	bool newUser=true;

	u->id = userId;
	u->is_friend = false;
	t->screen_name = u->screen_name = userScreenName;
	t->id = tweetId;
	t->user_id=u->id;

	if (protectedUpdate.indexIn(page)!=-1) {
		t->html_message="This twitterer has protected their updates. You can request to look into their brain on their <a href=\"http://twitter.com/"+userScreenName+"\">profile page.</a>";
		if (protectedProfileImage.indexIn(page)) {
			u->image_url=protectedProfileImage.cap(1);
		}
	} else {
		if ((pos=contentStart.indexIn(page))) {
			if ((endPos=contentEnd.indexIn(page,pos))) {
				t->html_message=preText+page.mid(pos,endPos-pos)+postText;
			}
		}
		if (profileImage.indexIn(page)) {
			u->image_url=profileImage.cap(1);
		}
	}
	if (box->getUserByScreenName(u->screen_name)) {
		QString name=u->screen_name;
		delete u;
		u=box->getUserByScreenName(name);
		t->user_id=u->id;
		newUser=false;
	}
	if (newUser)
		emit haveUser(u);
	emit haveTweet(t);
	emit updateFinished(1,this);
}

void WebSingleTweetFetcher::handleError(HTTPHandler*) {
	fetchSingleTweet(userScreenName,this->userId,tweetId);
}
