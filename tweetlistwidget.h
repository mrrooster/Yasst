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
/* $Id: tweetlistwidget.h,v 1.44 2010/07/27 20:02:17 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#ifndef TWEETLISTWIDGET_H
#define TWEETLISTWIDGET_H

#include <QtGui/QWidget>
#include "ui_tweetlistwidget.h"
#include "backend/Tweet.h"
#include "backend/BirdBox.h"
#include <QVBoxLayout>
#include <QSettings>
#include <QHash>
#include <QMenu>
#include "backend/ImageCache.h"
//#include "tweetwidget.h"
#include <QLayout>
#include <QMouseEvent>
#include "AbstractTweetWidget.h"

class TweetWidget;

class TweetListWidget : public QWidget
{
    Q_OBJECT

public:
    TweetListWidget(QWidget *parent = 0);
    TweetListWidget(BirdBox *b,ImageCache *i=0, QWidget *parent = 0);
    virtual ~TweetListWidget();
    void removeAllTweets(bool=false);
    virtual void writeSettings(QSettings &settings);
    virtual void readSettings(QSettings &settings);
    void setLayout(QLayout *);
	void setActive(bool active);
	void setPopoutMode(bool);
	virtual QString title();
	virtual void setTitle(QString);
	bool hasPendingTweets();
    int getUnreadWidgetCount();
    QString getName();
    QPixmap getIcon();
    bool isMuted();

    int type;
    int minCount;
    int maxAgeMins;

protected:
	QString deHTML(QString);
	void setStack(int x);
	virtual bool canDoUpdate(Tweet *);
	QMutex tweetsLock;
	QMutex knownTweetsLock;
	QList<Tweet*> pendingTweets;
	void doTweetUpdate(Tweet *);
	void startDrainTimer();
	void pruneRead(bool);
	void removeUnwantedTweets();
    BirdBox *tweets;
    ImageCache *images;
    Ui::TweetListWidgetClass ui;
    QHash<QString,AbstractTweetWidget*> inReplyToTweets;
    QHash<Tweet*,AbstractTweetWidget*> knownTweets;
    QString defaultTitleStyle;
    QLayout *layout;
    bool firstRefresh;
    bool canClearFirstRefresh;
    bool inRefresh;
    bool isActive;
    bool muted;
    bool popoutMode;
    bool doingResize;
    bool doingDrag;
    bool otherDrag;
    bool havePrunedTweets; // If this is set, don't add any tweets marked as read.
    int filteredCount;
    int tweetClamp;
    int maxCreateLimit;

    int getWidgetCount(bool=false);
    void mouseMoveEvent ( QMouseEvent *  );
    void mouseReleaseEvent ( QMouseEvent *  );
    void mousePressEvent ( QMouseEvent *  ) ;
protected slots:
	void handleDeleteTweet(Tweet *);
	void slowDelete();
    virtual void setAccountIcon();
private:
	bool widgetIsFiltered(AbstractTweetWidget*);
	bool lastUnreadIsVisible();
	void refreshTweet(Tweet*);
	AbstractTweetWidget* lastUnreadWidget();
	QString filterText;
    QTimer *filterTimer;
    QTimer *timer;
    QTimer *clearNewTimer;
    QWidget *filler;
    unsigned int fillerTimestamp;
    int scrollBarLength;
    int inRefreshCount;
    int popoutWidth;
    int xOff;
    int yOff;
    bool lastWasMe;
    static int masterCount;
	QMenu pruneMenu;

public slots:
	virtual void tweetUpdate(Tweet *);
	virtual void tweetUpdates(QList<Tweet *>&);
	virtual void handleRefreshButton() {};
	void childUpdate();
	void setTweetsVisible();
	void prune();
	virtual void refresh();
//	virtual void handleClose() {};
	void drainTweets();
protected slots:
	virtual void repopulate();
    void setTweetCount();
	void hideTopText();
	void showTopText();
	void handlePruneRead();
	void handlePruneReadSingle();
private slots:
	void filterTimerFired();
	void filterTextChanged(QString);
	void hideFiltering();
	void hideFilteringHelp();
	void showFilteringHelp();
	void showFiltering();
	void haveURL(QString);
	void handleInReplyTo(TweetWidget*);
	void handleFinishedFirstFetch();
	void toggleMute();
	void expireNewIndicator();
	void handleScrollbarValueChanged(int);
	void markAllAsRead();
	void handlePopout();
signals:
	void cancel();
	void showMe(TweetListWidget *);
	void tweetsAdded(bool muted);
	void popout(TweetListWidget*);
	void titleChanged(QString);
	void tweetsPending();
	void removeTweet(Tweet *);
};

#endif // TWEETLISTWIDGET_H
