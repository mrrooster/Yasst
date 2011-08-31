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
 * SearchListWidget.h
 *
 *  Created on: 11-Mar-2009
 *      Author: ian
 */

#ifndef SEARCHLISTWIDGET_H_
#define SEARCHLISTWIDGET_H_

#include "tweetlistwidget.h"
#include "backend/fetchers/SearchFetcher.h"

class SearchListWidget: public TweetListWidget {
	Q_OBJECT
public:
	SearchListWidget(QWidget *parent = 0);
	SearchListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~SearchListWidget();
	void setSearch(QString,QString="");
	virtual void writeSettings(QSettings &);
	virtual void readSettings(QSettings &);
protected:
	SearchFetcher searcher;
	//virtual bool canDoUpdate(Tweet *);
protected slots:
	virtual void repopulate() {};
	virtual void setSearchVisible();
	virtual void close();
private:
	void setupList();
public slots:
	void saveSearch();
	void searchUpdate(QList<QString>);
	virtual void tweetUpdates(QList<Tweet*> &tweets);
	virtual void handleRefreshButton();
private slots:
	void handleEndFetching();
signals:
	void searchClosed(QString,QString);
};

#endif /* SEARCHLISTWIDGET_H_ */
