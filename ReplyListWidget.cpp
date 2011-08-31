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
/* $Id: ReplyListWidget.cpp,v 1.15 2010/07/25 14:51:22 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "ReplyListWidget.h"
#include <QSettings>

ReplyListWidget::ReplyListWidget() {
	type=2;
	ui.type->setPixmap(QPixmap(":/buttons/reply.png"));
}

ReplyListWidget::ReplyListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	ui.title->setText(tr("Mentions"));
	QSettings settings;

	type=2;
	ui.type->setPixmap(QPixmap(":/buttons/reply.png"));
	repopulate();
}

ReplyListWidget::~ReplyListWidget() {
}

bool ReplyListWidget::canDoUpdate(Tweet *t) {
	if (myUser=="")
		myUser = tweets->getAuthenticatingUserScreenName();
	return t->message.contains("@"+myUser,Qt::CaseInsensitive)&&TweetListWidget::canDoUpdate(t);
}

void ReplyListWidget::handleRefreshButton() {
	TweetFetcher *f = tweets->getFetcher(2);
	if (f)
		f->populate();
}

