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
/* $Id: TweetListWidgetSpacer.h,v 1.1 2010/03/21 18:08:05 ian Exp $

 File       : TweetListWidgetSpacer.h
 Create date: 20:18:11 20 Mar 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef TWEETLISTWIDGETSPACER_H_
#define TWEETLISTWIDGETSPACER_H_

#include "tweetlistwidget.h"

class TweetListWidgetSpacer: public TweetListWidget {
    Q_OBJECT
public:
	TweetListWidgetSpacer(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~TweetListWidgetSpacer();
};

#endif /* TWEETLISTWIDGETSPACER_H_ */
