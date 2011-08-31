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
/* $Id: TrendingTopicsListWidget.cpp,v 1.4 2009/09/26 20:30:11 ian Exp $

 File       : TrendingTopicsListWidget.cpp
 Create date: 19:03:55 20 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "TrendingTopicsListWidget.h"
#include <QMessageBox>

TrendingTopicsListWidget::TrendingTopicsListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : SearchListWidget(b,i,parent) {
	type=9;
	ui.title->setToolTip("");
	ui.title->setObjectName("trends");
	setTitle("Trending topics");
	ui.type->setPixmap(QPixmap(":/buttons/trendingtopics.png"));
	hideTopText();
	currentFetcher.setMode(TrendingTopicsFetcher::Current);
	dailyFetcher.setMode(TrendingTopicsFetcher::Daily);
	weeklyFetcher.setMode(TrendingTopicsFetcher::Weekly);
	b->addFetcher(&currentFetcher);
	b->addFetcher(&dailyFetcher);
	b->addFetcher(&weeklyFetcher);
	currentFetcher.populate();
	dailyFetcher.populate();
	weeklyFetcher.populate();
	topicsWidget=new TrendingTopicsWidget(this);
	changeTimer.setInterval(1000);

	QObject::connect(&changeTimer,SIGNAL(timeout()),this,SLOT(handleTimeout()));
	QObject::connect(topicsWidget,SIGNAL(showCurrentTopics()),this,SLOT(handleShowCurrentTopics()));
	QObject::connect(topicsWidget,SIGNAL(showDailyTopics()),this,SLOT(handleShowDailyTopics()));
	QObject::connect(topicsWidget,SIGNAL(showWeeklyTopics()),this,SLOT(handleShowWeeklyTopics()));
	QObject::connect(topicsWidget,SIGNAL(clearPressed()),this,SLOT(handleClear()));
	QObject::connect(topicsWidget,SIGNAL(topicSelected(QString,QString)),this,SLOT(handleTopicSelected(QString,QString)));

	((QBoxLayout*)ui.tweets->layout())->insertWidget(0,topicsWidget);

	QObject::connect(&currentFetcher,SIGNAL(haveTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>)),topicsWidget,SLOT(handleTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>)));
	QObject::connect(&dailyFetcher,SIGNAL(haveTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>)),topicsWidget,SLOT(handleTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>)));
	QObject::connect(&weeklyFetcher,SIGNAL(haveTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>)),topicsWidget,SLOT(handleTopics(TrendingTopicsFetcher::Trends,QList<TrendingTopic>)));

	topicsWidget->setClearVisibility(false);
	tweetClamp=150;
}

TrendingTopicsListWidget::~TrendingTopicsListWidget() {
	tweets->removeFetcher(&currentFetcher);
	tweets->removeFetcher(&dailyFetcher);
	tweets->removeFetcher(&weeklyFetcher);
}

void TrendingTopicsListWidget::setSearchVisible() {
}

void TrendingTopicsListWidget::close() {
	//emit searchClosed(ui.title->text(),ui.topText->text());
//	tweets->removeFetcher(&searcher);
}

void TrendingTopicsListWidget::readSettings(QSettings &settings) {
	TweetListWidget::readSettings(settings);
//	setSearch(settings.value("search","").toString(),settings.value("title","").toString());
}

void TrendingTopicsListWidget::writeSettings(QSettings &settings) {
	TweetListWidget::writeSettings(settings);
	settings.setValue("type",9);
}

void TrendingTopicsListWidget::handleShowCurrentTopics() {
//	fetcher.setMode(TrendingTopicsFetcher::Current);
	if (!topicsWidget->haveHadCurrent())
		currentFetcher.populate();
	topicsWidget->setMode(TrendingTopicsFetcher::Current);
//	fetcher.populate();
}

void TrendingTopicsListWidget::handleShowDailyTopics() {
//	fetcher.setMode(TrendingTopicsFetcher::Daily);
	if (!topicsWidget->haveHadDaily())
		dailyFetcher.populate();
	topicsWidget->setMode(TrendingTopicsFetcher::Daily);
//	fetcher.populate();
}

void TrendingTopicsListWidget::handleShowWeeklyTopics() {
//	fetcher.setMode(TrendingTopicsFetcher::Weekly);
	if (!topicsWidget->haveHadWeekly())
		weeklyFetcher.populate();
	topicsWidget->setMode(TrendingTopicsFetcher::Weekly);
//	fetcher.populate();
}

void TrendingTopicsListWidget::handleTopicSelected(QString name,QString search) {
	newSearch=search;
	newName=name;
	changeTimer.start();
	topicsWidget->setClearVisibility(true);
}

void TrendingTopicsListWidget::handleTimeout() {
	searcher.setInitialFetchCount(20);
	changeTimer.stop();
	setSearch(newSearch,newName);
	setTitle(tr("Topic: ")+newName);
}

void TrendingTopicsListWidget::handleClear() {
	removeAllTweets(true);
	refresh();
	searcher.setSearch("");
	setTitle("Trending topics");
	topicsWidget->setClearVisibility(false);
	topicsWidget->clearSelection();
}
