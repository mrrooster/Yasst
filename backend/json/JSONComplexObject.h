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
/* $Id: JSONComplexObject.h,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : JSONComplexObject.h
 Create date: 16:42:01 19 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef JSONCOMPLEXOBJECT_H_
#define JSONCOMPLEXOBJECT_H_

#include "JSONObject.h"
#include <QHash>

class JSONComplexObject: public JSONObject {
public:
	JSONComplexObject();
	virtual ~JSONComplexObject();
	virtual QString toString();

	void addValue(QString,JSONObject*);

	QHash<QString,JSONObject*> values;
};

#endif /* JSONCOMPLEXOBJECT_H_ */
