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
/* $Id: UserTweetFetcher.cpp,v 1.11 2010/07/20 20:08:15 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "UserTweetFetcher.h"
#include <QMessageBox>
#include <QBuffer>
#include <QtXml/QDomDocument>
#include <QByteArray>
#include "backend/BirdBox.h"
#include <QMessageBox>


#include <QDebug>

UserTweetFetcher::UserTweetFetcher() : TweetFetcher() {
	firstFetch=true;
	QObject::connect(this,SIGNAL(gotUsers(int)),this,SLOT(handleUsers(int)));
	pageCount=0;
	apiCount=1;
	statusUsersAreFriends=true;
	tmpLastId="";
	type=1;
}

UserTweetFetcher::~UserTweetFetcher() {
}


void UserTweetFetcher::populate() {
	QString endbit="";
	if (firstFetch) {
		endbit="?count="+QString::number(firstFetchCount);
		firstFetch=false;
	}
	if (lastId!="") {
//		endbit="?count=200&since_id="+tmpLastId;
		endbit="?since_id="+lastId;
	}

	if (statusPage>1)
		endbit="?page="+QString::number(statusPage);

	if (statusPage==1) {
		emit startFetching();
		inProgressCount++;
		tmpLastId=lastId;

	}
	doHttpBit("https://api.twitter.com/statuses/home_timeline.xml"+endbit,user,pass,"");
	statusPage++;
}

void UserTweetFetcher::populateUsers() {
	QString endBit="";
	if (pageCount>0)
		endBit="?page="+QString::number(pageCount+1);
	doHttpBit("https://twitter.com/statuses/friends.xml"+endBit,user,pass,"");
}

void UserTweetFetcher::handleUsers(int x) {
	if (x==100 && pageCount<10) {
		pageCount+=1;
		populateUsers();
	} else
		pageCount=0;
}

void UserTweetFetcher::doLogon() {
	doHttpBit("https://twitter.com/account/verify_credentials.xml",user,pass,"");
}

void UserTweetFetcher::tweet(QString origMessage, QString reply_id) {
	QString message = "source=yasst&status="+QUrl::toPercentEncoding(origMessage);
	if (reply_id!="")
		message += "&in_reply_to_status_id="+reply_id;

	int req = doHttpBit("https://twitter.com/statuses/update.xml",user,pass,message);
	isDirectMessage.insert(req,false);
	sentTweet.insert(req,origMessage);
	sentInReplyTo.insert(req,reply_id);
}

void UserTweetFetcher::retweet(QString message_id) {
	if (message_id=="")
		return;
	QString message = "source=yasst";

//	QMessageBox::information (0,"assdf","https://twitter.com/statuses/retweet/"+message_id+tr(".xml"));
	doHttpBit("https://twitter.com/statuses/retweet/"+message_id+tr(".xml"),user,pass,message);
//	isDirectMessage.insert(req,false);
//	sentTweet.insert(req,origMessage);
//	sentInReplyTo.insert(req,reply_id);
}

void UserTweetFetcher::tweetDirectMessage(QString origMessage, QString screen_name) {
	QString message = "source=yasst&text="+QUrl::toPercentEncoding(origMessage);
	if (screen_name!="")
		message += "&user="+screen_name;
	//QMessageBox::information (0,"assdf",message);
	int req = doHttpBit("https://twitter.com/direct_messages/new.xml",user,pass,message);
	isDirectMessage.insert(req,true);
	sentTweet.insert(req,origMessage);
	sentInReplyTo.insert(req,screen_name);
}

void UserTweetFetcher::destroyTweet(Tweet *t) {
	if (!t)
		return;

	QString id=t->id;
	if (t->direct_message)
		doHttpBit("https://twitter.com/direct_messages/destroy/"+id+".xml",user,pass,"id="+id);
	else
		doHttpBit("https://twitter.com/statuses/destroy/"+id+".xml",user,pass,"id="+id);
	t->deleted=true;
	emit deletedTweet(t);
}

void UserTweetFetcher::favoriteTweet(Tweet *t) {
	if (!t)
		return;
	doHttpBit("https://twitter.com/favorites/create/"+t->id+".xml",user,pass," ");
}

void UserTweetFetcher::unfavoriteTweet(Tweet *t) {
	if (!t)
		return;
	doHttpBit("https://twitter.com/favorites/destroy/"+t->id+".xml",user,pass," ");
}

void UserTweetFetcher::followUserByScreenName(QString name) {
	doHttpBit("https://twitter.com/friendships/create.xml?follow=true&screen_name="+name,user,pass," ");
}

void UserTweetFetcher::unfollowUserByScreenName(QString name) {
	doHttpBit("https://twitter.com/friendships/destroy.xml?screen_name="+name,user,pass," ");
}

void UserTweetFetcher::httpFinished(int req,bool error) {
	// API Limit check
	QString rateLimit = getResponseHeader(req,"X-RateLimit-Remaining");
	QString rateReset = getResponseHeader(req,"X-RateLimit-Reset");
//	QMessageBox::information(0,rateLimit,rateReset);
	if (rateLimit!="") {
		long secondsToReset = rateReset.toLong()-QDateTime::currentDateTime().toTime_t();
		emit rateUpdate(rateLimit.toLong(),(secondsToReset<60?60:secondsToReset));
	}
	TweetFetcher::httpFinished(req,error);
	if (isError.contains(req))
		error = error || isError.take(req);
	if (sentTweet.contains(req)) {
		QString sentText = sentTweet.take(req);
		QString sentUser = sentInReplyTo.take(req);
		bool directMessage = isDirectMessage.take(req);
		if (error) {
			if (directMessage)
				emit failedDirectMessage(sentText,sentUser);
			else
				emit failedTweet(sentText,sentUser,"");
		}
	}
}

