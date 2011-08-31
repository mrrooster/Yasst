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
/* $Id: FeedFetcher.cpp,v 1.5 2010/09/14 13:49:05 ian Exp $

 File       : FeedFetcher.cpp
 Create date: 22:29:03 3 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "FeedFetcher.h"
#include "backend/FeedItem.h"

#include <QList>
#include <QMessageBox>
#include <QDebug>

FeedFetcher::FeedFetcher() : TweetFetcher() {
	apiCount=0;
	minTimeoutSecs=300;
	type=9;
}

FeedFetcher::~FeedFetcher() {
}

void FeedFetcher::populate() {
	if (url!="")
		doHttpBit(url);
}

void FeedFetcher::handleXml(QDomDocument *doc,int,int) {
	QDomNodeList nodes = doc->childNodes();
	for (int x=0;x<nodes.count();x++) {
		QDomNode node = nodes.at(x);
		if (nodeName(node)=="rss") {
			handleRssXml(node);
		} else if(nodeName(node).toLower()=="rdf") {
			handleRdfXml(node);
		} else if(nodeName(node).toLower()=="feed") {
			handleAtomXml(node);
		} else {
			qDebug() << doc->toString();
		}
	}
}

void FeedFetcher::handleRdfXml(QDomNode node) {
	QList<Tweet*> tweets;
	QDomNodeList children = node.childNodes();
	QDateTime startDate = QDateTime::currentDateTime();
	for (int x=0;x<children.count();x++) {
		QDomNode item = children.at(x);
//			QMessageBox::information(0,"Bleugh",item.nodeName());
		if (nodeName(item)=="channel") {
			QDomNode title = item.firstChildElement("title");
			if (!title.isNull())
				emit feedTitle(title.firstChild().nodeValue());
		} else if (nodeName(item)=="item") {
			FeedItem *tweet = new FeedItem();
			tweet->created_at=startDate.addSecs(-x);
			tweet->feed_item_date="";//tweet->created_at.toString();
			tweet->sourceUrl=url;
			QDomNodeList children2 = item.childNodes();
			for (int y=0;y<children2.count();y++) {
				QDomNode childItem = children2.at(y);
				if (nodeName(childItem)=="title") {
					tweet->message=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="guid") {
					tweet->id=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="description") {
					tweet->content=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="link") {
					tweet->url=childItem.firstChild().nodeValue();
					if (tweet->id=="")
						tweet->id=tweet->url;
				} else if (nodeName(childItem)=="pubDate") {
					//Thu Feb 19 00:20:20 +0000 2009
					QString date = childItem.firstChild().nodeValue();
//					tweet->feed_item_date = childItem.firstChild().nodeValue().left(26);
//						QString hour = dateStr.mid(20,5);
//						dateStr=dateStr.left(20)+dateStr.right(4);
//					tweet->created_at=QDateTime::fromString(dateStr,"ddd MMM dd HH:mm:ss +0000 yyyy");
					// Sat, 05 Sep 2009 14:51:00 GMT
					tweet->created_at=parseRdfDate(date.left(25));
				}
			}
			tweets.append(tweet);
		}
	}
	emit haveTweets(tweets);
}

void FeedFetcher::handleRssXml(QDomNode node) {
	QList<Tweet*> tweets;
	QDomNodeList children = node.firstChild().childNodes();
	QDateTime startDate = QDateTime::currentDateTime();
	for (int x=0;x<children.count();x++) {
		QDomNode item = children.at(x);
//			QMessageBox::information(0,"Bleugh",item.nodeName());
		if (nodeName(item)=="title") {
			emit feedTitle(item.firstChild().nodeValue());
		} else if (nodeName(item)=="item") {
			FeedItem *tweet = new FeedItem();
			tweet->created_at=startDate.addSecs(-x);
			tweet->feed_item_date="";//tweet->created_at.toString();
			tweet->sourceUrl=url;
			QDomNodeList children2 = item.childNodes();
			for (int y=0;y<children2.count();y++) {
				QDomNode childItem = children2.at(y);
				if (nodeName(childItem)=="title") {
					tweet->message=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="guid") {
					tweet->id=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="description") {
					tweet->content=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="link") {
					tweet->url=childItem.firstChild().nodeValue();
					if (tweet->id=="")
						tweet->id=tweet->url;
				} else if (nodeName(childItem)=="pubDate") {
					//Thu Feb 19 00:20:20 +0000 2009
					QString date = childItem.firstChild().nodeValue();
//					tweet->feed_item_date = childItem.firstChild().nodeValue().left(26);
//						QString hour = dateStr.mid(20,5);
//						dateStr=dateStr.left(20)+dateStr.right(4);
//					tweet->created_at=QDateTime::fromString(dateStr,"ddd MMM dd HH:mm:ss +0000 yyyy");
					// Sat, 05 Sep 2009 14:51:00 GMT
					tweet->created_at=parseRssDate(date);
				}
			}
			tweets.append(tweet);
		}
	}
	emit haveTweets(tweets);
}

void FeedFetcher::handleAtomXml(QDomNode node) {
	QList<Tweet*> tweets;
	QDomNodeList children = node.childNodes();
	QDateTime startDate = QDateTime::currentDateTime();
//	QMessageBox::information(0,"Bleugh",nodeName(node));
	for (int x=0;x<children.count();x++) {
		QDomNode item = children.at(x);
//			QMessageBox::information(0,"Bleugh",item.nodeName());
		if (nodeName(item)=="title") {
			emit feedTitle(item.firstChild().nodeValue());
		} else if (nodeName(item)=="entry") {
			FeedItem *tweet = new FeedItem();
			tweet->created_at=startDate.addSecs(-x);
			tweet->feed_item_date="";//tweet->created_at.toString();
			tweet->sourceUrl=url;
			QDomNodeList children2 = item.childNodes();
			for (int y=0;y<children2.count();y++) {
				QDomNode childItem = children2.at(y);
				if (nodeName(childItem)=="title") {
					tweet->message=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="id") {
					tweet->id=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="summary") {
					tweet->content=childItem.firstChild().nodeValue();
				} else if (nodeName(childItem)=="link") {
					tweet->url=childItem.attributes().namedItem("href").nodeValue();
					if (tweet->id=="")
						tweet->id=tweet->url;
				} else if (nodeName(childItem)=="updated") {
					//Thu Feb 19 00:20:20 +0000 2009
					QString date = childItem.firstChild().nodeValue();
					//					tweet->feed_item_date = childItem.firstChild().nodeValue();
//						QString hour = dateStr.mid(20,5);
//						dateStr=dateStr.left(20)+dateStr.right(4);
//					tweet->created_at=QDateTime::fromString(dateStr,"ddd MMM dd HH:mm:ss +0000 yyyy");
					// 2009-09-04T22:29:51Z
					tweet->created_at=QDateTime::fromString(date.left(19),"yyyy-MM-ddTHH:mm:ss");
				}
			}
			tweets.append(tweet);
		}
	}
	emit haveTweets(tweets);
}

void FeedFetcher::setUrl(QString u) {
	url=u;
}

QString FeedFetcher::getUrl() {
	return url;
}

QDateTime FeedFetcher::parseRdfDate(QString twitterdate) {
	//Thu Feb 19 00:20:20 +0000 2009
	QStringList parts = twitterdate.split(" ");
	QDateTime ret;

	ret.setTimeSpec(Qt::UTC);
	if (parts.size()==6) {
		QStringList months;
		QDate date;
		months << "jan" << "feb" << "mar" << "apr" << "may" << "jun" << "jul" << "aug" << "sep" << "oct" << "nov" << "dec";
		QString month = parts.at(1).toLower();
		date.setDate(parts.at(5).toInt(),months.indexOf(month)+1,parts.at(2).toInt());
		ret.setDate(date);
		ret.setTime(QTime::fromString(parts.at(3),"HH:mm:ss"));
	}
	return ret;
}

QDateTime FeedFetcher::parseRssDate(QString twitterdate) {
	// Sat, 05 Sep 2009 14:51:00 GMT
	QStringList parts = twitterdate.split(" ");
	QDateTime ret;

	ret.setTimeSpec(Qt::UTC);
	if (parts.size()==6) {
		QStringList months;
		QDate date;
		months << "jan" << "feb" << "mar" << "apr" << "may" << "jun" << "jul" << "aug" << "sep" << "oct" << "nov" << "dec";
		QString month = parts.at(2).toLower();
		date.setDate(parts.at(3).toInt(),months.indexOf(month)+1,parts.at(1).toInt());
		ret.setDate(date);
		ret.setTime(QTime::fromString(parts.at(4),"HH:mm:ss"));
	}
	return ret;
}
