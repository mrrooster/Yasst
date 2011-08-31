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
/* $Id: FavoriteListWidget.cpp,v 1.3 2010/07/25 14:51:22 ian Exp $

 File       : FavoriteListWidget.cpp
 Create date: 01:14:03 26-Jul-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "FavoriteListWidget.h"
#include <QSettings>

FavoriteListWidget::FavoriteListWidget(BirdBox *b,ImageCache *i,QString user, QWidget *parent) : TweetListWidget(b,i,parent) {
	ui.title->setText(tr("Favourites"));
	QSettings settings;
	if (user!="")
		myUser = user;
	else
		myUser = settings.value("accounts/twitter/user").toString();

	type=6;
	repopulate();
	ui.type->setPixmap(QPixmap(":/buttons/favorite.png"));
}

FavoriteListWidget::~FavoriteListWidget() {
}

bool FavoriteListWidget::canDoUpdate(Tweet *t) {
	return t->favorite&&TweetListWidget::canDoUpdate(t);
}

void FavoriteListWidget::handleRefreshButton() {
	TweetFetcher *f = tweets->getFetcher(4);
	if (f)
		f->populate();
}

