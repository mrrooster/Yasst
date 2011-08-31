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
/* $Id: TrendingTopicsListWidget.h,v 1.2 2009/09/23 19:35:51 ian Exp $

 File       : TrendingTopicsListWidget.h
 Create date: 19:03:54 20 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef TRENDINGTOPICSLISTWIDGET_H_
#define TRENDINGTOPICSLISTWIDGET_H_

#include "SearchListWidget.h"
#include "TrendingTopicsWidget.h"
#include "backend/fetchers/TrendingTopicsFetcher.h"
#include <QTimer>

class TrendingTopicsListWidget: public SearchListWidget {
	Q_OBJECT
public:
	TrendingTopicsListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~TrendingTopicsListWidget();
	virtual void writeSettings(QSettings &);
	virtual void readSettings(QSettings &);
protected:
	//virtual bool canDoUpdate(Tweet *);
protected slots:
	virtual void setSearchVisible();
	virtual void close();
private:
	TrendingTopicsFetcher currentFetcher;
	TrendingTopicsFetcher dailyFetcher;
	TrendingTopicsFetcher weeklyFetcher;
	TrendingTopicsWidget *topicsWidget;
	QTimer changeTimer;
	QString newName;
	QString newSearch;
private slots:
	void handleShowCurrentTopics();
	void handleShowDailyTopics();
	void handleShowWeeklyTopics();
	void handleTopicSelected(QString,QString);
	void handleTimeout();
	void handleClear();
};

#endif /* TRENDINGTOPICSLISTWIDGET_H_ */
