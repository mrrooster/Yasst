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
 * TweetTabWidget.h
 *
 *  Created on: 24-Mar-2009
 *      Author: ian
 */

#ifndef TWEETTABWIDGET_H_
#define TWEETTABWIDGET_H_

#include <QTabWidget>
#include <QList>
#include "TabPanel.h"
#include "TweetListTabPanel.h"
#include "tweetlistwidget.h"
#include "EditableLabel.h"

class TweetTabWidget : public QTabWidget {
	Q_OBJECT
public:
	TweetTabWidget(QWidget *p=0);
	virtual ~TweetTabWidget();

	int addTab(QWidget *p,const QString l);
	void hideTabBar();
	void showTabBar();
	TweetListTabPanel *addListView();
	void addListWidget(TweetListWidget *);
	void readSettings(QSettings &);
	void writeSettings(QSettings &);
	void setTabText(int index, const QString & label );
	QString tabText ( int index ) const;
	void handleTabCountChange();
	void closeActiveTab();
	void setActive();
public slots:
	void refresh();
signals:
	void groupListClosed(QString,QList<QString>);
	void searchClosed(QString,QString);
protected:
	TweetListTabPanel *activeList;
	bool eventFilter(QObject *obj, QEvent *event);
private slots:
	void handleCurrentChanged(int);
	void handleTabCloseRequested(int i);
	void handleTabRename(EditableLabel *);
	void handleTabEditing(EditableLabel *);
	void handleTabActivity();
};

#endif /* TWEETTABWIDGET_H_ */
