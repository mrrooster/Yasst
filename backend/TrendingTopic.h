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
/* $Id: TrendingTopic.h,v 1.1 2009/09/21 00:53:02 ian Exp $

 File       : TrendingTopic.h
 Create date: 03:36:43 20 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef TRENDINGTOPICS_H_
#define TRENDINGTOPICS_H_

#include <QString>

class TrendingTopic {
public:
	TrendingTopic();
	virtual ~TrendingTopic();

	QString name;
	QString search;
	int prevPos;
};

#endif /* TRENDINGTOPICS_H_ */
