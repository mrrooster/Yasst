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
/* $Id: JSONComplexObject.cpp,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : JSONComplexObject.cpp
 Create date: 16:42:01 19 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "JSONComplexObject.h"
#include <QHashIterator>

JSONComplexObject::JSONComplexObject() {
	type=JSONObject::JSONComplexObject;
}

JSONComplexObject::~JSONComplexObject() {
	QHashIterator <QString,JSONObject*> i(values);
	while (i.hasNext()) {
		i.next();
		delete i.value();
	}

}

void JSONComplexObject::addValue(QString name,JSONObject *value) {
	values.insert(name,value);
}

QString JSONComplexObject::toString() {
	QHashIterator <QString,JSONObject*> i(values);
	QString out = "{";
	while (i.hasNext()) {
		i.next();
		out += "\""+i.key()+"\"=";
		out += i.value()->toString();
		if (i.hasNext())
			out += ",";
	}
	out += "}";
	return out;
}
