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
/* $Id: JSONString.h,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : JSONString.h
 Create date: 22:34:20 16 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef JSONSTRING_H_
#define JSONSTRING_H_

#include "JSONObject.h"
#include <QString>

class JSONString: public JSONObject {
public:
	JSONString();
	JSONString(QString);
	virtual ~JSONString();
	virtual QString toString();

	void set(QString);
	QString get();
private:
	QString contents;
};

#endif /* JSONSTRING_H_ */
