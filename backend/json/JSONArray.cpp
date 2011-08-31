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
/* $Id: JSONArray.cpp,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : JSONArray.cpp
 Create date: 23:25:29 19 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "JSONArray.h"

JSONArray::JSONArray() {
	type=JSONObject::JSONArray;
}

JSONArray::~JSONArray() {
	for(int x=0;x<items.length();x++)
		delete items.at(x);
}

QString JSONArray::toString() {
	QString out = "[";
	for(int x=0;x<items.length();x++) {
		out += items.at(x)->toString();
		if ((x+1)<items.length())
			out += ",";
	}
	out += "]";
	return out;
}
