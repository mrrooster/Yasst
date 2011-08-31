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
#include "TrendingTopicsWidget.h"
#include <QMessageBox>

TrendingTopicsWidget::TrendingTopicsWidget(QWidget *parent) : AbstractTopWidget(parent) {
	ui.setupUi(this);

	type=2;
	mode=TrendingTopicsFetcher::Current;

	QObject::connect(ui.current,SIGNAL(clicked()),this,SIGNAL(showCurrentTopics()));
	QObject::connect(ui.daily,SIGNAL(clicked()),this,SIGNAL(showDailyTopics()));
	QObject::connect(ui.weekly,SIGNAL(clicked()),this,SIGNAL(showWeeklyTopics()));
	QObject::connect(ui.topics,SIGNAL(currentRowChanged(int)),this,SLOT(handleRowChanged(int)));
	QObject::connect(ui.clear,SIGNAL(clicked()),this,SIGNAL(clearPressed()));

	ui.topics->setMinimumHeight(0);
	ui.topics->setMaximumHeight(0);

	ui.current->setProperty("active","true");

	hadCurrent=false;
	hadDaily=false;
	hadWeekly=false;

	currentName="";
}

TrendingTopicsWidget::~TrendingTopicsWidget()
{
//	QObject::disconnect(&userImageCache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)));
}

void TrendingTopicsWidget::handleTopics(TrendingTopicsFetcher::Trends type,QList<TrendingTopic> topics) {
	//QMessageBox::information (0,"assdf","Hello");
	if (type==TrendingTopicsFetcher::Current) {
		hadCurrent=true;
		currentTrends=topics;
	} else if (type==TrendingTopicsFetcher::Daily) {
		dailyTrends=topics;
		hadDaily=true;
	} else if (type==TrendingTopicsFetcher::Weekly) {
		weeklyTrends=topics;
		hadWeekly=true;
	}
	refreshList();
}


void TrendingTopicsWidget::setMode(TrendingTopicsFetcher::Trends newMode) {
	mode=newMode;
	ui.current->setProperty("active","false");
	ui.daily->setProperty("active","false");
	ui.weekly->setProperty("active","false");
	if (newMode==TrendingTopicsFetcher::Current)
		ui.current->setProperty("active","true");
	else if (newMode==TrendingTopicsFetcher::Daily)
		ui.daily->setProperty("active","true");
	else if (newMode==TrendingTopicsFetcher::Weekly)
		ui.weekly->setProperty("active","true");
	refreshList();
	setStyleSheet("");
}

void TrendingTopicsWidget::handleRowChanged(int row) {
	QList<TrendingTopic> topics=currentTrends;
	if (mode==TrendingTopicsFetcher::Daily)
		topics=dailyTrends;
	else if (mode==TrendingTopicsFetcher::Weekly)
		topics=weeklyTrends;
	if (row>=0 && row<topics.size()) {
		TrendingTopic topic = topics.at(row);
//		QMessageBox::information(this,topic.name,topic.search);
		currentName=topic.name;
		emit topicSelected(topic.name,topic.search);
	}
}

void TrendingTopicsWidget::refreshList() {
	QList<TrendingTopic> topics=currentTrends;
	if (mode==TrendingTopicsFetcher::Daily)
		topics=dailyTrends;
	else if (mode==TrendingTopicsFetcher::Weekly)
		topics=weeklyTrends;

	int height=0;
	ui.topics->clear();
	for(int x=0;x<topics.size();x++) {
		TrendingTopic topic = topics.at(x);
		QString name = QString::number(x+1)+". ";
		QListWidgetItem *item = new QListWidgetItem();
		name+=topic.name;
		item->setText(name);
		if (topic.prevPos==-1)
			item->setIcon(QIcon(":/trendstates/newentry.png"));
		else if (topic.prevPos==x)
			item->setIcon(QIcon(":/trendstates/stick.png"));
		else if (topic.prevPos>x)
			item->setIcon(QIcon(":/trendstates/up.png"));
		else
			item->setIcon(QIcon(":/trendstates/down.png"));

		ui.topics->addItem(item);

		if (currentName==topic.name) {
			item->setSelected(true);
			ui.topics->setCurrentItem(item);
		}
		height += ui.topics->visualItemRect(item).height();
	}
	ui.topics->setMinimumHeight(height);
	ui.topics->setMaximumHeight(height);
}

void TrendingTopicsWidget::setClearVisibility(bool shown) {
	ui.clear->setHidden(!shown);
}

void TrendingTopicsWidget::clearSelection() {
	currentName="";
	ui.topics->clearSelection();
}
