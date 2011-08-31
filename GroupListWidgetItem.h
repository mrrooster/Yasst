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
/* $Id: GroupListWidgetItem.h,v 1.1 2009/05/03 21:08:47 ian Exp $

 File       : GroupListWidgetItem.h
 Create date: 22:16:34 02-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef GROUPLISTWIDGETITEM_H_
#define GROUPLISTWIDGETITEM_H_
#include <QListWidgetItem>

class GroupListWidgetItem : public QListWidgetItem {
public:
	GroupListWidgetItem();
	GroupListWidgetItem(const QString & text, QListWidget * parent = 0, int type = Type );
	virtual ~GroupListWidgetItem();

	bool operator< ( const QListWidgetItem & other ) const;
};

#endif /* GROUPLISTWIDGETITEM_H_ */
