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
/* $Id: UserReplyFetcher.h,v 1.2 2010/06/19 20:29:41 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef USERREPLYFETCHER_H_
#define USERREPLYFETCHER_H_

#include "UserTweetFetcher.h"

class UserReplyFetcher: public UserTweetFetcher {
	Q_OBJECT
public:
	UserReplyFetcher();
	virtual ~UserReplyFetcher();
	void populate();
protected:
	//virtual int doHttpBit(QString URL,QString user="",QString pass="", QString data="", QString = "");
};

#endif /* USERREPLYFETCHER_H_ */
