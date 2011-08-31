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
/* $Id: ya_st.h,v 1.50 2010/09/14 13:49:05 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#ifndef YA_ST_H
#define YA_ST_H

/**************
** Package maintainers should change the below to indicate their package
**
**/
#define PACKAGE_SUFFIX "-gpl"


#include <QtGui/QMainWindow>
#include <QString>
#include "logonwidget.h"
#include "backend/BirdBox.h"
#include "backend/fetchers/TweetFetcher.h"
#include "backend/fetchers/UserGroupFetcher.h"
#include "backend/ImageCache.h"
#include <QScrollArea>
#include <QTimer>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QMenu>
#include "OptionsWindow.h"
#include "GroupManagementWindow.h"
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include "tweetlistwidget.h"
#include "TweetTabWidget.h"
#include "backend/fetchers/UserDirectMessageFetcher.h"
#include <QSystemTrayIcon>
#include "backend/fetchers/UserFavoriteMessageFetcher.h"
#include "ProgramUpdateCheckFetcher.h"
#include "TwitterOAuthPinWidget.h"

class Ya_st : public QMainWindow
{
    Q_OBJECT

public:
    Ya_st(ImageCache *i,QWidget *parent = 0);
    ~Ya_st();
    static QString labelCSS;
    static QString mainCSS;
	void addNewAccount(BirdBox *);
	void tweetAndQuit(QString tweet,QString tweetAs);
public slots:
	void handleLogon(QString,QString,bool);
	void handleLogonSkipped();
	void handleTweet(BirdBox*,QString,QString,QString);
	void handleDeleteTweet(Tweet *t);
	void handleTweetDirectMessage(BirdBox*,QString,QString,QString);
	void userUpdate(TweetUser *);
	void newFriendsList();
	void showDirectMessages();
	void showFavourites();
	void newGroup();
	void newSearch();
	void newFeedList();
	void newTrendsList();
	void showAll();
	void showReplies();
	void showOpts();
	void showGroupManagement();
	void authError(HTTPHandler*);
	void otherError(HTTPHandler*);
	void statusChange(int active,int total, int tweetcount);
	void rateUpdate(long count,long secondsToReset);
	void handleOptionsChange();
	void refreshTweets();
	void scrollToList(TweetListWidget*);
	void handleDoSearch(QString);
	void setStyle(int );
	void setStyle(QString main,QString text);
	void handlePopout(TweetListWidget*);
	void handleAccountChanged(BirdBox *b);
protected:
	void closeEvent(QCloseEvent *);
	void changeEvent ( QEvent * );
private:
	bool notifyOpening;
    QHBoxLayout *layout;
//    BirdBox *box;
    BirdBox *activeAccount;
    BirdBox *rssAccount;
    ImageCache *imagecache;
    OptionsWindow *opts;
    GroupManagementWindow *groups;
    QTimer *dockTimer;
	QDockWidget *dock;
	QWidget *notifyArea;
	QLabel *notifyText;
	QTimer notifyTimer;
	QTimer notifyOpenTimer;
	uint lastNotificationTime;
//	TweetFetcher *tmp;
//	TweetFetcher *userFetcher;
//	TweetFetcher *friendFetcher;
//	TweetFetcher *mentionFetcher;
//	TweetFetcher *groupFetcher;
//	UserDirectMessageFetcher *directMessageFetcher;
//	UserFavoriteMessageFetcher *favoriteFetcher;
#ifndef GPL
	ProgramUpdateCheckFetcher *updateChecker;
#endif
    int targetSize;
    TweetTabWidget *tabs;
    QToolBar *toolBar;
	QAction *newGroupAct;
	QAction *newSearchAct;
	QAction *repliesAct;
	QAction *dockToggleAct;
	QAction *allTweetsAct;
	QAction *refreshAct;
	QAction *optionsAct;
	QAction *friendsAct;
	QAction *directMessageAct;
	QAction *favoritesAct;
	QAction *newListTabAct;
	QAction *closeActiveTabAct;
	QAction *findPeopleAct;
	QAction *feedAct;
	QAction *trendsAct;
	QAction *quitAct;
	QAction *twitterListAct;
	QAction *twitterFollowListAct;
	QAction *closedGroupAct;
	QAction *changeAcctAct;
	QAction *pruneReadTweets;
	QMenu listMenu;
	QMenu followingListMenu;
	QMenu accountsMenu;
	QMenu groupMenu;
	QMenu *groupButtonMenu;
	QMenu *searchMenu;
	QMenu *trayMenu;
	QMainWindow *popout;
	QWidget *popoutList;
	QLayout *popoutLayout;
	bool poppedOut;
	int popoutPos;
	bool loggedOn;
	QString userName;
	int active;
	int total;
	int tweetcount;
	int closedGroupCount;
	int closedSearchCount;
	int uploadingImageCount;
	long api;
	long secs;
	QDateTime lastactive;
	QStatusBar *status;
	QLabel *statusTextWidget;
	Phonon::MediaObject *mediaObject;
	Phonon::AudioOutput *audioOutput;
	Phonon::Path path;
	QSystemTrayIcon trayIcon;
	bool ignoreRaisedWindow;
	QString doTweet;
	QString doTweetAs;

	void updateStatus();
	void writeSettings();
	void readSettings();
	void checkSettings();
	bool doStoredLogin();
	void setupActions();
	void setupToolBar();
	void setupMenu();
	void restoreWindows();
	void insertList(TweetListWidget*);
	void insertList(TweetListWidget*,BirdBox*);
	void addFetcher(TweetFetcher*);
	void notify(QString);
	void doNotifications(bool alwaysShow=false);
	void doTweetAndQuit(QString tweet,QString tweetAs);
private slots:
	void init();
	void scrollToLast();
	TabPanel* newListTab();
	void closeActiveTab();
	void handleChildUpdate();
	void handleStartFetching();
	void handleEndFetching();
	void handleFollowUserByScreenName(QString);
	void handleUnfollowUserByScreenName(QString);
	void handleGroupListClosed(QString,QList<QString>);
	void restoreGroup();
	void handleSearchClosed(QString,QString);
	void restoreSearch();
	void openFindPeopleTab();
	void handleStartImageUpload();
	void handleStopImageUpload();
	void handleTrayIconActivated ( QSystemTrayIcon::ActivationReason reason );
	void handleQuit();
	void handleRetweet(Tweet*,bool);
	void handleFavoriteTweet(Tweet*);
	void handleUnfavoriteTweet(Tweet*);
	void handleshowUserDetails(TweetUser *);
	void handleshowUserDetails(QString);
	void handleNotifyTimer();
	void handleNotifyOpenTimer();
	void handlePopoutClosed();
	void handleShowGroupMenu();
	void handleShowAccountsMenu();
	void handleListMenuTriggered(QAction *act);
	void handleAccountsMenuTriggered(QAction *act);
	void updateImage(QString);
	void handleAccountRemoved(BirdBox *b);
	void handleExplicitPruneReadTweets();
	void handlePINRequest(OAuth*,QString);
signals:
	void accountChanged(BirdBox*);
};

#endif // YA_ST_H
