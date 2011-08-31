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
 * TweetTabWidget.cpp
 *
 *  Created on: 24-Mar-2009
 *      Author: ian
 */

#include "TweetTabWidget.h"
#include <QTabBar>
#include <QMessageBox>
#include <QPoint>
#include <QMouseEvent>
#include "EditableLabel.h"

TweetTabWidget::TweetTabWidget(QWidget *p) : QTabWidget(p) {
	setMovable(true);
	setTabsClosable(false);
	activeList=0;
	QObject::connect(this,SIGNAL(currentChanged(int)),this,SLOT(handleCurrentChanged(int)));
	QObject::connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(handleTabCloseRequested(int)));
//	setDocumentMode(true);
	tabBar()->setObjectName(tr("tabbar"));
	tabBar()->installEventFilter(this);
	//setCornerWidget(new QLabel("hello"));
/*
	QPalette pal = palette();
	pal.setColor(QPalette::Window,Qt::darkCyan);
	pal.setColor(QPalette::Foreground,Qt::black);
	setPalette(pal);
*/
}

TweetTabWidget::~TweetTabWidget() {
	tabBar()->removeEventFilter(this);
}


int TweetTabWidget::addTab(QWidget *p,const QString l) {
	int r=QTabWidget::addTab(p,l);
	EditableLabel *e =new EditableLabel(l);

	tabBar()->setTabButton(r,QTabBar::LeftSide,e);
	QObject::connect(e,SIGNAL(contentsChanged(EditableLabel*)),this,SLOT(handleTabRename(EditableLabel *)));
	QObject::connect(e,SIGNAL(contentsEditing(EditableLabel*)),this,SLOT(handleTabEditing(EditableLabel *)));
	setTabText(r,l);
	handleTabCountChange();
	return r;
}

void TweetTabWidget::hideTabBar() {
	tabBar()->hide();
}

void TweetTabWidget::showTabBar() {
	tabBar()->show();
}

TweetListTabPanel* TweetTabWidget::addListView() {
	TweetListTabPanel *list = new TweetListTabPanel(this);
	setCurrentIndex(addTab(list,tr("New tab")));
	if (activeList)
		activeList->setActive(false);
	activeList=list;
	QObject::connect(list,SIGNAL(groupListClosed(QString,QList<QString>)),this,SIGNAL(groupListClosed(QString,QList<QString>)));
	QObject::connect(list,SIGNAL(searchClosed(QString,QString)),this,SIGNAL(searchClosed(QString,QString)));
	QObject::connect(list,SIGNAL(activity()),this,SLOT(handleTabActivity()));
	list->setActive(true);
	return list;
}

void TweetTabWidget::addListWidget(TweetListWidget *w) {
	if (!activeList)
		addListView();
	activeList->addListWidget(w);
}

void TweetTabWidget::writeSettings(QSettings &settings) {

	settings.remove("tabs");

	settings.beginWriteArray("tabs");
	for (int x=0;x<count();x++) {
		settings.setArrayIndex(x);
		settings.setValue("name",tabText(x));
		((TabPanel*)widget(x))->writeSettings(settings);
	}
	settings.endArray();
	settings.setValue("activeTab",currentIndex()); // 0 - list o tweets
}

void TweetTabWidget::handleCurrentChanged(int i) {
	TabPanel *p = (TabPanel *)widget(i);
	if (p->getType()==0) {
		if (activeList)
			activeList->setActive(false);
		activeList=(TweetListTabPanel*)p;
		activeList->setActive(true);
	}
//	QTabWidget::setTabText(i,"");
}

void TweetTabWidget::setActive() {
	if (activeList)
		activeList->setActive(true);
}

void TweetTabWidget::handleTabCloseRequested(int i) {
	TabPanel *w = (TabPanel*)widget(i);
	removeTab(i);
	if (w->getType()==0) {
		QObject::disconnect(w,SIGNAL(groupListClosed(QString,QList<QString>)),this,SIGNAL(groupListClosed(QString,QList<QString>)));
		QObject::disconnect(w,SIGNAL(searchClosed(QString,QString)),this,SIGNAL(searchClosed(QString,QString)));
		QObject::disconnect(w,SIGNAL(activity()),this,SLOT(handleTabActivity()));
		if (activeList==(TweetListTabPanel*)w)
			activeList=0;
	}
	w->deleteLater();
	handleTabCountChange();
}

void TweetTabWidget::closeActiveTab() {
	handleTabCloseRequested(currentIndex());
}

void TweetTabWidget::handleTabCountChange() {
	if (count()>1) {
		setTabsClosable(true);
		showTabBar();
		int c = count();
		for(int x=0;x<c;x++) {
			EditableLabel *l = (EditableLabel*)(tabBar()->tabButton(x,QTabBar::LeftSide));
			tabBar()->setTabButton(x,QTabBar::LeftSide,0);
			tabBar()->setTabButton(x,QTabBar::LeftSide,l);
		}
	} else {
		hideTabBar();
		setTabsClosable(false);
	}
}

void TweetTabWidget::handleTabRename(EditableLabel *l) {
	QTabBar *b=tabBar();
	for(int x=0;x<b->count();x++) {
		if (l==b->tabButton(x,QTabBar::LeftSide)) {
			setTabText(x,l->getEditText());
			break;
		}
	}
}

void TweetTabWidget::handleTabEditing(EditableLabel *l) {
	QTabBar *b=tabBar();
	for(int x=0;x<b->count();x++) {
		if (l==b->tabButton(x,QTabBar::LeftSide)) {
			QTabWidget::setTabText(x,"");
			break;
		}
	}
}

void TweetTabWidget::setTabText(int index, const QString & label ) {
	QString toolTip="Double click to rename";
	QTabWidget::setTabText(index,label);
	EditableLabel *l = (EditableLabel*)(tabBar()->tabButton(index,QTabBar::LeftSide));
	l->setToolTip(toolTip);
//	tabBar()->setTabWhatsThis(index,"SAFASDFD");
	tabBar()->setTabToolTip(index,toolTip);
	tabBar()->setTabButton(index,QTabBar::LeftSide,0);
	tabBar()->setTabButton(index,QTabBar::LeftSide,l);
	l->setEditText(label);
	l->setDisplayText("");
}

QString TweetTabWidget::tabText(int index) const {
	if (count()<2)
		return "Tab";
	return ((EditableLabel*)tabBar()->tabButton(index,QTabBar::LeftSide))->getEditText();
}

void TweetTabWidget::handleTabActivity() {
	TabPanel *t = qobject_cast<TabPanel *>(sender());
	int y=currentIndex();
	for (int x=0;x<count();x++) {
		if ((TabPanel*)widget(x)==t && x!=y) {
//			((EditableLabel*)tabBar()->tabButton(x,QTabBar::LeftSide))->setDisplayText("*");
//			QTabWidget::setTabText(x,"*");
		}
	}
//	QTabWidget::setTabText(currentIndex(),"");
//	((EditableLabel*)tabBar()->tabButton(currentIndex(),QTabBar::LeftSide))->setDisplayText("");
}

void TweetTabWidget::refresh() {
	for (int x=0;x<count();x++)
		((TabPanel*)widget(x))->refresh();
}

bool TweetTabWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick) {
    	QPoint point = ((QMouseEvent*)event)->pos();
    	for(int x=0;x<count();x++) {
    		QRect rect = tabBar()->tabRect(x);
    		if (rect.contains(point))
    			((EditableLabel*)tabBar()->tabButton(x,QTabBar::LeftSide))->setEditMode();
    	}
    	return true;
    }
    return QObject::eventFilter(obj,event);
}
