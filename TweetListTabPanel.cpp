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
 * TweetListTabPanel.cpp
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#include "TweetListTabPanel.h"
#include <QMessageBox>
#include <QWidget>

TweetListTabPanel::TweetListTabPanel(QWidget *parent) : TabPanel(parent) {
	type=0;
	QWidget *c = new QWidget(this);
//	QScrollArea *area = new QScrollArea();
//	tabs->addTab(area,tr("Tweets"));

	setWidget(c);
	layout=new QHBoxLayout();
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	layout->setAlignment(Qt::AlignLeft);
	layout->setMargin(0);
	layout->setSpacing(0);
	setWidgetResizable(true);
	c->setLayout(layout);
	c->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
	c->setObjectName(tr("tweetlisttabpanel"));

	drainTimer.setSingleShot(false);
	QObject::connect(&drainTimer,SIGNAL(timeout()),this,SLOT(handleDrainTimer()));
	drainTimer.start(200);
}

TweetListTabPanel::~TweetListTabPanel() {
	TweetListWidget *w;
	layout->deleteLater();
	while(layout->count()) {
		w = (TweetListWidget*)layout->itemAt(0)->widget();
		layout->removeWidget(w);
//		w->handleClose();
		switch(w->type) {
			case 1:
				QObject::disconnect(w,SIGNAL(groupListClosed(QString,QList<QString>)),this,SIGNAL(groupListClosed(QString,QList<QString>)));
				break;
			case 3:
				QObject::disconnect(w,SIGNAL(searchClosed(QString,QString)),this,SIGNAL(searchClosed(QString,QString)));
				break;
		}
		QObject::disconnect(w,SIGNAL(tweetsAdded(bool)),this,SLOT(handleTweetsAdded(bool)));
		QObject::disconnect(w,SIGNAL(tweetsPending()),this,SLOT(handleTweetsPending()));
		w->deleteLater();
	}
}

void TweetListTabPanel::addListWidget(TweetListWidget *w) {
	layout->insertWidget(0,w);
	w->setLayout(layout);
	switch(w->type) {
		case 1:
			QObject::connect(w,SIGNAL(groupListClosed(QString,QList<QString>)),this,SIGNAL(groupListClosed(QString,QList<QString>)));
			break;
		case 3:
			QObject::connect(w,SIGNAL(searchClosed(QString,QString)),this,SIGNAL(searchClosed(QString,QString)));
			break;
	}
	QObject::connect(w,SIGNAL(tweetsAdded(bool)),this,SLOT(handleTweetsAdded(bool)));
	QObject::connect(w,SIGNAL(popout(TweetListWidget*)),this,SIGNAL(popout(TweetListWidget*)));
	QObject::connect(w,SIGNAL(tweetsPending()),this,SLOT(handleTweetsPending()));
}

/*void TweetListTabPanel::readSettings(QSettings &settings) {
}*/

void TweetListTabPanel::writeSettings(QSettings &settings) {
	TabPanel::writeSettings(settings);
	//settings.setValue("type",type);// 0 - list o tweets
	settings.beginWriteArray("columns");
	//QMessageBox::information (0,"assdf",QString::number(type));
	for(int x=0;x<layout->count();x++) {
		settings.setArrayIndex(x);
		((TweetListWidget*)layout->itemAt(x)->widget())->writeSettings(settings);
	}
	settings.endArray();
}

void TweetListTabPanel::setActive(bool a) {
	if (a)
		QTimer::singleShot(200,this,SLOT(handleSetActive()));
	else
		QTimer::singleShot(200,this,SLOT(handleSetInactive()));
}

void TweetListTabPanel::handleSetActive() {
	for(int x=0;x<layout->count();x++) {
		((TweetListWidget*)layout->itemAt(x)->widget())->setActive(true);
	}
}

void TweetListTabPanel::handleSetInactive() {
	for(int x=0;x<layout->count();x++) {
		((TweetListWidget*)layout->itemAt(x)->widget())->setActive(false);
	}
}

void TweetListTabPanel::handleTweetsAdded(bool muted) {
	if (!muted)
		emit tweetsAdded();
	emit activity();
}

void TweetListTabPanel::handleTweetsPending() {
	if (drainTimer.timerId()==-1)
		drainTimer.start();
}

void TweetListTabPanel::refresh() {
	for(int x=0;x<layout->count();x++) {
		((TweetListWidget*)layout->itemAt(x)->widget())->refresh();
	}
}

void TweetListTabPanel::handleDrainTimer() {
/*

	for(int x=0;x<layout->count();x++) {
		((TweetListWidget*)layout->itemAt(x)->widget())->drainTweets();
	}

*/
	int count=layout->count();
	int done=0;
	int x;
	while (done<count) {
		x=(drainTick++)%count;
//	QMessageBox::information (0,"assdf",QString::number(x));
		TweetListWidget *t = (TweetListWidget*)layout->itemAt(x)->widget();
		done++;
		if (t->hasPendingTweets()) {
			done--;
			t->drainTweets();
			break;
		} else
			t->drainTweets();
	}
//	if (done==count)
//		drainTimer.stop();
}

int TweetListTabPanel::count() {
	return layout->count();
}

TweetListWidget* TweetListTabPanel::widgetAt(int x) {
	return (TweetListWidget*)layout->itemAt(x)->widget();
}
