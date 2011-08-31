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
/* $Id: YasstApplication.h,v 1.2 2010/03/04 23:48:15 ian Exp $

 File       : YasstApplication.h
 Create date: 14:26:49 17 Jan 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef YASSTAPPLICATION_H_
#define YASSTAPPLICATION_H_

#include <QApplication>
#include "backend/BirdBox.h"

class YasstApplication : public QApplication {
	Q_OBJECT
public:
	YasstApplication( int & argc, char ** argv );
	virtual ~YasstApplication();

	void addAccount(BirdBox *);
	void removeAccount(BirdBox *);
	QList<BirdBox*> getAccounts();
	int getAccountsCount();
	QDateTime startTime;
private:
    QList<BirdBox*> accounts;
signals:
    void accountsChanged(int);
    void accountAdded(BirdBox*);
    void accountRemoved(BirdBox*);
};

#endif /* YASSTAPPLICATION_H_ */
