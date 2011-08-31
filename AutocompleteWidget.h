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
/* $Id: AutocompleteWidget.h,v 1.4 2010/10/05 18:08:51 ian Exp $

 File       : AutoCompleteWidget.h
 Create date: 16:53:41 25 Apr 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef AUTOCOMPLETEWIDGET_H_
#define AUTOCOMPLETEWIDGET_H_

// Qt
#include <QWidget>
#include <QListWidget>
// Local
#include "backend/BirdBox.h"

class AutocompleteWidget : public QWidget {
	Q_OBJECT
public:
	AutocompleteWidget(BirdBox *b,QWidget *text=0,QWidget *parent=0);
	virtual ~AutocompleteWidget();

	void setMatchText(QString);
	QString getCurrentEntry();

	int type;
public slots:
	virtual void setVisible(bool);
protected:
	QString search;
	BirdBox *tweets;
	QListWidget *list;

	bool eventFilter(QObject *, QEvent *);
	virtual void populateList() {};
signals:
	void exactMatch();
	void noMatch();
};

#endif /* AUTOCOMPLETEWIDGET_H_ */
