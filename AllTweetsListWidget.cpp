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
/* $Id: AllTweetsListWidget.cpp,v 1.1 2009/05/06 13:08:29 ian Exp $

 File       : AllTweetsListWidget.cpp
 Create date: 00:16:18 06-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "AllTweetsListWidget.h"

AllTweetsListWidget::AllTweetsListWidget(QWidget *parent) : TweetListWidget(parent) {
}

AllTweetsListWidget::AllTweetsListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent) {
	repopulate();
}

AllTweetsListWidget::~AllTweetsListWidget() {
}
