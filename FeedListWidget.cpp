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
/* $Id: FeedListWidget.cpp,v 1.2 2010/09/14 13:49:05 ian Exp $

 File       : FeedListWidget.cpp
 Create date: 22:00:06 4 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "FeedListWidget.h"
#include "backend/FeedItem.h"
#include "URLInputBox.h"

FeedListWidget::FeedListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	QObject::connect(ui.topCloseButton,SIGNAL(clicked()),this,SLOT(showFeed()));
	QObject::connect(ui.title,SIGNAL(doubleclicked()),this,SLOT(showTopText()));
	QObject::connect(&feed,SIGNAL(feedTitle(QString)),this,SLOT(handleFeedTitle(QString)));
//	QObject::connect(&feed,SIGNAL(haveResults(QList<QString>)),this,SLOT(searchUpdate(QList<QString>)));
//	QObject::connect(&searcher,SIGNAL(endFetching()),this,SLOT(handleEndFetching()));
//	QObject::connect(ui.closeButton,SIGNAL(clicked()),this,SLOT(close()));
	b->addFetcher(&feed);
	ui.title->setToolTip("Double click to change the feed.");
	ui.title->setObjectName("feed");
	ui.title->setText("Feed");
	ui.type->setPixmap(QPixmap(":/buttons/feed.png"));
	ui.topCloseButton->setIcon(QIcon(":/buttons/feed.png"));
	showTopText();
//	defaultTitleStyle="color: green;";
	type=8;
}

FeedListWidget::~FeedListWidget() {
	tweets->removeFetcher(&feed);
}

void FeedListWidget::requestURL() {

	QWidget *w = QApplication::activeWindow();
	if (w==0)
		w=this->parentWidget();
	URLInputBox *getURL = new URLInputBox(w);
	if (getURL->exec()==QDialog::Rejected)
		this->close();
	else {
		ui.topText->setText(getURL->getURL());
		showFeed();
	}
	getURL->deleteLater();
}

void FeedListWidget::showFeed() {
	if (feed.getUrl()!=ui.topText->text()) {
		removeAllTweets();
		feed.setUrl(ui.topText->text());
		feed.populate();
	}
	hideTopText();
}

void FeedListWidget::handleFeedTitle(QString title) {
	ui.title->setText(title);
}

bool FeedListWidget::canDoUpdate(Tweet *t) {
	QMutexLocker tweetsLocker(&tweetsLock);
	QMutexLocker locker(&knownTweetsLock);
	if (knownTweets.contains(t)
			/*||pendingTweets.contains(t)*/
			)
		return false;
	return (t->type==2 && ((FeedItem*)t)->sourceUrl==ui.topText->text());
}

void FeedListWidget::readSettings(QSettings &settings) {
	TweetListWidget::readSettings(settings);
	ui.topText->setText(settings.value("feed_url","").toString());
	showFeed();
}

void FeedListWidget::writeSettings(QSettings &settings) {
	TweetListWidget::writeSettings(settings);
	settings.setValue("feed_url",ui.topText->text());
}
