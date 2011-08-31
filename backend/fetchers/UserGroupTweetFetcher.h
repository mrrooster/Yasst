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
/* $Id: UserGroupTweetFetcher.h,v 1.1 2010/03/17 18:48:14 ian Exp $

 File       : UserGroupTweetFetcher.h
 Create date: 00:27:50 7 Mar 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef USERGROUPTWEETFETCHER_H_
#define USERGROUPTWEETFETCHER_H_

#include "UserTweetFetcher.h"

class Group;

class UserGroupTweetFetcher : public UserTweetFetcher {
	Q_OBJECT
public:
	UserGroupTweetFetcher();
	virtual ~UserGroupTweetFetcher();

	void populate();
	void setGroup(Group *list);
	Group* getGroup();
protected:
	Group *list;
};

#endif /* USERGROUPTWEETFETCHER_H_ */
