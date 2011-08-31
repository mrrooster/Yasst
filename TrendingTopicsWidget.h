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
#ifndef TRENDINGTOPICSWIDGET_H
#define TRENDINGTOPICSWIDGET_H

#include <QtGui/QWidget>
#include "ui_TrendingTopicsWidget.h"
#include "AbstractTopWidget.h"
#include "backend/fetchers/TrendingTopicsFetcher.h"

class TrendingTopicsWidget : public AbstractTopWidget
{
    Q_OBJECT

public:
    TrendingTopicsWidget(QWidget *parent = 0);
    ~TrendingTopicsWidget();

	void setMode(TrendingTopicsFetcher::Trends);
	bool haveHadCurrent() {return hadCurrent;}
	bool haveHadDaily() {return hadDaily;}
	bool haveHadWeekly() {return hadWeekly;}
	void clearSelection();
public slots:
	void handleTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>);
	void setClearVisibility(bool);

//    Tweet* getTweet() {return tweet;};
private:
	void refreshList();

    Ui::TrendingTopicsWidgetClass ui;
    TrendingTopicsFetcher::Trends mode;
    QList<TrendingTopic> currentTrends;
    QList<TrendingTopic> dailyTrends;
    QList<TrendingTopic> weeklyTrends;

    QString currentName;

    bool hadCurrent;
    bool hadDaily;
    bool hadWeekly;
private slots:
	void handleRowChanged(int);
signals:
    void showCurrentTopics();
    void showDailyTopics();
    void showWeeklyTopics();
    void topicSelected(QString,QString);
    void clearPressed();
};

#endif // TRENDINGTOPICSWIDGET_H
