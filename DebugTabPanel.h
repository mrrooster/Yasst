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
 * DebugTabPanel.h
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#ifndef DEBUGTABPANEL_H_
#define DEBUGTABPANEL_H_

#include "TabPanel.h"
#include <QLabel>
#include "TweetTabWidget.h"
#include "backend/BirdBox.h"
#include "backend/ImageCache.h"

class DebugTabPanel: public TabPanel {
	Q_OBJECT
public:
	DebugTabPanel(BirdBox *,ImageCache *,TweetTabWidget *,QWidget *parent=0);
	virtual ~DebugTabPanel();
private:
	int getPrunableTweetCount();
	QString fetcherType(TweetFetcher *f);
	TweetTabWidget *tabs;
	BirdBox *tweets;
	ImageCache *cache;
	QTimer *timer;
	QLabel *label;
	QLayout *layout;
	QLayout *imgLayout;
	QString getImageInfo();
public slots:
	void updateDetails();
};

#endif /* DEBUGTABPANEL_H_ */
