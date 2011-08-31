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
/* $Id: FriendsListWidget.cpp,v 1.8 2010/07/25 14:51:22 ian Exp $
File:   FriendsListWidget.h
(c) 2009 Ian Clark

*/


#include "FriendsListWidget.h"
#include <QSettings>

FriendsListWidget::FriendsListWidget() {
	type = 4;
}

FriendsListWidget::FriendsListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	ui.title->setText(b->getAuthenticatingUserScreenName()+tr("'s Friends"));

	type=4;
	QObject::connect(&repopulateTimer,SIGNAL(timeout()),this,SLOT(handleTimer()));
	QObject::connect(b,SIGNAL(haveUser(TweetUser*)),this,SLOT(handleHaveUser(TweetUser*)));
	repopulateTimer.setSingleShot(true);
	repopulateTimer.setInterval(500);
	repopulate();
}

FriendsListWidget::~FriendsListWidget() {
}

bool FriendsListWidget::canDoUpdate(Tweet *t) {
	TweetUser *u=tweets->getUserByScreenName(t->screen_name);
	if (u)
		return u->is_friend&&TweetListWidget::canDoUpdate(t);
	else
		return false;
}

void FriendsListWidget::writeSettings(QSettings &settings) {
	TweetListWidget::writeSettings(settings);
	settings.setValue("type",4);
}

void FriendsListWidget::handleTimer() {
//	repopulateTimer.stop();
	refresh();
}

void FriendsListWidget::handleHaveUser(TweetUser *u) {
	if (u->is_friend) {
		repopulateTimer.start();
	}
}

void FriendsListWidget::handleRefreshButton() {
	TweetFetcher *f = tweets->getFetcher(1);
	if (f)
		f->populate();
}
