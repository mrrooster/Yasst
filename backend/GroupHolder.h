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
/* $Id: GroupHolder.h,v 1.1 2009/12/21 02:50:50 ian Exp $

 File       : GroupHolder.h
 Create date: 18:13:09 20 Dec 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef GROUPHOLDER_H_
#define GROUPHOLDER_H_
#include "Group.h"
#include <QVariant>

class GroupHolder {
	/*
	 * Convenience holder for a group pointer, used in UI lists and the like.
	 */
public:
	GroupHolder();
	GroupHolder(const GroupHolder &h);
	GroupHolder(Group *g);
	virtual ~GroupHolder();

	Group *group;
};
Q_DECLARE_METATYPE(GroupHolder)
#endif /* GROUPHOLDER_H_ */
