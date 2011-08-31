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
/* $Id: AutocompleteHashlistWidget.h,v 1.1 2010/04/25 23:20:27 ian Exp $

 File       : AutocompleteHashlistWidget.h
 Create date: 22:55:11 25 Apr 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef AUTOCOMPLETEHASHLISTWIDGET_H_
#define AUTOCOMPLETEHASHLISTWIDGET_H_

#include "AutocompleteWidget.h"

class AutocompleteHashlistWidget: public AutocompleteWidget {
	Q_OBJECT
public:
	AutocompleteHashlistWidget(BirdBox *b,QWidget *text=0,QWidget *parent=0);
	virtual ~AutocompleteHashlistWidget();

protected:
	void populateList();
};

#endif /* AUTOCOMPLETEHASHLISTWIDGET_H_ */
