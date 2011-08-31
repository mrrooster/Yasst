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
/* $Id: GroupListWidgetItem.cpp,v 1.1 2009/05/03 21:08:47 ian Exp $

 File       : GroupListWidgetItem.cpp
 Create date: 22:16:34 02-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "GroupListWidgetItem.h"

GroupListWidgetItem::GroupListWidgetItem() {
}

GroupListWidgetItem::GroupListWidgetItem(const QString & text, QListWidget * parent, int type) : QListWidgetItem(text,parent,type) {
}

GroupListWidgetItem::~GroupListWidgetItem() {
}

bool GroupListWidgetItem::operator< ( const QListWidgetItem & other ) const {
	return (text().toLower()<other.text().toLower());
}
