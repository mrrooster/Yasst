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
/* $Id: TrendingTopicsFetcher.cpp,v 1.8 2010/06/23 21:21:41 ian Exp $

 File       : TrendingTopicsFetcher.cpp
 Create date: 03:09:20 20 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "TrendingTopicsFetcher.h"
#include <QMessageBox>
#include <QRegExp>
#include <QTimer>
#include <QDebug>
#include "backend/BirdBox.h"
#include "backend/json/JSONParser.h"

TrendingTopicsFetcher::TrendingTopicsFetcher(): TweetFetcher() {
	mode=TrendingTopicsFetcher::Current;
	minTimeoutSecs=60;
	type=10;
}

TrendingTopicsFetcher::~TrendingTopicsFetcher() {
}

void TrendingTopicsFetcher::populate() {
	QUrl url;
	if (mode==TrendingTopicsFetcher::Current) {
		url="http://search.twitter.com/trends/current.json";
	} else if (mode==TrendingTopicsFetcher::Daily) {
		url="http://search.twitter.com/trends/daily.json";
	} else {
		url="http://search.twitter.com/trends/weekly.json";
	}

//	QMessageBox::information (0,"assdf",url.toString());
	this->doHttpBit(url);
	emit startFetching();
	inProgressCount++;
}

void TrendingTopicsFetcher::handleHttpResponse(QString resp,QUrl ,bool isSend,int) {
//	qDebug() << "Http response [] :\n" <<resp;
	emit endFetching();
	inProgressCount--;
	JSONComplexObject *bob = JSONParser::parse(resp);

	if (!bob) {
		QTimer::singleShot(10000,this,SLOT(populate()));
		emit otherError(this);
		return;
	}

	//QMessageBox::information (0,"assdf",bob->toString());
	JSONComplexObject *trendsHolder = (JSONComplexObject*)bob->values.value("trends");
	if (trendsHolder && trendsHolder->type==JSONObject::JSONComplexObject) {
		JSONArray *trends = (JSONArray*)trendsHolder->values.values().at(0);

		if (trends && trends->type==JSONObject::JSONArray) {
			QList<TrendingTopic> newTopics;
			for(int x=0;x<trends->items.length();x++) {
				if (trends->items.at(x)->type==JSONObject::JSONComplexObject) {
					JSONComplexObject *trend = (JSONComplexObject*)trends->items.at(x);
					TrendingTopic thisTopic;
					JSONObject *thing;

					thing=trend->values.value("query");
					if (thing && thing->type==JSONObject::JSONString) {
						thisTopic.search=((JSONString*)thing)->get();
					}
					thing=trend->values.value("name");
					if (thing && thing->type==JSONObject::JSONString) {
						thisTopic.name=((JSONString*)thing)->get();
					}
					for (int y=0;y<topics.length();y++) {
						if (topics.at(y).search==thisTopic.search) {
							thisTopic.prevPos=y;
							break;
						}
					}
					newTopics.append(thisTopic);
				}
			}
			emit haveTopics(mode,newTopics);
			topics=newTopics;
		} else {
			QTimer::singleShot(5000,this,SLOT(populate()));
			emit otherError(this);
		}
	} else {
		QTimer::singleShot(5000,this,SLOT(populate()));
		emit otherError(this);
	}
	delete bob;

	if (!isSend) {
		emit updateFinished(0,this);
		firstFetch=false;
		minTimeoutSecs=1800;
		if (mode==TrendingTopicsFetcher::Current)
			minTimeoutSecs=300;
	}
}

void TrendingTopicsFetcher::setMode(TrendingTopicsFetcher::Trends newMode) {
	mode=newMode;
	minTimeoutSecs=1800;
	if (newMode==TrendingTopicsFetcher::Current)
		minTimeoutSecs=300;
	if (firstFetch)
		minTimeoutSecs=60;
	topics.clear();
}

