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
 * FriendsListWidget.h
 *
 *  Created on: 16-Mar-2009
 *      Author: ian
 */

#ifndef FRIENDSLISTWIDGET_H_
#define FRIENDSLISTWIDGET_H_

#include "tweetlistwidget.h"

class FriendsListWidget: public TweetListWidget {
	Q_OBJECT
public:
	FriendsListWidget();
	FriendsListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~FriendsListWidget();
public slots:
	virtual void writeSettings(QSettings &);
	virtual void handleRefreshButton();
protected:
	virtual bool canDoUpdate(Tweet *);
private:
	QTimer repopulateTimer;
private slots:
	void handleTimer();
	void handleHaveUser(TweetUser*);

};

#endif /* FRIENDSLISTWIDGET_H_ */
