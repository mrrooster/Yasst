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
/* $Id: JSONString.cpp,v 1.2 2010/06/19 20:29:41 ian Exp $

 File       : JSONString.cpp
 Create date: 22:34:20 16 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "JSONString.h"
#include <QDebug>

JSONString::JSONString() {
	type=JSONObject::JSONString;
}

JSONString::JSONString(QString val) {
	type=JSONObject::JSONString;
	this->set(val);
}

JSONString::~JSONString() {
}

void JSONString::set(QString val) {
	contents="";
	int start=0,x=0;
	for(x=0;x<val.size();x++) {
		if (val.at(x)=='\\') {
//			qDebug()<<"Got ctrl, x="<<x<<" char="<<val.at(x+1);
//			qDebug()<<"Start: "<<start;
			if ((x-1)>0)
				contents+=val.mid(start,x-start);
//			qDebug()<<val.mid(start,x-start);
			if (val.at(++x)=='u') {
//				qDebug()<<"val.mid("<<x<<",4)="<<val.mid(x+1,4);
				unsigned int i = val.mid(x+1,4).toUInt(0,16);
//				qDebug()<<"u got, i = "<<i;
				contents+=QChar(i);
				x+=4;
			} else if (val.at(++x)=='n') {
				contents+='\n';
			}
			start=x+1;
		}
	}
	contents+=val.mid(start,x-start);
//	contents=val;
}

QString JSONString::get() {
	return contents;
}

QString JSONString::toString() {
	return "\""+contents+"\"";
}
