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
/* $Id: YasstApplication.cpp,v 1.2 2010/03/04 23:48:16 ian Exp $

 File       : YasstApplication.cpp
 Create date: 14:26:49 17 Jan 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#include "YasstApplication.h"

YasstApplication::YasstApplication( int & argc, char ** argv ) : QApplication(argc,argv) {
	startTime=QDateTime::currentDateTime();
}

YasstApplication::~YasstApplication() {
}

void YasstApplication::addAccount(BirdBox *b) {
	if (!accounts.contains(b)) {
		accounts.append(b);
		emit accountAdded(b);
		emit accountsChanged(accounts.count());
	}
}

void YasstApplication::removeAccount(BirdBox *b) {
	if (accounts.contains(b)) {
		accounts.removeOne(b);
		emit accountRemoved(b);
		emit accountsChanged(accounts.count());
	}
}

QList<BirdBox*> YasstApplication::getAccounts() {
	return accounts;
}

int YasstApplication::getAccountsCount() {
	return accounts.count();
}
