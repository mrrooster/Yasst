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
/* $Id: AbstractTopWidget.cpp,v 1.1 2009/09/01 00:06:30 ian Exp $

 File       : AbstractTopWidget.cpp
 Create date: 21:16:58 28-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "AbstractTopWidget.h"
#include <QDateTime>

AbstractTopWidget::AbstractTopWidget(QWidget *parent) :AbstractTweetWidget(parent) {
	tweet.created_at=QDateTime::currentDateTime().addYears(1000);
}

AbstractTopWidget::~AbstractTopWidget() {
}
