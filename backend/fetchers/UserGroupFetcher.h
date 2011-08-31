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
/* $Id: UserGroupFetcher.h,v 1.2 2010/03/17 18:48:14 ian Exp $

 File       : UserGroupFetcher.h
 Create date: 15:47:01 6 Dec 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef GROUPFETCHER_H_
#define GROUPFETCHER_H_

#include "UserTweetFetcher.h"
#include "../Group.h"

class UserGroupFetcher: public UserTweetFetcher {
	Q_OBJECT
public:
	UserGroupFetcher();
	virtual ~UserGroupFetcher();

	virtual void populate();
	void setGroup(Group*);
	Group *group() {return myGroup;};
	void promoteGroupToList(Group*);

protected:
	virtual void handleXml(QDomDocument *doc,int type, int req);
	void handleListXml(QDomNode list);

	Group *myGroup;
	QList<QString> members;
	QString cursor;

private:
	int state; // 0 - normal, 1 - create

signals:
	//void groupUpdate(Group*);
	void haveGroup(Group*);
};

#endif /* GROUPFETCHER_H_ */
