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
/* $Id: TweetPanel.h,v 1.20 2010/10/05 18:08:51 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#ifndef TWEETPANEL_H
#define TWEETPANEL_H

// Qt
#include <QStackedLayout>
#include <QToolBar>
#include <QtGui/QWidget>
#include <QList>
// backend
#include "backend/Tweet.h"
#include "backend/BirdBox.h"
#include "backend/ImageCache.h"
// frontend
#include "AutocompleteWidget.h"
#include "tweetwidget.h"
#include "TweetTextHighlighter.h"
#include "URLShortener.h"
#include "ui_TweetPanel.h"

class TweetPanel : public QWidget
{
    Q_OBJECT

public:
    TweetPanel(ImageCache *c = 0,QString="",QWidget *parent = 0);
    ~TweetPanel();

    void setRssAccount(BirdBox *b);
public slots:
	void tweetClicked(void);
	void retweet(Tweet *,bool);
	void reply(Tweet *);
	void directMessage(Tweet *);
    void setAccount(BirdBox*);
signals:
	void twitter(BirdBox*,QString,QString,QString);
	void twitterDirectMessage(BirdBox*,QString,QString,QString);
	void accountChanged(BirdBox*);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private slots:
	void handleAccountChange();
	void textChanged();
	void cancelReply();
	void returnPressed();
	void escapePressed();
	void shortenClicked();
	void cancelSend(TweetWidget *w);
	void doSend(TweetWidget *w);
	void handlePastedUrl(QString);
	void handleShortUrl(QString longUrl,QString shortUrl);
	void handleImageUpload();
	void cancelUploadImage();
	void handleFailedTweet(QString,QString,QString);
	void handleFailedDirectMessage(QString,QString);
	void handleAccountsChanged(int);
	void addAccount(BirdBox*);
	void removeAccount(BirdBox*);
	void accountAuthChanged();
	void updateImage(QString);
	void refreshToolbar();
	void handleShowTranslateToMenu();
	void handleTranslateTo();
	void handleTranslateResult(int,QString,QString,QString resp);
private:
    Ui::TweetPanelClass ui;
	ImageCache *imgcache;
	BirdBox *tweets;
	BirdBox *prevtweets;
	TweetWidget *replyWidget;
	QString replyId;
	QString user;
	TweetTextHighlighter *highlighter;
	QList<QString> urlsToShorten;
	URLShortener shortener;
	QString uploadImageFilename;
	QStackedLayout imageUploadLayout;
	QLabel image;
	QPushButton cancelImage;
	QToolBar *toolBar;
	QList<QIcon> inactiveIcon;
	QList<QIcon> activeIcon;
	AutocompleteWidget *autocompleter;
	QMenu translateToMenu;
	QMenu allLanguagesTo;
	Translator *translator;
	BirdBox *rssAccount;

	void setUploadImage(QString filename);
};

#endif // TWEETPANEL_H
