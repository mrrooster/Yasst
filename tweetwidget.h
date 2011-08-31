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
/* $Id: tweetwidget.h,v 1.32 2010/08/02 14:33:42 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#ifndef TWEETWIDGET_H
#define TWEETWIDGET_H

#include <QtGui/QWidget>
#include <QTimer>
#include <QEvent>
#include "ui_tweetwidget.h"
#include "backend/Tweet.h"
#include "backend/ImageCache.h"
#include "backend/BirdBox.h"
#include "backend/Translator.h"
#include <QStackedLayout>
#include <QMenu>
#include "AbstractTweetWidget.h"
#include "tweetlistwidget.h"

class TweetWidget : public AbstractTweetWidget
{
    Q_OBJECT

public:
    TweetWidget(QWidget *parent = 0);
    TweetWidget(Tweet *t,BirdBox *b,ImageCache *c = 0,QWidget *parent = 0,TweetListWidget *w = 0);
    ~TweetWidget();

	void setCompactMode(bool);
	QString getReplyId();
	BirdBox *getAccount();
    void setFontSize(int point);
    virtual QSize sizeHint() const;
	static QList<QString> lastLang;

	QSize optimisticSizeHint;
public slots:
	void refresh();
	void updateImage(QString);
	void setListMode();
	void setDirectMessageListMode();
	void setReplyMode();
	void setDeleteMode();
	void setUnfollowMode();
	void setRetweetMode();
	void setSendMode();
	void setResendMode();
	void setInReplyToMode();
	void setTweet(Tweet*);
	Tweet* getTweet();
	void setNew();
	void clearNew(bool fast=false);
	bool isNew();
signals:
	void cancel();
	void updated();
	void reply(Tweet *);
	void retweet(Tweet *,bool);
	void directMessage(Tweet *);
	void setFavorite(Tweet *);
	void clearFavorite(Tweet *);
	void timerCancel(TweetWidget *);
	void timerComplete(TweetWidget *);
	void inReplyTo(TweetWidget *);
	void doSearch(QString);
	void deleteTweet(Tweet*);
	void followUserByScreenName(QString);
	void unfollowUserByScreenName(QString);
	void showUserDetails(TweetUser *);
	void showUserDetails(QString);
protected:
	virtual bool event(QEvent *);
	bool eventFilter(QObject *obj, QEvent *event);
    virtual void paintEvent(QPaintEvent *);
	//virtual void changeEvent ( QEvent * );
private:
	bool reverseReply;
	bool listMode;
	bool fav;
	bool compact;
	bool isDirectMessage;
	QString convertToHTML(QString);
	ImageCache *imgcache;
	BirdBox *tweets;
    Ui::TweetWidgetClass ui;
    QTimer *timer;
    QTimer *showTimer;
    QTimer *hideTimer;
	QStackedLayout userImageLayout;
	QPushButton refreshUser;
	QPushButton userInfo;
	QPushButton *retweetIndicatorButton;
	QWidget *userControls;
	QWidget *retweetIndicator;
	ClickableLabel image;
//   QTimer *notNewAnyMoreTimer;
    long timeOut;
    long currentTime;
    bool haveClickedInReplyTo;
    int widgetState;
    static const int stateNormal=0;
    static const int stateDeleting=1;
    static const int stateTimer=2;
    static const int stateUnfollow=3;
    static const int stateRetweet=4;
    Translator *translator;
	Tweet *tweet;
	TweetListWidget *list;
	QMenu userMenu;
	QMenu groupMenu;
	QMenu translateFromMenu;
	QMenu translateToMenu;
	QMenu allLanguagesFrom;
	QMenu allLanguagesTo;
	QString displayScreenName;
//	QWidget *windowWidget;
	bool translated;
	QString translation;
	QString hoverUrl;
	QPoint hoverPos;
	QTimer hoverTimer;
private slots:
	void showControls();
	void hideControls();
	void buttonClicked();
	void haveURL(QString);
	void tick();
	void handleDoubleclick();
	void handleReplyClick();
	void handleRetweetClick();
	void handleDirectMessageClick();
	void handleFollowUser();
	void handleUnfollowUser();
	void handleUpdatedUser(TweetUser*);
	void removeNewIndicator();
	void handleFavorite();
	void handleRefreshUser();
	void handleUserInfo();
	void handleShowGroupMenu();
	void handleShowTranslateFromMenu();
	void handleShowTranslateToMenu();
	void handleTranslateFrom();
	void handleTranslateTo();
	void handleTranslateResult(int,QString,QString,QString resp);
	void handleCustomContextMenuRequested ( const QPoint & pos );
	void handleGroupMenuTriggered(QAction*);
	void handleRemoveFromThisGroup();
	void handleLinkHovered(QString);
	void handleLinkHoverTimer();
};

#endif // TWEETWIDGET_H
