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
/* $Id: FriendFetcher.h,v 1.5 2010/03/21 18:08:05 ian Exp $

 File       : FriendFetcher.h
 Create date: 00:53:52 1 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef FRIENDFETCHER_H_
#define FRIENDFETCHER_H_

#include "TweetFetcher.h"

class FriendFetcher: public TweetFetcher {
	Q_OBJECT
public:
	FriendFetcher();
	virtual ~FriendFetcher();
	void populate();
	int getState() {return state;};

	void setSingleUserInfo(QString);
protected:
	void handleXml(QDomDocument *doc,int type,int);
protected slots:
	void handleUsers(int);
private:
	int state; // 0 - logon, 1 - friend fetch
	int pageCount;
	QString infoScreenName;
};

#endif /* FRIENDFETCHER_H_ */
