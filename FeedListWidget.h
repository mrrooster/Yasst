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
/* $Id: FeedListWidget.h,v 1.2 2010/09/14 13:49:05 ian Exp $

 File       : FeedListWidget.h
 Create date: 22:00:06 4 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef FEEDLISTWIDGET_H_
#define FEEDLISTWIDGET_H_

#include "tweetlistwidget.h"
#include "backend/fetchers/FeedFetcher.h"

class FeedListWidget: public TweetListWidget {
	Q_OBJECT
public:
	FeedListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~FeedListWidget();

	void requestURL();
public slots:
	void writeSettings(QSettings &);
	void readSettings(QSettings &);
protected:
	virtual bool canDoUpdate(Tweet *);
private slots:
	void showFeed();
	void handleFeedTitle(QString);
private:
	FeedFetcher feed;
};

#endif /* FEEDLISTWIDGET_H_ */
