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
 * EditableLabel.cpp
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#include "EditableLabel.h"
#include "FilterLineEdit.h"
#include <QStackedLayout>
#include <QMessageBox>
#include <QRect>

EditableLabel::EditableLabel(QWidget *parent) : QWidget(parent) {
	doInit();
}
EditableLabel::EditableLabel(QString labelText,QWidget *parent) : QWidget(parent) {
	doInit();
	text->setText(labelText);
	edit->setText(labelText);
}

void EditableLabel::doInit() {
	QStackedLayout *layout=new QStackedLayout();

	layout->setSpacing(0);
	layout->setMargin(0);
	text=new QLabel(this);
	edit=new FilterLineEdit(this);
//	text->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
//	edit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
//	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
//	edit->setMinimumWidth(10);
//	text->setMinimumWidth(10);
//	setMinimumWidth(10);
	edit->setMaximumWidth(90);
	layout->addWidget(text);
	layout->addWidget(edit);
	layout->setCurrentIndex(0);
	setLayout(layout);
	QObject::connect(edit,SIGNAL(editingFinished()),this,SLOT(setDisplayMode()));
	QObject::connect(edit,SIGNAL(lostFocus()),this,SLOT(setDisplayMode()));
//	QObject::connect(text,SIGNAL(doubleclicked()),this,SLOT(setEditMode()));
	editing=false;
}

EditableLabel::~EditableLabel() {
	text->deleteLater();
	edit->deleteLater();
}

void EditableLabel::setEditMode() {
//	QMessageBox::information (0,"assdf","j2");
	((QStackedLayout *)layout())->setCurrentIndex(1);
	edit->setFocus(Qt::OtherFocusReason);
	editing=true;
	this->setMinimumWidth(edit->sizeHint().width()-40);
	emit contentsEditing(this);
}

void EditableLabel::setDisplayMode() {
	((QStackedLayout *)layout())->setCurrentIndex(0);
	editing=false;
	this->setMinimumWidth(0);
	emit contentsChanged(this);
}

QString EditableLabel::getDisplayText() {
	return text->text();
}

QString EditableLabel::getEditText() {
	return edit->text();
}

void EditableLabel::setDisplayText(QString s) {
	text->setText(s);
}

void EditableLabel::setEditText(QString s) {
	edit->setText(s);
}

void EditableLabel::mouseDoubleClickEvent ( QMouseEvent *  ) {
	setEditMode();
}

QSize EditableLabel::sizeHint () const {
	//size.setWidth(text->fontMetrics().width(text->text()));
	QSize hint = edit->sizeHint();
	QSize thint = text->sizeHint();
	if (!editing)
		hint.setWidth(thint.width());
	else
		hint.setWidth(hint.width()-40);
	return hint;
}
