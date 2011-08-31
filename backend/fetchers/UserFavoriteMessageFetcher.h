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
/* $Id: UserFavoriteMessageFetcher.h,v 1.1 2009/09/02 20:44:08 ian Exp $

 File       : UserFavoriteMessageFetcher.h
 Create date: 01:18:07 26-Jul-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef USERFAVOURITEMESSAGEFETCHER_H_
#define USERFAVOURITEMESSAGEFETCHER_H_

#include "UserTweetFetcher.h"

class UserFavoriteMessageFetcher: public UserTweetFetcher {
	Q_OBJECT
public:
	UserFavoriteMessageFetcher();
	virtual ~UserFavoriteMessageFetcher();
	void populate();
};
#endif /* USERFAVOURITEMESSAGEFETCHER_H_ */
