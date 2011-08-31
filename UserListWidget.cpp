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
/* $Id: UserListWidget.cpp,v 1.7 2010/09/14 13:49:05 ian Exp $

 File       : UserListWidget.cpp
 Create date: 21:47:51 20-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "UserListWidget.h"
#include "UserWidget.h"

UserListWidget::UserListWidget(QWidget *parent) : TweetListWidget(parent) {
	type=7;
	user=0;
	userWidget=0;
	fetcher=0;
}

UserListWidget::UserListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	type=7;
	user=0;
	userWidget=0;
	fetcher=0;
	t.created_at=QDateTime::currentDateTime().addYears(1000);
	ui.title->setText(tr("User info"));
	ui.topText->text()="";
	QObject::connect(tweets,SIGNAL(haveUpdatedUser(TweetUser*)),this,SLOT(handleTweetUser(TweetUser*)));
	QObject::connect(tweets,SIGNAL(haveUser(TweetUser*)),this,SLOT(handleTweetUser(TweetUser*)));
	QObject::connect(ui.topCloseButton,SIGNAL(clicked()),this,SLOT(handleSearch()));
	QObject::connect(ui.title,SIGNAL(doubleclicked()),this,SLOT(showTopText()));
	ui.type->setPixmap(QPixmap(":/buttons/user_search.png"));
	showTopText();
}

UserListWidget::~UserListWidget() {
	if (fetcher)
		tweets->removeFetcher(fetcher);
}

void UserListWidget::handleSearch() {
	setUser(ui.topText->text());
}

void UserListWidget::setUser(QString screen_name) {
	TweetUser *u = tweets->getUserByScreenName(screen_name);

	hideTopText();
	ui.topText->setText(screen_name);
	if (u)
		setUser(u);
	else {
		TwitterUserTweetsFetcher *fetcher = new TwitterUserTweetsFetcher();
		fetcher->setFetchScreenName(ui.topText->text());
		fetcher->periodic=false;
		tweets->addFetcher(fetcher);
		fetcher->populate();
	}
}

void UserListWidget::setUser(TweetUser *u) {
	QSettings opts;
	QMutexLocker locker(&knownTweetsLock);
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();


	hideTopText();
	user=u;
	ui.topText->setText(user->screen_name);
	if (userWidget) {
		QBoxLayout *layout2 = (QBoxLayout*)ui.scrollArea->widget()->layout();
		layout2->removeWidget(userWidget);
		userWidget->deleteLater();
	}

	if (fetcher)
		tweets->removeFetcher(fetcher);

	fetcher = new TwitterUserTweetsFetcher();
	fetcher->setFetchScreenName(user->screen_name);
	fetcher->periodic=true;
	tweets->addFetcher(fetcher);
	fetcher->weight = opts.value("singleUserFetcherWeight",5).toInt();
	fetcher->populate();

	userWidget = new UserWidget(&t,user,tweets,images);
	userWidget->setProperty("userwidget", "true");
	repopulate();
	layout->insertWidget(0,userWidget);
	knownTweets.insert(&t,userWidget);
	ui.title->setText(user->screen_name);
	refresh();
}

bool UserListWidget::canDoUpdate(Tweet *t) {
	if (!user)
		return false;
	return(user->screen_name.compare(t->screen_name,Qt::CaseInsensitive)==0);
}

void UserListWidget::readSettings(QSettings &settings) {
	TweetListWidget::readSettings(settings);
	setUser(settings.value("screen_name","").toString());
}

void UserListWidget::writeSettings(QSettings &settings) {
	TweetListWidget::writeSettings(settings);
	settings.setValue("screen_name",ui.topText->text());
}

void UserListWidget::handleTweetUser(TweetUser* u) {
	if (!user) {
		if (u->screen_name.compare(ui.topText->text(),Qt::CaseInsensitive)==0)
			setUser(u);
	}
}
