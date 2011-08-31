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
 * DirectMessageListWidget.h
 *
 *  Created on: 25-Mar-2009
 *      Author: ian
 */

#ifndef DIRECTMESSAGELISTWIDGET_H_
#define DIRECTMESSAGELISTWIDGET_H_

#include "tweetlistwidget.h"

class DirectMessageListWidget: public TweetListWidget {
	Q_OBJECT
public:
	DirectMessageListWidget();
	DirectMessageListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~DirectMessageListWidget();
public slots:
	virtual void handleRefreshButton();
protected:
	virtual bool canDoUpdate(Tweet *);
	virtual void repopulate();
private:
	QString myUser;
};

#endif /* DIRECTMESSAGELISTWIDGET_H_ */
