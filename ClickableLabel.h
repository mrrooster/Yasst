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
/* $Id: ClickableLabel.h,v 1.5 2009/03/14 03:14:51 ian Exp $
File:   ClickableLabel.h
(c) 2009 Ian Clark

*/

#ifndef CLICKABLELABEL_H_
#define CLICKABLELABEL_H_
#include <QLabel>

class ClickableLabel : public QLabel{
	Q_OBJECT
public:
	ClickableLabel(QWidget *p=0) ;
	virtual ~ClickableLabel();
	void mouseDoubleClickEvent ( QMouseEvent * e );
signals:
	void doubleclicked();
};

#endif /* CLICKABLELABEL_H_ */
