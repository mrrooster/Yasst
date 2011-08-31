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
/* $Id: WebTabPanel.h,v 1.3 2009/06/21 23:12:59 ian Exp $

 File       : WebTabPanel.h
 Create date: 22:51:21 19-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef WEBTABPANEL_H_
#define WEBTABPANEL_H_

#include "TabPanel.h"
#include <QLayout>
#include <QWebView>
#include "BirdBox.h"
#include "ImageCache.h"
#include <QWidget>
#include <QLabel>
#include <QProgressBar>

class WebTabPanel: public TabPanel {
	Q_OBJECT
public:
	WebTabPanel(BirdBox *,ImageCache *,QWidget *parent=0);
	virtual ~WebTabPanel();
protected:
	void displayStatusMessage(QString,TweetUser* =0);
private:
	QLayout *layout;
	BirdBox *tweets;
	ImageCache *cache;
	QWebView *web;
	QWidget *statusArea;
	QLabel *statusIcon;
	QLabel *message;
	TweetUser *statusUser;
	QWidget *infoArea;
	QLabel *loadingText;
	QProgressBar *progress;
	QTimer timer;
	int state;
	QString expectedId;
private slots:
	void resetPage();
	void handleUrlChanged(const QUrl&);
	void handleLinkClicked(const QUrl&);
	void handleLoadStarted();
	void handleLoadFinished(bool);
	void handleUpdatedUser(TweetUser*);
	void handleUpdateImage(QString);
	void handleTimeout();
signals:
	void followUserByScreenName(QString);

};

#endif /* WEBTABPANEL_H_ */
