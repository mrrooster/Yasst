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
/* $Id: GroupListWidget.h,v 1.12 2010/07/25 14:51:22 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef GROUPLISTWIDGET_H_
#define GROUPLISTWIDGET_H_

#include "tweetlistwidget.h"
#include "backend/Group.h"

class GroupListWidget: public TweetListWidget {
	Q_OBJECT
public:
	GroupListWidget(QWidget *parent = 0);
	GroupListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
	virtual ~GroupListWidget();
	void setMembers(QList<QString>);
	void setName(QString);
	QString getName();
	void addUserByScreenName(QString);
	void removeUserByScreenName(QString);
	void setGroup(Group*);
public slots:
	void saveGroup();
	virtual void writeSettings(QSettings &);
    virtual void readSettings(QSettings &settings);
    virtual void handleRefreshButton();
    void close();
protected:
	virtual bool canDoUpdate(Tweet *);
protected slots:
	virtual void repopulate();
private:
	void populateList();
	QStringList groupmembers;
	Group *myGroup;
	QString id;
	bool hadSave;
private slots:
	void setListVisible();
	void handleHaveUser(TweetUser *);
	void handleImageUpdate(QString);
	void handleGroupUpdated(Group *);
	void handleCancelPressed();
	void handleItemActivated  ( QListWidgetItem *  );
signals:
	void groupListClosed(QString,QList<QString>);

};

#endif /* GROUPLISTWIDGET_H_ */
