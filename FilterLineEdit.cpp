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
/*
 * FilterLineEdit.cpp
 *
 *  Created on: 13-Mar-2009
 *      Author: ian
 */

#include "FilterLineEdit.h"

FilterLineEdit::FilterLineEdit(QWidget *p) : QLineEdit(p) {
}

FilterLineEdit::~FilterLineEdit() {
}

void FilterLineEdit::focusInEvent ( QFocusEvent * event ) {
	emit gotFocus();
	QLineEdit::focusInEvent(event);
}

void FilterLineEdit::focusOutEvent ( QFocusEvent * event ) {
	emit lostFocus();
	QLineEdit::focusOutEvent(event);
}
