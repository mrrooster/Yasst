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
/* $Id: AutocompleteWidget.cpp,v 1.6 2010/10/05 18:08:51 ian Exp $

 File       : AutoCompleteWidget.cpp
 Create date: 16:53:41 25 Apr 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#include "AutocompleteWidget.h"
// Qt
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QVBoxLayout>
// Local includes
#include "backend/TweetUser.h"

AutocompleteWidget::AutocompleteWidget(BirdBox *b,QWidget *text,QWidget *parent) : QWidget(parent) {
	tweets = b;

	QVBoxLayout *l = new QVBoxLayout();

	setLayout(l);

	list = new QListWidget(this);
	list->setSortingEnabled(true);
	list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	l->setContentsMargins(0,0,0,0);
	l->addWidget(list);
	l->setMargin(1);


	search = "";

	list->addItem("");
	int h = 0;
	int w = 0;
	QListWidgetItem *i=0;
	if ((i=list->item(0))) {
		h = (list->visualItemRect(i).height())* 4;
		w = (list->visualItemRect(i).width());
	}
	if (h==0)
		h=50;
	setMinimumHeight(h);
	list->setMaximumHeight(h);
	list->setMinimumWidth(w);
	list->clear();

	if (text)
		text->installEventFilter(this);

	type=0;
}

AutocompleteWidget::~AutocompleteWidget() {
}

void AutocompleteWidget::setMatchText(QString t) {
	search = t;
	populateList();
	if (list->count()==1 && list->item(0)->text().compare(t,Qt::CaseInsensitive)==0)
		emit exactMatch();
	else if (list->count()==0)
		emit noMatch();
}

QString AutocompleteWidget::getCurrentEntry() {
	if (list->count()==0)
		return "";

	if (list->currentItem())
		return list->currentItem()->text();

	return list->item(0)->text();
}

bool AutocompleteWidget::eventFilter(QObject *,QEvent *event) {
	if (event->type()==QEvent::KeyPress && (
			((QKeyEvent*)event)->key()==Qt::Key_Up ||
			((QKeyEvent*)event)->key()==Qt::Key_Down
			)
		) {
		int r;
		if (((QKeyEvent*)event)->key()==Qt::Key_Up) {
			r = list->currentRow()-1;
		} else {
			r = list->currentRow()+1;
		}
		if (r>=list->count())
			r=list->count()-1;
		if (r<0)
			r=0;
		list->setCurrentRow(r);
		return true;
	}
	return false;
}

void AutocompleteWidget::setVisible(bool vis) {
	if (vis) {
		int w = 0;
		QListWidgetItem *i=0;
		if ((i=list->item(0))) {
			w = (list->visualItemRect(i).width());
		}
		list->setMinimumWidth(w);
	}
	QWidget::setVisible(vis);
}
