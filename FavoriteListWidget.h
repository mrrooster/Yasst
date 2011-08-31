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
/* $Id: FavoriteListWidget.h,v 1.3 2010/07/25 14:51:22 ian Exp $

 File       : FavoriteListWidget.h
 Create date: 01:14:03 26-Jul-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef FAVORITELISTWIDGET_H_
#define FAVORITELISTWIDGET_H_

#include "tweetlistwidget.h"

class FavoriteListWidget : public TweetListWidget {
	Q_OBJECT
public:
	FavoriteListWidget(BirdBox *b,ImageCache *i=0,QString user="", QWidget *parent = 0);
	virtual ~FavoriteListWidget();
public slots:
	virtual void handleRefreshButton();
protected:
	virtual bool canDoUpdate(Tweet *);
private:
	QString myUser;
};

#endif /* FAVORITELISTWIDGET_H_ */
