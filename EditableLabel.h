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
 * EditableLabel.h
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#ifndef EDITABLELABEL_H_
#define EDITABLELABEL_H_
#include <QLineEdit>
#include "ClickableLabel.h"

class EditableLabel : public QWidget {
	Q_OBJECT
public:
	EditableLabel(QWidget *parent = 0);
	EditableLabel(QString,QWidget *parent = 0);
	virtual ~EditableLabel();
	QString getDisplayText();
	QString getEditText();
	void setDisplayText(QString);
	void setEditText(QString);
	virtual QSize sizeHint () const;
private:
	QLabel *text;
	QLineEdit *edit;
	bool editing;
	void doInit();
	void mouseDoubleClickEvent ( QMouseEvent * e );
public slots:
	void setEditMode();
	void setDisplayMode();
signals:
	void contentsChanged(EditableLabel*);
	void contentsEditing(EditableLabel*);
};

#endif /* EDITABLELABEL_H_ */
