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
/* $Id: JSONObject.h,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : JSONObject.h
 Create date: 22:35:48 15 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef JSONOBJECT_H_
#define JSONOBJECT_H_

#include <QString>
#include <QVariant>

class JSONObject {
public:
	enum Type {
		JSONint,JSONfloat,JSONString,JSONComplexObject,JSONArray
	};

	JSONObject();
	virtual ~JSONObject();
	virtual QString toString() {return "[JSONObject]";}

	JSONObject::Type type;
};
Q_DECLARE_METATYPE(JSONObject)
#endif /* JSONOBJECT_H_ */
