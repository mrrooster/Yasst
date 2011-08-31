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
/* $Id: ClickableLabel.cpp,v 1.6 2009/05/04 14:40:54 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/

#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget *p) : QLabel(p) {
}

ClickableLabel::~ClickableLabel() {
}

void ClickableLabel::mouseDoubleClickEvent ( QMouseEvent *  ) {
	emit doubleclicked();
}
