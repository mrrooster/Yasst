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
/* $Id: JSONParser.cpp,v 1.7 2010/08/01 18:15:36 ian Exp $

 File       : JSONParser.cpp
 Create date: 22:32:47 15 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "JSONParser.h"
#include "JSONObject.h"
#include "JSONString.h"
#include "JSONComplexObject.h"
#include "JSONArray.h"
#include <QMessageBox>
#include <QRegExp>

JSONParser::JSONParser() {
}

JSONParser::~JSONParser() {
}

JSONComplexObject* JSONParser::parse(QString input) {
	int x=0;
	return parse(input,x);
}

JSONComplexObject* JSONParser::parse(QString input,int &pos) {
	QRegExp deSlash("\\\\[^\\\\]");
	int state=0; //0-start,1-value,2-array
	QString name;
	JSONComplexObject *me=0;
	JSONArray *array=0;
	JSONString *value;

	for(int x=pos;x<input.length();x++) {
		char c = input.at(x).toAscii();
		int start;
		QString string;
		switch(c) {
		case '{' :
			switch(state){
			case 0:
				me = new JSONComplexObject();
				break;
			case 1:
				me->addValue(name,parse(input,x));
				state=0;
				break;
			case 2:
//				QMessageBox::information(0,"","b:"+input.mid(x,1)+QString::number(x));
				array->items.append(parse(input,x));
//				QMessageBox::information(0,"","a:"+input.mid(x,1)+QString::number(x));
				break;
			}
			break;
		case '"' :
			start=++x;
			for(;x<input.length()&&input.at(x)!='"';x++)
				if (input.at(x)=='\\')
					x++;
			string = input.mid(start,(x-start));
//			x++;
//			while ((otherPos=deSlash.indexIn(string,0))>=0)
//				string=string.left(otherPos)+string.mid(otherPos+1,-1);
//			if (x<100)
//				QMessageBox::information(0,"",string);
			switch(state) {
			case 0:
				name=string;
				state=1;
				break;
			case 1:
				value=new JSONString(string);
				me->addValue(name,value);
				state=0;
				break;
			case 2:
				value=new JSONString(string);
				array->items.append(value);
				break;
			}
			break;
		case '-' :
		case '0' :
		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :
		case '8' :
		case '9' :
			start=x;
			char c;
			for(;x<input.length()&&((c=input.at(x).toAscii())=='.'||(c<='9'&&c>='0'));x++) ;
			string = input.mid(start,(x-start));
			switch(state) {
			case 0:
				name=string;
				state=1;
//				for(;x<input.length()&&input.at(x).toAscii()!=':';x++);
//				x++;
				break;
			case 1:
				value=new JSONString(string);
				me->addValue(name,value);
				state=0;
				break;
			case 2:
				value=new JSONString(string);
				array->items.append(value);
				break;
			}

			break;
		case 'n':
			x+=3;//null
			if (state==1)
				state=0;
			break;
		case '[' :
			if (state==1) {
				array=new JSONArray();
				state=2;
			}
			break;
		case ']' :
			if (state==2) {
				me->addValue(name,array);
				state=0;
			}
			break;
		case '}' :
			pos=x;
			return me;
			break;
		default:
			if (state==0&&x==0) {
				if (me)
					delete me;
				return 0;
			}
		}
	}
	return me;
}
