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
/* $Id: Tweet.cpp,v 1.4 2010/03/04 23:48:16 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "Tweet.h"
#include <QRegExp>

Tweet::Tweet() {
	direct_message=false;
	refCount=0;
	stamp();
	//created_at=QDateTime::currentDateTime();
	deleted=false;
	favorite=false;
	retweet=false;
	read=false;
	type=0;
}

Tweet::~Tweet() {
}

void Tweet::stamp() {
	timestamp=QDateTime::currentDateTime();
}

QString Tweet::getTextSource() {
	long otherPos=0;
	QRegExp deLt("<[^>]*>");
	QString text=source;
	while ((otherPos=deLt.indexIn(text,otherPos))>=0)
		text=text.left(otherPos)+text.mid(otherPos+deLt.matchedLength(),-1);
	return text;
}
