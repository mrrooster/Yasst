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
/* $Id: TwitrpixImageHandler.cpp,v 1.9 2010/08/01 18:08:24 ian Exp $

 File       : TwitrpixImageHandler.cpp
 Create date: 20:12:03 31-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "TwitrpixImageHandler.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include "../BirdBox.h"

#include <QDebug>

TwitrpixImageHandler::TwitrpixImageHandler() {
	periodic=false;
	source="<a href=\"http://twitrpix.com/\">TwitrPix</a>";
	seperator=" ";
	type=11;
	apiCount=1;
}

TwitrpixImageHandler::~TwitrpixImageHandler() {
}

void TwitrpixImageHandler::tweet(QString msg,QString, QString image) {
	QFile file(image);
	QFileInfo info(file);
	if (!file.exists()||!okToPost())
		return;

	// OAuth
	OAuth *oauth = box->getOAuth();
	if (oauth) {
		QString authurl = "https://api.twitter.com/1/account/verify_credentials.json";
		headers.insert("X-Auth-Service-Provider",authurl);
		headers.insert("X-Verify-Credentials-Authorization",oauth->getAuthorizationHeader(authurl,"GET","","http://api.twitter.com/"));
	}
	//debuging
	//headers.insert("Accept-Encoding"," ");

	multipartFormData.insert("api_key","768822d8e4683117b55fc4957208e8");
	//multipartFormData.insert("key","40c8f14d3022f29ddfcb8589d9191c8b");
	multipartFormData.insert("message",msg);
	if (image!="")
		multipartFormFiles.insert("media",image);

	this->msg=msg;

//	int req = doHttpBit(QUrl("http://api.twitpic.com/2/upload.xml"),"","",data,"multipart/form-data; boundary="+boundry);
	int req = doHttpBit(QUrl("http://api.twitrpix.com/2/upload.xml"));
	sentTweet.insert(req,msg);
	sentImage.insert(req,image);
	emit imageUploadStarted();
}

void TwitrpixImageHandler::handleXml(QDomDocument *doc,int,int req) {
	QDomNodeList children = doc->firstChild().nextSibling().childNodes();
	QString text = sentTweet.value(req);
	if (text.isNull()||text.isEmpty())
		text = this->msg;

	for (unsigned int y=0;y<children.length();y++){
		QDomNode child = children.item(y);
		if (child.nodeName()=="url") {
			QString tweetText = child.firstChild().nodeValue()+seperator+text;
			if (box->getUserFetcher())
				box->getUserFetcher()->tweet(tweetText);
			break;
		}
		if (child.nodeName()=="media") {
			QDomNodeList moreChildren = child.childNodes();
			for(unsigned int x=0;x<moreChildren.length();x++) {
				QDomNode otherChild = moreChildren.item(x);
				if (otherChild.nodeName()=="url") {
					QString tweetText = otherChild.firstChild().nodeValue()+seperator+text;
					if (box->getUserFetcher())
						box->getUserFetcher()->tweet(tweetText);
					break;
				}
			}
		}
	}
	emit imageUploadEnded();
	emit updateFinished(1,this);
}

bool TwitrpixImageHandler::okToPost() {
	QSettings settings;

	if (settings.value("accounts/twitrpix/showntncs",false).toBool())
		return true;
	if (QMessageBox::question(0,"TwitrPix image upload","<style>a{color:green; font-weight:bold; text-decoration: none;}</style><p>You are about to upload an image using the <a href='http://www.twitrpix.com'>TwitrPix</a> image upload service. This will send <a href='http://www.twitrpix.com'>TwitrPix</a> your twitter username and password.</p><p>By using this service you agree to the <a href='http://www.twitrpix.com'>TwitrPix</a> terms and conditions, which can be found on their <a href='http://twitrpix.com/terms'>website</a>.</p><p>Do you want to continue?</p>",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes) {
		settings.setValue("accounts/twitrpix/showntncs",true);
		return true;
	}
	return false;
}


void TwitrpixImageHandler::httpFinished(int req,bool error) {
	if (sentTweet.contains(req)) {
		QString sentText = sentTweet.take(req);
		QString sentImage = this->sentImage.take(req);
		if (error) {
			emit imageUploadEnded();
			emit failedTweet(sentText,"",sentImage);
		}
	}
	TweetFetcher::httpFinished(req,error);
}
