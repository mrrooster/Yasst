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
 * SearchListWidget.cpp
 *
 *  Created on: 11-Mar-2009
 *      Author: ian
 */

#include "SearchListWidget.h"

SearchListWidget::SearchListWidget(QWidget *parent) : TweetListWidget(parent) {
	type=3;
//	if (maxCreateLimit>10)
//		maxCreateLimit=10;
//	maxCreateLimit=3;
}

SearchListWidget::SearchListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	this->setSearchVisible();
	QObject::connect(ui.topCloseButton,SIGNAL(clicked()),this,SLOT(saveSearch()));
	QObject::connect(ui.title,SIGNAL(doubleclicked()),this,SLOT(setSearchVisible()));
	QObject::connect(&searcher,SIGNAL(haveResults(QList<QString>)),this,SLOT(searchUpdate(QList<QString>)));
	QObject::connect(&searcher,SIGNAL(endFetching()),this,SLOT(handleEndFetching()));
	QObject::connect(ui.closeButton,SIGNAL(clicked()),this,SLOT(close()));
	b->addFetcher(&searcher);
	ui.title->setToolTip("Double click to change the search.");
	ui.title->setObjectName("search");
	setTitle("Search");
	ui.type->setPixmap(QPixmap(":/buttons/search.png"));
//	defaultTitleStyle="color: green;";
	type=3;
/*
	if (maxCreateLimit>5)
		maxCreateLimit=5;
*/
}

SearchListWidget::~SearchListWidget() {
	tweets->removeFetcher(&searcher);
}

void SearchListWidget::searchUpdate(QList<QString> tweetIds) {
	QMutexLocker tweetsLocker(&tweetsLock);
	int count=0;

	startDrainTimer();
	while(tweetIds.size()) {
		Tweet *t = tweets->getTweet(tweetIds.takeFirst());
		if (t&&TweetListWidget::canDoUpdate(t)) {
			TweetUser *u=tweets->getUserByScreenName(t->screen_name);
			t->refCount++;
			if (u)
				images->getPixMap(u->image_url);
			pendingTweets.append(t);
			count++;
		}
	}
//	drainTweets();
	canClearFirstRefresh=true;
	if (!firstRefresh && count) {
		emit tweetsAdded(muted);
	}
}

void SearchListWidget::setSearchVisible() {
	showTopText();
	//setStack(2);
}

void SearchListWidget::tweetUpdates(QList<Tweet*> &) {
}

void SearchListWidget::close() {
	emit searchClosed(ui.title->text(),ui.topText->text());
//	tweets->removeFetcher(&searcher);
}

void SearchListWidget::saveSearch() {
	if (ui.topText->text()!=searcher.getSearch()) {
		setupList();
		removeAllTweets(true);
		searcher.populate();
	}
	hideTopText();
}

void SearchListWidget::setupList() {
/*
	if (ui.searchName->text().size()==0)
		ui.title->setText(ui.query->text());
	else
*/
		setTitle(ui.topText->text());

	//ui.title->setStyleSheet(defaultTitleStyle);

	setTweetsVisible();

	searcher.setSearch(ui.topText->text());
}

void SearchListWidget::readSettings(QSettings &settings) {
	TweetListWidget::readSettings(settings);
	setSearch(settings.value("search","").toString(),settings.value("title","").toString());
}

void SearchListWidget::writeSettings(QSettings &settings) {
	TweetListWidget::writeSettings(settings);
	settings.setValue("type",3);
	settings.setValue("search",ui.topText->text());
	settings.setValue("title",ui.title->text());
}

void SearchListWidget::setSearch(QString search,QString ) {
	ui.topText->setText(search);
/*
	if (title!=ui.query->text()) {
		ui.searchName->setText(title);
	}
*/

	saveSearch();
}

void SearchListWidget::handleEndFetching() {
	canClearFirstRefresh=true;
}

void SearchListWidget::handleRefreshButton() {
	searcher.populate();
}

