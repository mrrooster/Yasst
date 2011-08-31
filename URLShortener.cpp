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
 * URLShortener.cpp
 *
 *  Created on: 28-Feb-2009
 *      Author: ian
 */

#include "URLShortener.h"
#include <QMessageBox>
#include <QSettings>

URLShortener::URLShortener() {
}

URLShortener::~URLShortener() {
}

void URLShortener::shortenURL(QString url) {
	// User: yasst
	// APIKey: R_33552a93dda4f93c800dc111e2fe2c9e
	QUrl bitly("http://api.j.mp/shorten?version=2.0.1&format=xml");
	QSettings opts;

	QString user = opts.value("accounts/bitly/user","").toString();
	QString userAPI = opts.value("accounts/bitly/APIkey","").toString();

	if (user=="" || userAPI=="") {
		user = "yasst";
		userAPI = "R_33552a93dda4f93c800dc111e2fe2c9e";
	} else
		bitly.addQueryItem("history","1");

	bitly.addQueryItem("login",user);
	bitly.addQueryItem("apiKey",userAPI);

	bitly.addQueryItem("longUrl",url); //QUrl::toPercentEncoding(url,"/:")
//	QMessageBox::information (0,"assdf",bitly.toString());
//	emit shortURL(url,bitly.toString());
	doHttpBit(bitly);
}

void URLShortener::httpFinished(int i,bool error) {
	QBuffer *buff;
//	reqLock.lock();

//	QMessageBox::information (0,"assdf",QString::number(i));
	if (myReq.contains(i)) {
		buff=myReq.take(i);
		if (!error) {
			QDomDocument *doc = new QDomDocument();
			doc->setContent(buff->data());
			handleXml(doc);
			delete doc;
		}
		buff->deleteLater();
	}
//	reqLock.unlock();
}

void URLShortener::handleHttpResponse(QString data,QUrl,bool,int) {
	QDomDocument *doc = new QDomDocument();
	doc->setContent(data);
	handleXml(doc);
	delete doc;
}

void URLShortener::handleXml(QDomDocument *doc) {
	QDomNodeList  nodes = doc->elementsByTagName("nodeKeyVal");
	unsigned int x;
	QString longUrl="",shortUrl="";

	for (x=0;x<nodes.length();x++) {
		QDomNode node = nodes.item(x);
		QDomNodeList children = node.childNodes();
		for (unsigned int y=0;y<children.length();y++){
			QDomNode child = children.item(y);

			if (child.nodeName()=="nodeKey") {
				longUrl=child.firstChild().nodeValue();
			} else if (child.nodeName()=="shortUrl") {
				shortUrl=child.firstChild().nodeValue();
			}
		}
	}
	if (longUrl!="" && shortUrl!="")
		emit shortURL(QUrl::fromPercentEncoding(HTTPHandler::htmlDecode(longUrl).toAscii()),shortUrl);
}
