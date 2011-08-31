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
 * SearchFetcher.cpp
 *
 *  Created on: 12-Mar-2009
 *      Author: ian
 */

#include "SearchFetcher.h"
#include <QStringList>
#include <QMessageBox>
#include <QRegExp>
#include "backend/BirdBox.h"
#include "backend/json/JSONParser.h"

SearchFetcher::SearchFetcher(): TweetFetcher() {
	sinceId="";
	setInitialFetchCount(100);
	type=7;
}

SearchFetcher::~SearchFetcher() {
}

void SearchFetcher::populate() {
	if (search=="")
		return;
	//refreshUrl="";
	QUrl url("http://search.twitter.com/search.json");
	if (firstFetch)
		url.addQueryItem("rpp",QString::number(firstFetchCount));
	if (sinceId!="")
		url.addQueryItem("since_id",sinceId);
	url.addQueryItem("q",search);

//	QMessageBox::information (0,"assdf",sinceId+":"+url.toString());
	doHttpBit(url);
	emit startFetching();
	inProgressCount++;
}

void SearchFetcher::handleHttpResponse(QString resp,QUrl ,bool isSend,int) {
	QRegExp deLt("&lt;");
	QRegExp deGt("&gt;");
	QRegExp deQuot("&quot;");
	int otherPos;
	bool newUser;
	QList<Tweet*> tweets;
	QList<QString> tweetIds;

	emit endFetching();
	inProgressCount--;
	//QMessageBox::information (0,"assdf",resp);
	JSONComplexObject *bob = JSONParser::parse(resp);
	if (!bob) {
		emit otherError(this);
		return;
	}
//	QMessageBox::information (0,"assdf",bob->toString());
	JSONArray *results = (JSONArray*)bob->values.value("results");
	if (results) {
		for(int x=0;x<results->items.length();x++) {
			Tweet *t=new Tweet();
			TweetUser *u=new TweetUser();
			u->is_friend=false;
			newUser=true;
			JSONComplexObject *result = (JSONComplexObject*)results->items.at(x);
			JSONObject *thing;

			thing=result->values.value("text");
			if (thing && thing->type==JSONObject::JSONString) {
				t->message=((JSONString*)thing)->get();
			}
			thing=result->values.value("from_user");
			if (thing && thing->type==JSONObject::JSONString) {
				u->screen_name=t->screen_name=((JSONString*)thing)->get();
			}
			thing=result->values.value("id");
			if (thing && thing->type==JSONObject::JSONString) {
				t->id=((JSONString*)thing)->get();
			}
			thing=result->values.value("from_user_id");
			if (thing && thing->type==JSONObject::JSONString) {
				QString text=((JSONString*)thing)->get();
				t->user_id=u->id=u->search_id=text;
				if (box->getUserByScreenName(u->screen_name)) {
					QString name=u->screen_name;
					delete u;
					u=box->getUserByScreenName(name);
					u->search_id=text;
					t->user_id=u->id;
					newUser=false;
				}
			}
			thing=result->values.value("source");
			if (thing && thing->type==JSONObject::JSONString) {
				QString text=((JSONString*)thing)->get();
				while ((otherPos=deLt.indexIn(text,0))>=0)
					text=text.left(otherPos)+"<"+text.mid(otherPos+4,-1);
				while ((otherPos=deGt.indexIn(text,0))>=0)
					text=text.left(otherPos)+">"+text.mid(otherPos+4,-1);
				while ((otherPos=deQuot.indexIn(text,0))>=0)
					text=text.left(otherPos)+"\""+text.mid(otherPos+6,-1);
				t->source=text;
			}
			thing=result->values.value("profile_image_url");
			if (thing && thing->type==JSONObject::JSONString) {
				u->image_url=((JSONString*)thing)->get();
			}
			thing=result->values.value("created_at");
			if (thing && thing->type==JSONObject::JSONString) {
				t->created_at=parseSearchDate(((JSONString*)thing)->get());
			}
			if (newUser)
				emit haveUser(u);
			tweets.append(t);
			tweetIds.append(t->id);
		}
	}

	JSONObject *since_id = bob->values.value("max_id");
	if (since_id && since_id->type==JSONObject::JSONString) {
		sinceId=((JSONString*)since_id)->get();
	}

	delete bob;

	emit haveTweets(tweets);
	emit haveResults(tweetIds);
	if (!isSend) {
		emit updateFinished(0,this);
		firstFetch=false;
	}
}

void SearchFetcher::setSearch(QString search) {
	this->search=search;
	firstFetch=true;
	sinceId="";
}

QString SearchFetcher::getSearch() {
	return search;
}

QDateTime SearchFetcher::parseSearchDate(QString twitterdate) {
	//ddd, dd MMM yyyy HH:mm:ss +0000

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
