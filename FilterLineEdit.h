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
 * FilterLineEdit.h
 *
 *  Created on: 13-Mar-2009
 *      Author: ian
 */

#ifndef FILTERLINEEDIT_H_
#define FILTERLINEEDIT_H_
#include <QLineEdit>

class FilterLineEdit : public QLineEdit {
	Q_OBJECT
public:
	FilterLineEdit(QWidget *p=0);
	virtual ~FilterLineEdit();
	void focusInEvent ( QFocusEvent * event );
	void focusOutEvent ( QFocusEvent * event );
signals:
	void gotFocus();
	void lostFocus();
};

#endif /* FILTERLINEEDIT_H_ */
