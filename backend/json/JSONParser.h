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
/* $Id: JSONParser.h,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : JSONParser.h
 Create date: 22:32:47 15 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef JSONPARSER_H_
#define JSONPARSER_H_
#include "JSONObject.h"
#include "JSONString.h"
#include "JSONComplexObject.h"
#include "JSONArray.h"
#include <QString>

class JSONParser {
public:
	JSONParser();
	virtual ~JSONParser();

	static JSONComplexObject* parse(QString);
protected:
	static JSONComplexObject* parse(QString,int&);
};

#endif /* JSONPARSER_H_ */
