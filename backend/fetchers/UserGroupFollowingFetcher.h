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
/* $Id: UserGroupFollowingFetcher.h,v 1.1 2010/03/17 18:48:14 ian Exp $

 File       : UserGroupFollowingFetcher.h
 Create date: 22:56:21 10 Mar 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef USERGROUPFOLLOWINGFETCHER_H_
#define USERGROUPFOLLOWINGFETCHER_H_

#include "UserGroupFetcher.h"

class UserGroupFollowingFetcher: public UserGroupFetcher {
	Q_OBJECT
public:
	UserGroupFollowingFetcher();
	virtual ~UserGroupFollowingFetcher();

	virtual void populate();
};

#endif /* USERGROUPFOLLOWINGFETCHER_H_ */
