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
/* $Id: yasstFns.cpp,v 1.4 2010/06/23 21:21:41 ian Exp $

File       : yasstFns.cpp
Create date: 18:18:58 17-Apr-2009
Project    : yasst

(c) 2009 Ian Clark

*/
#include "yasstFns.h"

long yasst_rand() {
	return rand(); // Yes, I know..
}

QString getVersionString() {
//	return "20100621";
/*#ifdef Q_WS_WIN*/

	return QString::number(getVersion());
/*#endif*/
	return "";
}

int getVersion() {
	QString date = __DATE__;
	QString months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	int x;
	for(x=0;x<11;x++)
		if (months[x]==date.left(3))
			break;

	return (date.mid(7,4).toInt()*10000)+((x+1)*100)+date.mid(4,2).toInt();
}
