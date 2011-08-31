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
/* $Id: DirectMessageListWidget.cpp,v 1.7 2010/07/25 14:51:22 ian Exp $
File:   ImageCacheThread.h
(c) 2009 Ian Clark

*/
/*
 * DirectMessageListWidget.cpp
 *
 *  Created on: 25-Mar-2009
 *      Author: ian
 */

#include "DirectMessageListWidget.h"

DirectMessageListWidget::DirectMessageListWidget() {
	type=5;
}

DirectMessageListWidget::DirectMessageListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	ui.title->setText(tr("Direct Messages"));
	QSettings settings;

	QObject::disconnect(b,SIGNAL(haveTweets(QList<Tweet *>&)),this,SLOT(tweetUpdates(QList<Tweet *>&)));
	QObject::connect(b,SIGNAL(haveDirectMessages(QList<Tweet *>&)),this,SLOT(tweetUpdates(QList<Tweet *>&)));

	type=5;
	repopulate();
}

DirectMessageListWidget::~DirectMessageListWidget() {
}

bool DirectMessageListWidget::canDoUpdate(Tweet *t) {
	return t->direct_message&&TweetListWidget::canDoUpdate(t);
}

void DirectMessageListWidget::repopulate() {
	removeAllTweets();
	pendingTweets = tweets->getDirectMessages();
	QTimer::singleShot(0, this, SLOT(drainTweets()));
}

void DirectMessageListWidget::handleRefreshButton() {
	TweetFetcher *f = tweets->getFetcher(3);
	if (f)
		f->populate();
}

