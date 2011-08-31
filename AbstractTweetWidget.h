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
/* $Id: AbstractTweetWidget.h,v 1.6 2009/09/26 18:54:09 ian Exp $

 File       : AbstractTweetWidget.h
 Create date: 19:29:51 16-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef ABSTRACTTWEETWIDGET_H_
#define ABSTRACTTWEETWIDGET_H_
#include <QWidget>
#include "backend/Tweet.h"

class AbstractTweetWidget : public QWidget {
    Q_OBJECT
public:
	AbstractTweetWidget(QWidget *parent = 0);
	virtual ~AbstractTweetWidget();
	int type;
	virtual void setCompactMode(bool) {};
public slots:
	virtual Tweet* getTweet() {return 0;};
	virtual void refresh() {};
	virtual void setInReplyToMode() {};
	virtual void setNew() {};
	virtual void clearNew(bool=false) {};
	virtual bool isNew() {return false;};


};

#endif /* ABSTRACTTWEETWIDGET_H_ */
