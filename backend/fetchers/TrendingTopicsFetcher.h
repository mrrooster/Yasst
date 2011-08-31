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
/* $Id: TrendingTopicsFetcher.h,v 1.2 2010/09/26 22:27:28 ian Exp $

 File       : TrendingTopicsFetcher.h
 Create date: 03:09:20 20 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef TRENDINGTOPICSFETCHER_H_
#define TRENDINGTOPICSFETCHER_H_

#include "TweetFetcher.h"
#include "../TrendingTopic.h"

class TrendingTopicsFetcher: public TweetFetcher {
	Q_OBJECT
public:
	enum Trends {Current,Daily,Weekly,Location};
	TrendingTopicsFetcher();
	virtual ~TrendingTopicsFetcher();
	void populate();
	void setSearch(QString search);
	void setMode(TrendingTopicsFetcher::Trends);

protected:
	virtual void handleHttpResponse(QString,QUrl,bool,int);
	//void handleHttpResponse(QString resp,QUrl url,bool isSend);

	long location;

private:
	TrendingTopicsFetcher::Trends mode;
	QList<TrendingTopic> topics;

signals:
	void haveTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>);
};
#endif /* TRENDINGTOPICSFETCHER_H_ */
