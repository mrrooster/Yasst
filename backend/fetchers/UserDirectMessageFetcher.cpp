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
/* $Id: UserDirectMessageFetcher.cpp,v 1.5 2010/07/25 14:51:22 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "UserDirectMessageFetcher.h"
#include <QMessageBox>

UserDirectMessageFetcher::UserDirectMessageFetcher(): UserTweetFetcher() {
	weight=50;
	type=3;
}

UserDirectMessageFetcher::~UserDirectMessageFetcher() {
}

void UserDirectMessageFetcher::populate() {
	QString endbit="";
	if (firstFetch) {
//		endbit="?count=100";
		firstFetch=false;
	}

	doHttpBit("https://twitter.com/direct_messages.xml"+endbit,user,pass,"");
	emit startFetching();
	inProgressCount++;
}

void UserDirectMessageFetcher::handleXml(QDomDocument *doc,int isSend,int req) {
	int count;
	QDomNode node = doc->firstChild().nextSibling();

	if (node.nodeName()=="direct-messages"||node.nodeName()=="direct_message") {
		emit endFetching();
		inProgressCount--;
		count = handleDirectMessagesXml(doc);
		if (!isSend)
			emit updateFinished(count,this);
	} else
		UserTweetFetcher::handleXml(doc,isSend,req);
}

int UserDirectMessageFetcher::handleDirectMessagesXml(QDomDocument *doc) {
	QDomNodeList  nodes = doc->elementsByTagName("direct_message");
	unsigned int x;
	int count=0;
	QList<Tweet*> tweets;

	for(x=0;x<nodes.length();x++) {
		QDomNode node = nodes.item(x);
		QDomNodeList children = node.childNodes();
		Tweet *tweet = new Tweet();
		tweet->direct_message=true;

		for (unsigned int y=0;y<children.length();y++){
			QDomNode child = children.item(y);
			if (child.nodeName()=="text") {
				tweet->message = child.firstChild().nodeValue();
			} else if (child.nodeName()=="id") {
				tweet->id = child.firstChild().nodeValue();
			} else if (child.nodeName()=="sender_id") {
				tweet->in_reply_to_user_id = child.firstChild().nodeValue();
			} else if (child.nodeName()=="sender_screen_name") {
				tweet->in_reply_to_screen_name = child.firstChild().nodeValue();
			} else if (child.nodeName()=="sender") {
				handleUserXml(child,false,tweet);
			} else if (child.nodeName()=="created_at") {
				//Thu Feb 19 00:20:20 +0000 2009
				tweet->created_at=parseTwitterDate(child.firstChild().nodeValue());
			}
		}
		//QMessageBox::information (0,"assdf",tweet->created_at.toString());

		if (tweet->id!="") {
			count++;
			//emit haveTweet(tweet);
			tweets.append(tweet);
		}
	}
	emit haveDirectMessages(tweets);
	return count;
}
