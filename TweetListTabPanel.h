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
 * TweetListTabPanel.h
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#ifndef TWEETLISTTABPANEL_H_
#define TWEETLISTTABPANEL_H_

#include "TabPanel.h"
#include "tweetlistwidget.h"
#include <QHBoxLayout>

class TweetListTabPanel: public TabPanel {
	Q_OBJECT
public:
	TweetListTabPanel(QWidget *parent=0);
	virtual ~TweetListTabPanel();
	//virtual void readSettings(QSettings &);
	virtual void writeSettings(QSettings &);
	virtual void setActive(bool);

	void addListWidget(TweetListWidget *);
	int count();
	TweetListWidget *widgetAt(int);
public slots:
	virtual void refresh();
signals:
	void groupListClosed(QString,QList<QString>);
	void searchClosed(QString,QString);
	void tweetsAdded();
	void activity();
	void popout(TweetListWidget*);
protected:
	QHBoxLayout *layout;
	QTimer drainTimer;
protected slots:
	void handleTweetsAdded(bool mute);
	void handleTweetsPending();
	void handleSetActive();
	void handleSetInactive();
	void handleDrainTimer();
private:
	unsigned int drainTick;
};

#endif /* TWEETLISTTABPANEL_H_ */
