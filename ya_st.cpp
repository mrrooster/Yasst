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
/* $Id: ya_st.cpp,v 1.94 2010/10/05 18:08:51 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#include "ya_st.h"
#include "yasstFns.h"
#include <QMessageBox>
#include <QDockWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QSettings>
#include <QSpacerItem>
#include <QNetworkProxy>
#include <QDateTime>
#include <QScrollBar>
#include <QWidget>
#include <QMenuBar>
#include <QFile>
#include <QToolButton>
#include <QUuid>
#include <QGraphicsBlurEffect>
#include <QFileDialog>

#include "YasstApplication.h"
#include "tweetlistwidget.h"
#include "GroupListWidget.h"
#include "SearchListWidget.h"
#include "ReplyListWidget.h"
#include "DirectMessageListWidget.h"
#include "FavoriteListWidget.h"
#include "FriendsListWidget.h"
#include "FeedListWidget.h"
#include "TrendingTopicsListWidget.h"
#include "backend/fetchers/UserTweetFetcher.h"
#include "backend/fetchers/TweetFetcher.h"
#include "backend/fetchers/UserReplyFetcher.h"
#include "backend/fetchers/UserDirectMessageFetcher.h"
#include "backend/fetchers/UserFavoriteMessageFetcher.h"
#include "backend/fetchers/FriendFetcher.h"
#include "backend/fetchers/TwitrpixImageHandler.h"
#include "backend/fetchers/UserGroupFollowingFetcher.h"
#include "DebugTabPanel.h"
#include "TweetPanel.h"
#include "AllTweetsListWidget.h"
//#include "WebTabPanel.h"
#include "UserListWidget.h"
#include "PopoutWidget.h"
#include "PasswordWidget.h"
#include "NotificationWidget.h"

QString Ya_st::labelCSS="";
QString Ya_st::mainCSS="";

Ya_st::Ya_st(ImageCache *i,QWidget *parent)
    : QMainWindow(parent),activeAccount(0),opts(0),dockTimer(0),popout(0),dock(0),notifyArea(0),notifyText(0),updateChecker(0),tabs(0),toolBar(0),newListTabAct(0),groupMenu(0),searchMenu(0),loggedOn(false),closedGroupCount(0),closedSearchCount(0),changeAcctAct(0),uploadingImageCount(0)
{
//	box=b;
/*
	if (TweetFetcher::file==0) {
		TweetFetcher::file = new QFile(QFileDialog::getSaveFileName(0,"Select log file","Yasst_debug.txt"));
		TweetFetcher::file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	}
*/
	imagecache=i;
	this->setWindowTitle("Ya@st");

	lastNotificationTime = QDateTime::currentDateTime().toTime_t();

	toolBar = addToolBar(tr(""));
	toolBar->setVisible(false);
	toolBar->setObjectName("maintoolbar");
	tabs=0;
	status=new QStatusBar(this);
	statusTextWidget = new QLabel(this);
	statusTextWidget->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	this->setStatusBar(status);
	status->addWidget(statusTextWidget,1);

//	notifyTimer.setSingleShot(true);
	notifyTimer.setInterval(35000);
	notifyOpenTimer.setInterval(20);

	QObject::connect((YasstApplication*)qApp,SIGNAL(accountRemoved(BirdBox*)),this,SLOT(handleAccountRemoved(BirdBox*)));
	QObject::connect(imagecache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)),Qt::QueuedConnection);
	QObject::connect(&trayIcon,SIGNAL(activated ( QSystemTrayIcon::ActivationReason )),this,SLOT(handleTrayIconActivated ( QSystemTrayIcon::ActivationReason )));
	QObject::connect(&notifyTimer,SIGNAL(timeout()),this,SLOT(handleNotifyTimer()));
	QObject::connect(&notifyOpenTimer,SIGNAL(timeout()),this,SLOT(handleNotifyOpenTimer()));
	active=total=tweetcount=api=secs=0;
	opts = new OptionsWindow(i,this);
	groups = new GroupManagementWindow(i,this);
	QObject::connect(opts,SIGNAL(optionsSaved()),this,SLOT(handleOptionsChange()));
	mediaObject = new Phonon::MediaObject(this);
	audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	path = Phonon::createPath(mediaObject, audioOutput);
	setWindowIcon(QIcon(":/appicon/yasst.png"));
	resize(285,523);
	readSettings();
	setCentralWidget(new QWidget());
	setupActions();
	setupToolBar();
#ifdef Q_WS_MAC
	setupMenu();
#endif
	handleOptionsChange();
	lastactive=QDateTime::currentDateTime();

	setMinimumWidth(285);
	QTimer::singleShot(0, this, SLOT(init()));

	QString ver = getVersionString();//QDateTime::fromString("$Date: 2010/10/05 18:08:51 $","'$''Date: 'yyyy/MM/dd HH:mm:ss' $'").toString("yyyyMMdd");
	setWindowTitle(windowTitle()+tr(" (Build ")+ver+tr(")"));
#ifdef Q_WS_WIN
	HTTPHandler::userAgent="Yasst/"+ver+" (Windows)";
#else
#ifdef Q_WS_MAC
	HTTPHandler::userAgent="Yasst/"+ver+" (OSX)";
#else
	HTTPHandler::userAgent="Yasst/"+ver+" (X11)";
#endif
#endif
//	setStyleSheet("QWidget#TweetWidget{background-color: black;}");
/*
	QPalette pal = palette();
	pal.setColor(QPalette::Window,Qt::darkCyan);
	pal.setColor(QPalette::Foreground,Qt::white);
	setPalette(pal);
*/
	setDocumentMode(true);
	ignoreRaisedWindow=false;

	groupButtonMenu = new QMenu(this);
    newGroupAct->setMenu(groupButtonMenu);
    QAction *ngAct = groupButtonMenu->addAction("Manage lists");
    ngAct->setIcon(QIcon(":/buttons/list.png"));
    QObject::connect(ngAct, SIGNAL(triggered()), this, SLOT(showGroupManagement()));
    ngAct = groupButtonMenu->addAction("New group");
    ngAct->setIcon(QIcon(":/buttons/group.png"));
    QObject::connect(ngAct, SIGNAL(triggered()), this, SLOT(newGroup()));
    QObject::connect(groupButtonMenu,SIGNAL(aboutToShow()),this,SLOT(handleShowGroupMenu()));
    groupButtonMenu->addSeparator();
    listMenu.setTitle(tr("My lists"));
    followingListMenu.setTitle(tr("Lists I follow"));
	QObject::connect(&listMenu,SIGNAL(triggered(QAction *)),this,SLOT(handleListMenuTriggered(QAction*)));
	QObject::connect(&followingListMenu,SIGNAL(triggered(QAction *)),this,SLOT(handleListMenuTriggered(QAction*)));
	QObject::connect(&accountsMenu,SIGNAL(aboutToShow()),this,SLOT(handleShowAccountsMenu()));
	QObject::connect(&accountsMenu,SIGNAL(triggered(QAction *)),this,SLOT(handleAccountsMenuTriggered(QAction*)));
	twitterListAct = groupButtonMenu->addMenu(&listMenu);
	twitterFollowListAct = groupButtonMenu->addMenu(&followingListMenu);
    groupMenu.setTitle("Closed groups");
    closedGroupAct = groupButtonMenu->addMenu(&groupMenu);

	trayMenu=new QMenu(this);
	trayMenu->addAction(optionsAct);
	trayMenu->addSeparator();
	trayMenu->addAction(quitAct);

	trayIcon.setIcon(QIcon(":/appicon/yasst.png"));
	trayIcon.setContextMenu(trayMenu);

	i->setMaxSize(40,40);
//	trayIcon.show();

	targetSize=0;
	poppedOut=false;

	rssAccount=new BirdBox();
#ifndef GPL
	if (!updateChecker) {
		updateChecker=new ProgramUpdateCheckFetcher();
		updateChecker->setVersion(getVersionString());
		rssAccount->addFetcher(updateChecker);
		updateChecker->populate();
	}
#endif
}

Ya_st::~Ya_st()
{
	delete opts;
}

void Ya_st::init() {
	if (!doStoredLogin()) {
		setCentralWidget(new LogonWidget(this));
		if (toolBar)
			toolBar->setVisible(false);
		QObject::connect(centralWidget(),SIGNAL(logon(QString,QString,bool)),this,SLOT(handleLogon(QString,QString,bool)));
		QObject::connect(centralWidget(),SIGNAL(logonSkipped()),this,SLOT(handleLogonSkipped()));
	} else if (doTweet!="" && doTweetAs!="") {
		doTweetAndQuit(doTweet,doTweetAs);
	}
}

void Ya_st::handleOptionsChange() {
	QSettings opts;
	QNetworkProxy::ProxyType type = QNetworkProxy::NoProxy;

	switch(opts.value("proxyType",QVariant(0)).toInt()) {
		case 0: // None
			type = QNetworkProxy::NoProxy;
			break;
		case 1: // SOCKS5
			type = QNetworkProxy::Socks5Proxy;
			break;
		case 2: // HTTP
			type = QNetworkProxy::HttpCachingProxy;
			break;
	}
	QNetworkProxy proxy(type,opts.value("proxyHost","").toString(),opts.value("proxyPort","").toInt(),opts.value("proxyUser","").toString(),opts.value("proxyPass","").toString());
	QNetworkProxy::setApplicationProxy(proxy);
	QString alert = opts.value("sounds/newTweet","").toString();
	if (alert=="")
		mediaObject->setCurrentSource(Phonon::MediaSource(new QFile(":/sounds/new_tweets.mp3")));
	else
		mediaObject->setCurrentSource(Phonon::MediaSource(alert));
	if (tabs)
		tabs->refresh();
	int theme = opts.value("currentTheme",2).toInt();
	if (theme!=this->opts->initialTheme)
		setStyle(theme);

	if (opts.value("trayicon",QVariant(false)).toBool())
		trayIcon.show();
	else {
		trayIcon.hide();
		if (!isVisible())
			handleQuit();
	}
	QString newUser = opts.value("accounts/twitter/user","").toString();
	QString newPass = QString(QByteArray::fromBase64(opts.value("accounts/twitter/pass","").toByteArray()));
	if (newUser!="") {
		refreshTweets();
	}
}

void Ya_st::statusChange(int ,int total, int tweetcount) {
//	this->active=active;
	this->total=total;
	this->tweetcount=tweetcount;
	updateStatus();
}

void Ya_st::rateUpdate(long count,long secondsToReset) {
	api=count;
	secs=secondsToReset;
	updateStatus();
}

void Ya_st::updateStatus(){
	if (!activeAccount)
		return;
	QString statusText;

	long secs = this->secs;
	if (api>0)
		secs=secs/api;
	long count = activeAccount->fetchersCount();
	long total = activeAccount->fetchersCount()+activeAccount->freeFetchersCount();
	long active = activeAccount->activeFetchersCount();

	if (active>total)
		total=active;
	if (count)
		secs*=count;
	QDateTime update = QDateTime::currentDateTime().addSecs((this->secs>10)?(10*60):(this->secs*60));
	for (int x=0;x<activeAccount->fetchersCount();x++) {
		if (!activeAccount->fetchersAt(x)->nextUpdate.isNull() && activeAccount->fetchersAt(x)->nextUpdate<update)
			update=activeAccount->fetchersAt(x)->nextUpdate;
	}
	QString apiStatus = QString::number(api)+tr(" API left, next refresh at ")+update.toString("hh:mm ");
	if (api==0&&(secs<15?15:secs)==0)
		apiStatus="";
	if (active==0) {
		statusText=apiStatus+tr("Last update ")+QString::number(tweetcount)+tr(" tweet")+(tweetcount!=1?tr("s."):tr("."));
	} else {
		statusText=apiStatus+QString::number(active)+tr("/")+QString::number(total)+tr(", Last update ")+QString::number(tweetcount)+tr(" tweet")+(total!=1?tr("s"):tr(""))+tr(".");
	}
	if (uploadingImageCount)
		statusText+=tr(" ")+QString::number(uploadingImageCount)+tr(" image upload")+(uploadingImageCount!=1?tr("s"):tr(""))+tr(" in progress.");
//	refreshAct->setIconText(statusText);
	if (activeAccount->hasCredentials())
		statusText+=" ["+activeAccount->getAuthenticatingUserScreenName()+"]";
	statusTextWidget->setText(statusText);
//	status->showMessage(statusText);
}

void Ya_st::handleChildUpdate() {
	QSettings opts;
	if (lastactive.addSecs(30)<QDateTime::currentDateTime()) {
		if (opts.value("playnewtweetalert",QVariant(true)).toBool()) {
			mediaObject->stop();
			mediaObject->play();
			doNotifications(true);
		}
		lastactive=QDateTime::currentDateTime();
		if (opts.value("activatewindow",QVariant(false)).toBool())
			this->activateWindow();
	}
}

void Ya_st::authError(HTTPHandler* http) {
	TweetFetcher *fetcher = (TweetFetcher*)http;
	if (fetcher->type==2||fetcher->type==8) {
		BirdBox *b = fetcher->getBirdBox();
		QString acct = "";
		if (b->getAuthenticatingUserScreenName()!="")
			acct = " ["+b->getAuthenticatingUserScreenName()+"]";
		notify(tr("<b>Authentication error or twitter down")+acct+tr("</b><br>This may be a temporary twitter issue, but may indicate an incorrect user name or password. This can be changed in the options."));
/*
		LogonWidget *center;
		QSettings settings;
		center = new LogonWidget(this);
		center->setFailedLogon(userName);
		settings.remove("accounts/twitter/user");
		settings.remove("accounts/twitter/pass");
		removeDockWidget(dock);
		dockToggleAct->deleteLater();
		dock->deleteLater();
		setCentralWidget(center);
		box->removeAllFetchers();
		loggedOn=false;
		QObject::connect(center,SIGNAL(logon(QString,QString,bool)),this,SLOT(handleLogon(QString,QString,bool)));
		toolBar->setVisible(false);
		tabs=0;//tabs->clear();
		notifyText=0;
		notifyArea=0;
*/
	}
}

void Ya_st::otherError(HTTPHandler* h) {
	if (h->type==0)
		return;
	notify(tr("<b>Network error or twitter down (")+QString::number(h->type)+tr(")</b><br>There has been an error, Yasst will keep retrying..."));
}

bool Ya_st::doStoredLogin() {
	QSettings settings;
	int count = settings.beginReadArray("accounts/twitter");
	for (int x=0;x<count;x++) {
    	settings.setArrayIndex(x);
		QString user = QString(QByteArray::fromBase64(settings.value("username","").toByteArray()));
//		QString pass = QString(QByteArray::fromBase64(settings.value("password","").toByteArray()));
		QString token = settings.value("token","").toString();
		QString tokenSecret = settings.value("tokenSecret","").toString();
		QString id = settings.value("id",QUuid::createUuid().toString()).toString();
		BirdBox *b = new BirdBox();

		b->getOAuth()->setTokenAndSecret(token,tokenSecret);
		QObject::connect(b,SIGNAL(fetcherAdded(TweetFetcher*)),opts,SLOT(setupFetcherWeights(TweetFetcher*)));
		QObject::connect(b,SIGNAL(PINRequest(OAuth *,QString)),this,SLOT(handlePINRequest(OAuth *,QString)));
		b->setUser(user);
		b->setId(id);
//		if (pass.isNull() || pass=="") {
//			PasswordWidget *w = new PasswordWidget(user,this);
//			w->setFocus();
//			if (w->exec()==QDialog::Accepted) {
//				pass = w->getPassword();
//				b->setRememberMe(w->getRememberMe());
//			}
//		}
//		b->setPassword(pass);

		TweetFetcher *f = new UserGroupFetcher();
		b->addFetcher(f,true);
		f = new UserGroupFollowingFetcher();
		b->addFetcher(f,true);
		f = new FriendFetcher();
		b->addFetcher(f,true);
/*
		f = new UserTweetFetcher();
		if (!userFetcher)
			userFetcher = f;
		f->weight=settings.value("tweetFetcherWeight",75).toInt();
		b->addFetcher(f);
//		userFetcher->doLogon(); // populates the authenticating user.
//		userFetcher->populateUsers();
		f->setInitialFetchCount(settings.value("startupTweetFetchCount",200).toInt());
		f->populate();
*/

		addNewAccount(b);
	}
	settings.endArray();

	if (((YasstApplication*)qApp)->getAccounts().size()>0 || settings.value("skiplogon",false).toBool()) {
		handleLogon("","",false);
		return true;
	}
	return false;
}

void Ya_st::handleLogonSkipped() {
	QSettings settings;
	settings.setValue("skiplogon",true);
	handleLogon("","",false);
}

void Ya_st::handleLogon(QString user,QString pass,bool remember) {
	QSettings opts;
	toolBar->setVisible(true);
	loggedOn=true;
	userName=user;

	if (user!="") {
		BirdBox *b = new BirdBox();

		QObject::connect(b,SIGNAL(fetcherAdded(TweetFetcher*)),this->opts,SLOT(setupFetcherWeights(TweetFetcher*)));
		QObject::connect(b,SIGNAL(PINRequest(OAuth *,QString)),this,SLOT(handlePINRequest(OAuth *,QString)));

		b->setUser(user);
		b->setRememberMe(remember);
		b->setPassword(pass);

		TweetFetcher *f = new UserGroupFetcher();
		b->addFetcher(f,true);
		f = new UserGroupFollowingFetcher();
		b->addFetcher(f,true);

		f = new FriendFetcher();
		b->addFetcher(f);
		f->populate();
		addNewAccount(b);
	}
	QSettings settings;
//		if (remember) {
//			settings.setValue("accounts/twitter/user",user);
//			settings.setValue("accounts/twitter/pass",pass.toLatin1().toBase64());
//		}
//		BirdBox *box = new BirdBox();
	TweetPanel *panel = new TweetPanel(imagecache,user);
	panel->setRssAccount(rssAccount);

	dock = new QDockWidget(0);
	dock->hide();
	dock->setFeatures(QDockWidget::DockWidgetFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable));
	dock->setWindowTitle(tr("Speak your brains..."));
	dock->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::TopDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea,dock);
	dock->setObjectName("twitterbox");
	dock->setWidget(panel);
	dockToggleAct=dock->toggleViewAction();
	dockToggleAct->setIconText(tr("Tweet!"));
	dockToggleAct->setIcon(QIcon(":/buttons/tweet.png"));
	toolBar->insertAction(refreshAct,dockToggleAct);
	restoreDockWidget(dock);
	dock->setVisible(settings.value("mainwindow/tweetpanelvisible",true).toBool());
	QObject::connect(panel,SIGNAL(twitter(BirdBox*,QString,QString,QString)),this,SLOT(handleTweet(BirdBox*,QString,QString,QString)));
	QObject::connect(panel,SIGNAL(twitterDirectMessage(BirdBox*,QString,QString,QString)),this,SLOT(handleTweetDirectMessage(BirdBox*,QString,QString,QString)));
	QObject::connect(panel,SIGNAL(accountChanged(BirdBox*)),this,SLOT(handleAccountChanged(BirdBox *)));
	QObject::connect(this,SIGNAL(accountChanged(BirdBox*)),panel,SLOT(setAccount(BirdBox *)));
//		QObject::connect(panel,SIGNAL(tweetsAdded()),this,SLOT(handleChildUpdate()));
	getVersion();
//	box->addFetcher(tmp);
	restoreWindows();

}

void Ya_st::restoreWindows() {
	//TODO: Move this into the tab/tabpanel classes, need to sort out signals for update notification first.
	QSettings settings;
	int tabcount=settings.beginReadArray("tabs");
	int count,type;
	BirdBox *box = activeAccount; //TODO: Change for restor for multiple accounts

	if (!tabs) {
		QVBoxLayout *layout = new QVBoxLayout();
		QHBoxLayout *notifyLayout = new QHBoxLayout();
		QWidget *center = new QWidget();
		layout->setMargin(0);
		layout->setSpacing(0);
		notifyLayout->setMargin(5);
		notifyArea = new QWidget();
		notifyArea->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
		notifyArea->hide();
		notifyText = new QLabel();
		notifyText->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
		notifyText->setWordWrap(true);
		tabs = new TweetTabWidget();
		QObject::connect(tabs,SIGNAL(groupListClosed(QString,QList<QString>)),this,SLOT(handleGroupListClosed(QString,QList<QString>)));
		QObject::connect(tabs,SIGNAL(searchClosed(QString,QString)),this,SLOT(handleSearchClosed(QString,QString)));

		center->setLayout(layout);
		layout->addWidget(notifyArea);
		notifyArea->setLayout(notifyLayout);
		notifyLayout->addWidget(notifyText);
		layout->addWidget(tabs);


		setCentralWidget(center);
	}
	if (tabcount==0) {
		newListTab();
		if (((YasstApplication*)qApp)->getAccounts().size()>0) {
			insertList(new FriendsListWidget(box,imagecache));
		} else
			insertList(new FeedListWidget(rssAccount,imagecache));
	}

	for (int y=0;y<tabcount;y++) {
		settings.setArrayIndex(y);
		type=settings.value("type").toInt();
		switch(type) {
//			case 1 :
//				tabs->addTab(new WebTabPanel(box,imagecache),"Find people");
//				break;
			case 9999:
				tabs->addTab(new DebugTabPanel(box,imagecache,tabs),"Debug");
				break;
			case 0:
			default:
				newListTab()->readSettings(settings);
				count=settings.beginReadArray("columns");
				QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
				for (int x=count-1;x>=0;x--) {
					settings.setArrayIndex(x);
					type=settings.value("type").toInt();
					QString id = settings.value("account","").toString();
					BirdBox *box = 0;
					for(int y=0;y<accts.count();y++) {
						if (accts.at(y)->getId()==id) {
							box=accts.at(y);
							break;
						}
					}
					if (!box && type!=8)
						continue;
					TweetListWidget *w;

					// 0 - TweetListWidget (Use AllTweetsListWidget now)
					// 1 - GroupListWidget
					// 2 - ReplyListWidget
					// 3 - SearchListWidget
					// 4 - FriendsListWidget
					// 5 - DirectMessageListWidget
					// 6 - FavouriteListWidget
					// 7 - UserListWidget
					// 8 - FeedListWidget
					// 9 - TrendingTopicsListWidget
					switch(type) {
						case 1:
							w=(TweetListWidget*)new GroupListWidget(box,imagecache);
							break;
						case 2:
							w=(TweetListWidget*)new ReplyListWidget(box,imagecache);
							break;
						case 3:
							w=(TweetListWidget*)new SearchListWidget(box,imagecache);
							break;
						case 4:
							w=(TweetListWidget*)new FriendsListWidget(box,imagecache);
							break;
						case 5:
							w=(TweetListWidget*)new DirectMessageListWidget(box,imagecache);
							break;
						case 6:
							w=(TweetListWidget*)new FavoriteListWidget(box,imagecache);
							break;
						case 7:
							w=(TweetListWidget*)new UserListWidget(box,imagecache);
							break;
						case 8:
							w=(TweetListWidget*)new FeedListWidget(rssAccount,imagecache);
							break;
						case 9:
							w=(TweetListWidget*)new TrendingTopicsListWidget(box,imagecache);
							break;
						case 0:
						default:
							w=(TweetListWidget*)new AllTweetsListWidget(box,imagecache);
							break;
					}
					w->readSettings(settings);
					insertList(w,box);
				}
				settings.endArray();
				break;
		}
		tabs->setTabText(y,settings.value("name","New tab").toString());
	}
	settings.endArray();
	tabs->setCurrentIndex(settings.value("activeTab",0).toInt());
	tabs->handleTabCountChange();
	if (((YasstApplication*)qApp)->getAccounts().size()==0)
		dock->setVisible(false);
}

void Ya_st::handleTweet(BirdBox *box,QString message,QString reply_id,QString image_filename) {
	if (image_filename!="") {
		TwitrpixImageHandler *imgUpload=new TwitrpixImageHandler();
		box->addFetcher(imgUpload);
		imgUpload->tweet(message,reply_id,image_filename);
	} else {
		UserTweetFetcher *f = box->getUserFetcher();
		if (f)
			f->tweet(message,reply_id);
	}
}

void Ya_st::handleDeleteTweet(Tweet *t) {
	BirdBox *box = qobject_cast<BirdBox*>(sender());
	UserTweetFetcher *userFetcher = box->getUserFetcher();
	if (userFetcher && t)
		userFetcher->destroyTweet(t);
}

void Ya_st::handleTweetDirectMessage(BirdBox *box,QString message,QString reply_id,QString ) {
	UserDirectMessageFetcher *directMessageFetcher = box->getDirectMessageFetcher();
	if (directMessageFetcher)
		directMessageFetcher->tweetDirectMessage(message,reply_id);
}

void Ya_st::userUpdate(TweetUser *u) {
	if (u->image_url!="")
		imagecache->cacheImage(u->image_url);
}

void Ya_st::newGroup() {
	if (!activeAccount)
		return;
	GroupListWidget *w=new GroupListWidget(activeAccount,imagecache);
	insertList(w);
}

void Ya_st::newFriendsList() {
	if (!activeAccount)
		return;
	FriendsListWidget *w=new FriendsListWidget(activeAccount,imagecache);
	insertList(w);
}

void Ya_st::newFeedList() {
	FeedListWidget *w=new FeedListWidget(rssAccount,imagecache);
	insertList(w);
	w->requestURL();
}

void Ya_st::newTrendsList() {
	if (!activeAccount)
		return;
	TrendingTopicsListWidget *w=new TrendingTopicsListWidget(activeAccount,imagecache);
	insertList(w);
}

void Ya_st::newSearch() {
	if (!activeAccount)
		return;
	SearchListWidget *w=new SearchListWidget(activeAccount,imagecache);
	insertList(w);
}

void Ya_st::handleDoSearch(QString search) {
	if (!activeAccount)
		return;
	SearchListWidget *w=new SearchListWidget(activeAccount,imagecache);
	insertList(w);
	w->setSearch(search);
}

void Ya_st::showReplies() {
	if (!activeAccount)
		return;
	if (loggedOn) {
		insertList(new ReplyListWidget(activeAccount,imagecache));
	}
}

void Ya_st::showDirectMessages() {
	if (!activeAccount)
		return;
	if (loggedOn) {
		insertList(new DirectMessageListWidget(activeAccount,imagecache));
	}
}

void Ya_st::showFavourites() {
	if (!activeAccount)
		return;
	if (loggedOn) {
		insertList(new FavoriteListWidget(activeAccount,imagecache,userName));
	}
}

void Ya_st::showAll() {
	if (!activeAccount)
		return;
	insertList(new AllTweetsListWidget(activeAccount,imagecache));
}

void Ya_st::addFetcher(TweetFetcher *f) {
	activeAccount->addFetcher(f);
}

void Ya_st::handleshowUserDetails(TweetUser *user) {
    BirdBox *box = qobject_cast<BirdBox*>(sender());
    if (!box)
    	box=activeAccount;
	UserListWidget *w=new UserListWidget(box,imagecache);
	insertList(w);
	if (!user)
		return;
	w->setUser(user);
}

void Ya_st::handleshowUserDetails(QString u) {
    BirdBox *box = qobject_cast<BirdBox*>(sender());
    if (!box)
    	box=activeAccount;
	UserListWidget *w=new UserListWidget(box,imagecache);
	insertList(w);
	w->setUser(u);
}

void Ya_st::openFindPeopleTab() {
	if (!activeAccount)
		return;
	handleshowUserDetails(0);
//	tabs->addTab(new WebTabPanel(box,imagecache),"Find people");
}

void Ya_st::insertList(TweetListWidget* w) {
	insertList(w,activeAccount);
}

void Ya_st::insertList(TweetListWidget* w,BirdBox *activeAccount) {
	QSettings opts;
/*
	layout->addWidget(w);
	w->setLayout(layout);
*/
	tabs->addListWidget(w);
	QObject::connect(this->opts,SIGNAL(optionsSaved()),w,SLOT(refresh()));
//	QObject::connect(w,SIGNAL(showMe(TweetListWidget *)),this,SLOT(scrollToList(TweetListWidget*)));

//	QTimer::singleShot(10,this,SLOT(scrollToLast()));
	if (loggedOn) {
		// 0 - TweetListWidget
		// 1 - GroupListWidget
		// 2 - ReplyListWidget
		// 3 - SearchListWidget
		// 4 - FriendsListWidget
		// 5 - DirectMessageListWidget
		// 6 - FavoriteListWidget
		// 7 - UserListWidget
		// 8 - FeedListWidget
		// 9 - TrendingTopicsListWidget
		switch (w->type) {
		case 0 :
		case 1 :
		case 4 :
			if (activeAccount) {
				if (!activeAccount->getUserFetcher()) {
						UserTweetFetcher *f = new UserTweetFetcher();
						f->weight=opts.value("tweetFetcherWeight",75).toInt();
				//		userFetcher->doLogon(); // populates the authenticating user.
				//		userFetcher->populateUsers();
						f->setInitialFetchCount(opts.value("startupTweetFetchCount",200).toInt());
						activeAccount->addFetcher(f,true);
//						f->populate();
				}
			}
		case 2 :
			if (activeAccount) {
				if (!activeAccount->getFetcher(2)) {
					UserReplyFetcher *mentionFetcher=new UserReplyFetcher();
					mentionFetcher->weight=opts.value("mentionsFetcherWeight",50).toInt();
					activeAccount->addFetcher(mentionFetcher);
					mentionFetcher->populate();
				}
			}
			break;
		case 5 :
			if (activeAccount) {
				if (!activeAccount->getFetcher(3)) {
					UserDirectMessageFetcher *directMessageFetcher=new UserDirectMessageFetcher();
					directMessageFetcher->weight=opts.value("directMessagesFetcherWeight",50).toInt();
					addFetcher(directMessageFetcher);
					directMessageFetcher->populate();
				}
			}
			break;
		case 6 :
			if (activeAccount) {
				if (!activeAccount->getFetcher(4)) {
					UserFavoriteMessageFetcher *favoriteFetcher=new UserFavoriteMessageFetcher();
					favoriteFetcher->weight=opts.value("favouriteFetcherWeight",50).toInt();
					addFetcher(favoriteFetcher);
					favoriteFetcher->populate();
				}
			}
			break;
		}
	}
}

void Ya_st::scrollToLast() {
/*
	int width=0;
	for(int x=0;x<layout->count();x++) {
		width+=layout->itemAt(x)->geometry().width();
	}
	((QScrollArea*)centralWidget())->ensureVisible(width,0);
*/
}

void Ya_st::scrollToList(TweetListWidget *) {
/*
	((QScrollArea*)centralWidget())->ensureWidgetVisible(w);
	return;
*/
}

void Ya_st::setupActions() {
	newGroupAct = new QAction(tr("&List management"), this);
	newGroupAct->setShortcut(tr("Ctrl+G"));
//	newGroupAct->setStatusTip(tr("Click to create a new group, click and hold for a list of closed groups."));
	newGroupAct->setToolTip(tr("Click to manage lists.\nClick and hold to create a new group or open an existing list."));
	newGroupAct->setIcon(QIcon(":/buttons/group.png"));
    //connect(newGroupAct, SIGNAL(triggered()), this, SLOT(showGroupManagement()));
    friendsAct = new QAction(tr("Friends"), this);
    //friendsAct->setShortcut(tr("Ctrl+G"));
 //   friendsAct->setStatusTip(tr("Show all my friends tweets"));
 //   friendsAct->setToolTip(tr("Click to show all my friends tweets."));
	friendsAct->setIcon(QIcon(":/buttons/friends.png"));
	connect(friendsAct, SIGNAL(triggered()), this, SLOT(newFriendsList()));
    newSearchAct = new QAction(tr("&New search"), this);
    newSearchAct->setShortcut(tr("Ctrl+F"));
//    newSearchAct->setStatusTip(tr("Click to start a new search, click and hold for a list of previous searches."));
    newSearchAct->setToolTip(tr("Start a new search, click and hold for a list of previous searches."));
	newSearchAct->setIcon(QIcon(":/buttons/search.png"));
    connect(newSearchAct, SIGNAL(triggered()), this, SLOT(newSearch()));
    directMessageAct = new QAction(tr("Direct Messages"), this);
    directMessageAct->setShortcut(tr("Ctrl+D"));
//    directMessageAct->setStatusTip(tr("Show direct messages"));
    directMessageAct->setIcon(QIcon(":/buttons/direct_message.png"));
    connect(directMessageAct, SIGNAL(triggered()), this, SLOT(showDirectMessages()));
    favoritesAct = new QAction(tr("Favorites"), this);
//    favoritesAct->setShortcut(tr("Ctrl+D"));
//    favoritesAct->setStatusTip(tr("Show direct messages"));
    favoritesAct->setIcon(QIcon(":/buttons/favorite.png"));
    connect(favoritesAct, SIGNAL(triggered()), this, SLOT(showFavourites()));
    repliesAct = new QAction(tr("&Mentions"), this);
	repliesAct->setShortcut(tr("Ctrl+R"));
//	repliesAct->setStatusTip(tr("Show @replies"));
	repliesAct->setIcon(QIcon(":/buttons/reply.png"));
    connect(repliesAct, SIGNAL(triggered()), this, SLOT(showReplies()));
	allTweetsAct = new QAction(tr("A&ll"), this);
	allTweetsAct->setShortcut(tr("Ctrl+L"));
//	allTweetsAct->setStatusTip(tr("Show all tweets"));
	allTweetsAct->setIcon(QIcon(":/buttons/all_tweets.png"));
   connect(allTweetsAct, SIGNAL(triggered()), this, SLOT(showAll()));
    refreshAct = new QAction(tr("R&efresh"), this);
    refreshAct->setShortcut(tr("F5"));
//    refreshAct->setStatusTip(tr("Refresh tweets"));
    refreshAct->setIcon(QIcon(":/buttons/refresh.png"));
    connect(refreshAct, SIGNAL(triggered()), this, SLOT(refreshTweets()));
    optionsAct = new QAction(tr("&Options"), this);
    optionsAct->setShortcut(tr("Ctrl+O"));
//    optionsAct->setStatusTip(tr("Open the options window"));
    optionsAct->setIcon(QIcon(":/buttons/options.png"));
    connect(optionsAct, SIGNAL(triggered()), this, SLOT(showOpts()));
    newListTabAct = new QAction(tr("New &Tab"), this);
    newListTabAct->setShortcut(tr("Ctrl+T"));
//    newListTabAct->setStatusTip(tr("Open a new tab"));
    newListTabAct->setIcon(QIcon(":/buttons/newtab.png"));
    connect(newListTabAct, SIGNAL(triggered()), this, SLOT(newListTab()));
    closeActiveTabAct = new QAction(tr("Close Tab"), this);
    //closeActiveTabAct->setShortcut(tr("Ctrl+T"));
//    closeActiveTabAct->setStatusTip(tr("Close the current tab"));
    closeActiveTabAct->setIcon(QIcon(":/buttons/closetab.png"));
    connect(closeActiveTabAct, SIGNAL(triggered()), this, SLOT(closeActiveTab()));
    findPeopleAct = new QAction(tr("View user info"), this);
    //findPeopleAct->setShortcut(tr("Ctrl+T"));
//    findPeopleAct->setStatusTip(tr("Opens the find people tab"));
//    findPeopleAct->setToolTip(tr("Opens the find people tab"));
    findPeopleAct->setIcon(QIcon(":/buttons/user_search.png"));
    connect(findPeopleAct, SIGNAL(triggered()), this, SLOT(openFindPeopleTab()));
    feedAct = new QAction(tr("Subscribe to a web feed"), this);
    //findPeopleAct->setShortcut(tr("Ctrl+T"));
//    findPeopleAct->setStatusTip(tr("Opens the find people tab"));
//    findPeopleAct->setToolTip(tr("Opens the find people tab"));
    feedAct->setIcon(QIcon(":/buttons/feed.png"));
    connect(feedAct, SIGNAL(triggered()), this, SLOT(newFeedList()));
    trendsAct = new QAction(tr("Show trending topics."), this);
    //findPeopleAct->setShortcut(tr("Ctrl+T"));
//    findPeopleAct->setStatusTip(tr("Opens the find people tab"));
//    findPeopleAct->setToolTip(tr("Opens the find people tab"));
    trendsAct->setIcon(QIcon(":/buttons/trendingtopics.png"));
    connect(trendsAct, SIGNAL(triggered()), this, SLOT(newTrendsList()));
    quitAct = new QAction(tr("Quit"), this);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(handleQuit()));
    changeAcctAct = new QAction(tr("Change account"),this);
    changeAcctAct->setIcon(imagecache->getPixMap(""));
    changeAcctAct->setMenu(&accountsMenu);
    pruneReadTweets = new QAction(tr("Remove all read tweets."), this);
    //findPeopleAct->setShortcut(tr("Ctrl+T"));
//    findPeopleAct->setStatusTip(tr("Opens the find people tab"));
//    findPeopleAct->setToolTip(tr("Opens the find people tab"));
    pruneReadTweets->setIcon(QIcon(":/buttons/prune.png"));
    connect(pruneReadTweets, SIGNAL(triggered()), this, SLOT(handleExplicitPruneReadTweets()));
}

void Ya_st::showOpts() {
	opts->showOpts();
}

void Ya_st::showGroupManagement() {
	if (!activeAccount)
		return;
	groups->showGroupManagement(activeAccount);
}

void Ya_st::refreshTweets() {
	if (activeAccount) {
		activeAccount->refresh();
		activeAccount->getRate();
	}
}

void Ya_st::closeActiveTab() {
	if (tabs) {
		tabs->closeActiveTab();
	}
}

void Ya_st::setupToolBar() {
	QSize size;
	size.setWidth(16);
	size.setHeight(16);
	toolBar->setIconSize(size);
	toolBar->addAction(refreshAct);
	//toolBar->addAction(pruneReadTweets);
	toolBar->addAction(optionsAct);
	toolBar->addAction(newListTabAct);
#ifdef Q_WS_MAC
	toolBar->addAction(closeActiveTabAct);
#endif
	toolBar->addSeparator();
	toolBar->addAction(allTweetsAct);
	toolBar->addAction(friendsAct);
	toolBar->addAction(repliesAct);
	toolBar->addAction(directMessageAct);
	toolBar->addAction(favoritesAct);
	toolBar->addAction(feedAct);

	toolBar->addAction(newGroupAct);
	toolBar->addAction(newSearchAct);
	toolBar->addAction(findPeopleAct);
	toolBar->addAction(trendsAct);

	toolBar->addSeparator();
	toolBar->addAction(changeAcctAct);
	((QToolButton*)toolBar->widgetForAction(changeAcctAct))->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	((QToolButton*)toolBar->widgetForAction(changeAcctAct))->setPopupMode(QToolButton::InstantPopup);
	((QToolButton*)toolBar->widgetForAction(newGroupAct))->setPopupMode(QToolButton::InstantPopup);
}

void Ya_st::setupMenu() {
	QMenu *menu;
	menu=menuBar()->addMenu(tr("Tweets"));
	menu->addAction(allTweetsAct);
	menu->addAction(friendsAct);
	menu->addAction(repliesAct);
	menu->addAction(directMessageAct);
	menu->addSeparator();
	menu->addAction(newGroupAct);
	menu->addAction(newSearchAct);
	menu->addAction(findPeopleAct);
	menu->addAction(feedAct);
	menu->addAction(trendsAct);
	menu=menuBar()->addMenu(tr("Window"));
	menu->addAction(newListTabAct);
	menu->addAction(closeActiveTabAct);
	menu->addSeparator();
	menu->addAction(optionsAct);
}

void Ya_st::writeSettings() {
	QSettings settings;
	settings.setValue("version",QVariant(getVersion()));
	settings.beginWriteArray("favlang");
	for (int x=0;x<TweetWidget::lastLang.size();x++) {
		settings.setArrayIndex(x);
		settings.setValue("lang",TweetWidget::lastLang.at(x));
	}
	settings.endArray();
    settings.setValue("mainwindow/size", size());
    settings.setValue("mainwindow/pos", pos());
    settings.setValue("mainwindow/state", saveState(0));
    settings.setValue("mainwindow/geometry", saveGeometry());
//    if (userFetcher)
//   	settings.setValue("lastFriendTweetId",userFetcher->getLastId());
    if (dock)
    	settings.setValue("mainwindow/tweetpanelvisible",dock->isVisible());
    if (loggedOn) {
    	tabs->writeSettings(settings);

    	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
    	settings.beginWriteArray("accounts/twitter");
    	int y=0;
    	for (int x=0;x<accts.length();x++) {
    		BirdBox *b = accts.at(x);

    		if (b->hasCredentials()) {
    			settings.setArrayIndex(y++);
    			settings.setValue("username",b->getUser().toLatin1().toBase64());
    			QString pass="";
    			if (b->getRememberMe())
    				pass = b->getPassword();
    			settings.setValue("password",pass.toLatin1().toBase64());
    			settings.setValue("id",b->getId());
    			settings.setValue("token",b->getOAuth()->getAuthToken());
    			settings.setValue("tokenSecret",b->getOAuth()->getAuthTokenSecret());
    		}
    	}
    	settings.endArray();
/*
    	QList<Group*> groups = box->getGroups();
    	settings.remove("groups");
    	settings.beginWriteArray("groups");
    	int y=0;
    	for(int x=0;x<groups.count();x++) {
    		Group *g=groups.at(x);

    		if (!(g->twitterList)) {
        		settings.setArrayIndex(y++);
    			settings.setValue("name",g->name);
    			settings.setValue("local_id",g->local_id);
    			settings.beginWriteArray("members");
    			for(int z=0;z<g->groupmembers.count();z++) {
    				settings.setArrayIndex(z);
    				settings.setValue("user_id",g->groupmembers.at(z));
    			}
    			settings.endArray();
    		}
    	}
		settings.endArray();
		settings.remove("tabs");

		settings.beginWriteArray("tabs");
		settings.setArrayIndex(0);
		settings.setValue("type",0);// 0 - list o tweets
		settings.beginWriteArray("columns");
		for(int x=0;x<layout->count();x++) {
			settings.setArrayIndex(x);
			((TweetListWidget*)layout->itemAt(x)->widget())->writeSettings(settings);
		}
		settings.endArray();
		settings.endArray();
*/
    }
}

void Ya_st::readSettings() {
	checkSettings();
	QSettings settings;
	int langCount = settings.beginReadArray("favlang");
	TweetWidget::lastLang.clear();
	for (int x=0;x<langCount;x++) {
		settings.setArrayIndex(x);
		TweetWidget::lastLang.append(settings.value("lang").toString());
	}
	settings.endArray();

    if (settings.contains("mainwindow/size")) {
    	resize(settings.value("mainwindow/size", QSize(400, 400)).toSize());
    	move(settings.value("mainwindow/pos", QPoint(200, 200)).toPoint());
    	restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    	restoreState(settings.value("mainwindow/state").toByteArray(),0);
    }
/* -- Doesn't work well with quickly restarting
    if (userFetcher && settings.contains("lastFriendTweetId"))
    	userFetcher->setLastId(settings.value("lastFriendTweetId").toString());
*/
}

void Ya_st::checkSettings() {
	QSettings settings;
	int settingsVer = settings.value("version",QVariant(20100112)).toInt();

	if (settingsVer<=20100112) {
		QString user = settings.value("accounts/twitter/user","").toString();
		if (user!="") {
			QString pass = QString(QByteArray::fromBase64(settings.value("accounts/twitter/pass","").toByteArray()));
			QString newId = QUuid::createUuid().toString();
			settings.beginWriteArray("accounts/twitter");
			settings.setArrayIndex(0);
			settings.setValue("username",user.toLatin1().toBase64());
			settings.setValue("password",pass.toLatin1().toBase64());
			settings.setValue("id",newId);
			settings.endArray();
			int tabcount=settings.beginReadArray("tabs");
			while(tabcount) {
				settings.setArrayIndex(--tabcount);
				int type=settings.value("type").toInt();
				if (type==0) {
					int count=settings.beginReadArray("columns");
					while(count) {
						settings.setArrayIndex(--count);
						settings.setValue("account",newId);
					}
					settings.endArray();
				}
			}
			settings.endArray();
		}
	}
}

TabPanel* Ya_st::newListTab() {
	TweetListTabPanel *p=tabs->addListView();
	QObject::connect(p,SIGNAL(tweetsAdded()),this,SLOT(handleChildUpdate()));
	QObject::connect(p,SIGNAL(popout(TweetListWidget*)),this,SLOT(handlePopout(TweetListWidget*)));
	return p;
/*
	QWidget *c = new QWidget(this);
	QScrollArea *area = new QScrollArea();
	tabs->addTab(area,tr("Tweets"));

	layout=new QHBoxLayout();
	layout->setSizeConstraint(QLayout::SetMaximumSize);
	layout->setAlignment(Qt::AlignLeft);
	area->setWidget(c);
	area->setWidgetResizable(true);
	c->setLayout(layout);
	c->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
*/
}

void Ya_st::handleStartFetching() {
	active++;
	updateStatus();
}

void Ya_st::handleEndFetching() {
	active--;
	if (active<0)
		active=0;
	updateStatus();
	doNotifications();
}

void Ya_st::handleFollowUserByScreenName(QString id) {
    BirdBox *box = qobject_cast<BirdBox*>(sender());
	UserTweetFetcher *userFetcher = box->getUserFetcher();
	if (userFetcher) {
		TweetUser *u=box->getUserByScreenName(id);
		if (u) {
			u->am_unfollowing=false;
		}
		userFetcher->followUserByScreenName(id);
	}
}

void Ya_st::handleUnfollowUserByScreenName(QString id) {
    BirdBox *box = qobject_cast<BirdBox*>(sender());
	UserTweetFetcher *userFetcher = box->getUserFetcher();
	if (userFetcher) {
		TweetUser *u=box->getUserByScreenName(id);
		if (u) {
			u->am_unfollowing=true;
			u->is_friend=false;
		}
		userFetcher->unfollowUserByScreenName(id);
	}
}

void Ya_st::handleGroupListClosed(QString name ,QList<QString> members) {
	QAction *act = groupMenu.addAction(name);
	act->setData(QVariant(members));
	QObject::connect(act,SIGNAL(triggered()),this,SLOT(restoreGroup()));
	closedGroupCount++;
	if (closedGroupCount>=11) {
		groupMenu.removeAction(groupMenu.actions().at(0));
		closedGroupCount--;
	}
}

void Ya_st::restoreGroup() {
    QAction *act = qobject_cast<QAction *>(sender());
    QList<QVariant> members;
    QList<QString> newMembers;
    if (act) {
    	groupMenu.removeAction(act);
    	QObject::disconnect(act,SIGNAL(triggered()),this,SLOT(restoreGroup()));
    	members=act->data().toList();
    	for(int x=0;x<members.count();x++) {
    		newMembers.append(members.at(x).toString());
//    		QMessageBox::information (0,"assdf",members.at(x).toString());
    	}
    	GroupListWidget *w=new GroupListWidget(activeAccount,imagecache);
    	w->setName(act->text());
    	w->setMembers(newMembers);
    	w->setTweetsVisible();
    	insertList(w);
    	act->deleteLater();
    	closedGroupCount--;
    }
}

void Ya_st::handleSearchClosed(QString name ,QString term) {
	if (!searchMenu) {
		searchMenu=new QMenu(this);
		newSearchAct->setMenu(searchMenu);
	}
	QAction *act = searchMenu->addAction(name);
	act->setData(QVariant(term));
	QObject::connect(act,SIGNAL(triggered()),this,SLOT(restoreSearch()));
	closedSearchCount++;
	if (closedSearchCount>=11) {
		searchMenu->removeAction(searchMenu->actions().at(0));
		closedSearchCount--;
	}
}

void Ya_st::restoreSearch() {
    QAction *act = qobject_cast<QAction *>(sender());
    QList<QVariant> members;
    QList<QString> newMembers;
    if (act) {
    	searchMenu->removeAction(act);
    	QObject::disconnect(act,SIGNAL(triggered()),this,SLOT(restoreSearch()));
    	SearchListWidget *w=new SearchListWidget(activeAccount,imagecache);
    	w->setSearch(act->data().toString(),act->text());
    	insertList(w);
    	act->deleteLater();
    	closedSearchCount--;
    }
   if (!closedSearchCount) {
	   newSearchAct->setMenu(0);
	   searchMenu->deleteLater();
	   searchMenu=0;
   }
}

void Ya_st::handleStartImageUpload() {
	uploadingImageCount++;
	updateStatus();
}

void Ya_st::handleStopImageUpload() {
	uploadingImageCount--;
	if (uploadingImageCount<0)
		uploadingImageCount=0;
	updateStatus();
}

void Ya_st::setStyle(int s) {
    QString styles[]= {":/style/main.css",":/style/purple.css",":/style/dark.css"};
    QString textStyles[]= {":/style/main-text.css",":/style/purple-text.css",":/style/dark-text.css"};
    int styleCount = 3;

	if (s>=0 && s<styleCount) {
		setStyle(styles[s],textStyles[s]);
	} else if (s==styleCount) { // No style
		setStyleSheet("");
		Ya_st::labelCSS="";
	} else if (s==styleCount+1) {
		QSettings opts;
		setStyle(opts.value("userMainStylesheet","").toString(),opts.value("userTextStylesheet","").toString());
	}
}

void Ya_st::setStyle(QString main,QString text) {
	if (main=="" || text=="")
		return;
	// Set the stylesheeeet
	QFile css(main);
	QFile extra;
#ifdef Q_WS_X11
	extra.setFileName(":/style/x11.css");
#endif
#ifdef Q_WS_MAC
	extra.setFileName(":/style/osx.css");
#endif
	css.open(QIODevice::ReadOnly);
	//QMessageBox::information (0,"assdf",QString(css.readAll()));
	QString style = QString(css.readAll());
	if (extra.open(QIODevice::ReadOnly)) {
		style += QString(extra.readAll());
		extra.close();
	}
	setStyleSheet(style);
	Ya_st::mainCSS=style;
	css.close();
	css.setFileName(text);
	css.open(QIODevice::ReadOnly);
	//QMessageBox::information (0,"assdf",QString(cssFile.readAll()));
	Ya_st::labelCSS=QString(css.readAll());
	css.close();
}

void Ya_st::handleTrayIconActivated ( QSystemTrayIcon::ActivationReason reason ) {
	if (reason==QSystemTrayIcon::DoubleClick||reason==QSystemTrayIcon::Trigger) {
		if (!poppedOut)
			show();
	}
}

void Ya_st::changeEvent ( QEvent * e) {

	if (e->type()==QEvent::WindowStateChange||e->type()==QEvent::ActivationChange){
		if (isActiveWindow()&&tabs)
			tabs->setActive();
	}
	if (e->type()==QEvent::ActivationChange) {
//		QMessageBox::information (0,"assdf","Hello1!");
		if (!isActiveWindow()) {
			//QMessageBox::information (0,"assdf","Hello!");
			//close();

		}
	}
}

void Ya_st::closeEvent(QCloseEvent *event) {
	if (trayIcon.isVisible()) {
		hide();
		event->ignore();
	} else
		handleQuit();
}

void Ya_st::handleQuit() {
	writeSettings();
	qApp->quit();
}

void Ya_st::handleFavoriteTweet(Tweet *t) {
    BirdBox *box = qobject_cast<BirdBox*>(sender());
	UserTweetFetcher *userFetcher = box->getUserFetcher();
	if (userFetcher)
		((UserTweetFetcher*)userFetcher)->favoriteTweet(t);
}

void Ya_st::handleUnfavoriteTweet(Tweet *t) {
    BirdBox *box = qobject_cast<BirdBox*>(sender());
	UserTweetFetcher *userFetcher = box->getUserFetcher();
	if (userFetcher)
		((UserTweetFetcher*)userFetcher)->unfavoriteTweet(t);
}

void Ya_st::notify(QString text) {
	if (!notifyArea)
		return;
	if (notifyText)
		notifyText->setText(text);
	if (notifyArea->isHidden()) {
		notifyOpening=true;
		notifyArea->setMaximumHeight(0);
		notifyArea->show();
		notifyOpenTimer.start();
	} else
		notifyTimer.start();
}

void Ya_st::handleNotifyTimer() {
	notifyOpening=false;
	notifyOpenTimer.start();
	notifyTimer.stop();
}

void Ya_st::handleNotifyOpenTimer() {
	int h = notifyArea->height();
	if (notifyOpening) {
		if ((h==0 || h>targetSize)) {
			targetSize=h;
			h+=2;
			notifyArea->setMaximumHeight(h);
//			notifyArea->setMinimumHeight(h);
		} else {
			notifyOpenTimer.stop();
			notifyTimer.start();
			notifyArea->setMaximumHeight(10000);
		}
	} else {
		if (h>0) {
			h-=2;
			notifyArea->setMaximumHeight((h>=0)?h:0);
//			notifyArea->setMinimumHeight((h>=0)?h:0);
		} else {
			notifyOpenTimer.stop();
			notifyTimer.stop();
			notifyArea->hide();
		}
	}
}

void Ya_st::handlePopout(TweetListWidget *w) {
	QWidget *p = w->parentWidget();
	QSettings settings;
	if (p) {
		QLayout *l = p->layout();
		if (l) {
			if (popout)
				popout->deleteLater();
			popoutLayout = l;
			popoutPos = l->indexOf(w);
			popoutList = w;
			//QMessageBox::information(this,"","Hello:"+QString::number(pos));
			popout = new PopoutWidget(0);
			QWidget *central = new QWidget();
			popout->setCentralWidget(central);
			popout->setAttribute(Qt::WA_DeleteOnClose,false);
			popout->setStyleSheet(styleSheet());
			l = new QHBoxLayout();
			central->setLayout(l);
//			popout->setBaseSize(popout->width(),w->height());

			popout->setWindowTitle(w->title());
			popout->addDockWidget(Qt::BottomDockWidgetArea,dock);
			l->addWidget(w);
			l->setMargin(0);
			w->setPopoutMode(true);
			w->show();

			QObject::connect(popout,SIGNAL(amClosing()),this,SLOT(handlePopoutClosed()));
			QObject::connect(w,SIGNAL(titleChanged(QString)),popout,SLOT(setWindowTitle(QString)));

		    if (settings.contains("popout/size")) {
		    	popout->resize(settings.value("popout/size", QSize(250, 400)).toSize());
		    	popout->move(settings.value("popout/pos", QPoint(200, 200)).toPoint());
		    	popout->restoreGeometry(settings.value("popout/geometry").toByteArray());
		    	popout->restoreState(settings.value("popout/state").toByteArray(),0);
		    }

			popout->show();
			this->hide();
			poppedOut=true;
		}
	}
}

void Ya_st::handlePopoutClosed() {
	QSettings settings;
    settings.setValue("popout/size", popout->size());
    settings.setValue("popout/pos", popout->pos());
    settings.setValue("popout/state", popout->saveState(0));
    settings.setValue("popout/geometry", popout->saveGeometry());
	((TweetListWidget*)popoutList)->setPopoutMode(false);
	addDockWidget(Qt::BottomDockWidgetArea,dock);
	((QBoxLayout*)popoutLayout)->insertWidget(popoutPos,popoutList);
	popout->deleteLater();
	popout=0;
	popoutLayout=0;
	popoutList=0;
	this->show();
	poppedOut=false;
}

void Ya_st::handleRetweet(Tweet *t,bool newStyle) {
	if (!newStyle)
		return;
    BirdBox *box = qobject_cast<BirdBox*>(sender());
	UserTweetFetcher *userFetcher = box->getUserFetcher();
	if (userFetcher) {
		((UserTweetFetcher*)userFetcher)->retweet(t->id);
	}
}

void Ya_st::handleShowGroupMenu() {
	int count=0;
	int fcount=0;
	QList<Group*> groups = activeAccount->getGroups();

	listMenu.clear();
	followingListMenu.clear();
	for(int x=0;x<groups.size();x++) {
		if (groups.at(x)) {
			Group *g = groups.at(x);
			if (g->twitterList) {
				if (g->user==g->ownerScreenName) {
					QAction *act = listMenu.addAction(g->name);
					act->setData(x);
					act->setIcon(QIcon(":/buttons/twitter.png"));
					count++;
				} else {
					QAction *act = followingListMenu.addAction(g->name);
					act->setData(x);
					act->setIcon(QIcon(":/buttons/twitter-dark.png"));
					fcount++;
				}
			}
		}
	}
	twitterListAct->setDisabled((count==0));
	twitterFollowListAct->setDisabled((fcount==0));
	closedGroupAct->setDisabled((closedGroupCount==0));
}

void Ya_st::handleListMenuTriggered(QAction *act) {
	QList<Group*> groups = activeAccount->getGroups();
	int x = act->data().toInt();

	if (groups.at(x)) {
    	GroupListWidget *w=new GroupListWidget(activeAccount,imagecache);
    	w->setGroup(groups.at(x));
    	w->setTweetsVisible();
    	insertList(w);
	}
}

void Ya_st::handleShowAccountsMenu() {
	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();

	accountsMenu.clear();
	for(int x=0;x<accts.size();x++) {
		BirdBox *b=accts.at(x);
		QAction *act = accountsMenu.addAction(b->getAuthenticatingUserScreenName());
		TweetUser *u = b->getUserByScreenName(b->getAuthenticatingUserScreenName());
		QString url="";
		if (u) {
			url=u->image_url;
		}
		act->setIcon(imagecache->getPixMap(url));
		act->setData(x);
		if (activeAccount && activeAccount->getAuthenticatingUserScreenName()==b->getAuthenticatingUserScreenName()) {
			act->setCheckable(true);
			act->setChecked(true);
		}
	}
}

void Ya_st::handleAccountsMenuTriggered(QAction *act) {
	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
	int x = act->data().toInt();

	if (x<accts.count()) {
		handleAccountChanged(accts.at(x));
	}
}

void Ya_st::addNewAccount(BirdBox *box) {
	YasstApplication *app = (YasstApplication*)qApp;

	if (app->getAccounts().contains(box))
		return;

	QObject::connect(box,SIGNAL(haveUser(TweetUser *)),this,SLOT(userUpdate(TweetUser *)));
	QObject::connect(box,SIGNAL(authError(HTTPHandler*)),this,SLOT(authError(HTTPHandler*)));
	QObject::connect(box,SIGNAL(otherError(HTTPHandler*)),this,SLOT(otherError(HTTPHandler*)));
	QObject::connect(box,SIGNAL(statusChange(int,int,int)),this,SLOT(statusChange(int,int,int)));
	QObject::connect(box,SIGNAL(rateUpdate(long,long)),this,SLOT(rateUpdate(long,long)));
	QObject::connect(box,SIGNAL(doSearch(QString)),this,SLOT(handleDoSearch(QString)));
	QObject::connect(box,SIGNAL(followUserByScreenName(QString)),this,SLOT(handleFollowUserByScreenName(QString)));
	QObject::connect(box,SIGNAL(unfollowUserByScreenName(QString)),this,SLOT(handleUnfollowUserByScreenName(QString)));
	QObject::connect(box,SIGNAL(handleDeleteTweet(Tweet *)),this,SLOT(handleDeleteTweet(Tweet *)));
	QObject::connect(box,SIGNAL(retweet(Tweet *,bool)),this,SLOT(handleRetweet(Tweet *,bool)));
	QObject::connect(box,SIGNAL(setFavorite(Tweet *)),this,SLOT(handleFavoriteTweet(Tweet *)));
	QObject::connect(box,SIGNAL(clearFavorite(Tweet *)),this,SLOT(handleUnfavoriteTweet(Tweet *)));
	QObject::connect(box,SIGNAL(startFetching()),this,SLOT(handleStartFetching()));
	QObject::connect(box,SIGNAL(endFetching()),this,SLOT(handleEndFetching()));
	QObject::connect(box,SIGNAL(imageUploadStarted()),this,SLOT(handleStartImageUpload()));
	QObject::connect(box,SIGNAL(imageUploadEnded()),this,SLOT(handleStopImageUpload()));
	QObject::connect(box,SIGNAL(showUserDetails(TweetUser *)),this,SLOT(handleshowUserDetails(TweetUser *)));
	QObject::connect(box,SIGNAL(showUserDetails(QString)),this,SLOT(handleshowUserDetails(QString)));
	app->addAccount(box);
	if (!activeAccount) {
		handleAccountChanged(box);
//		refreshTweets();
	}
}

void Ya_st::handleAccountChanged(BirdBox *b) {
	activeAccount=b;
	emit accountChanged(b);
	if (!changeAcctAct)
		return;
	changeAcctAct->setText(b->getAuthenticatingUserScreenName());
	TweetUser *u = b->getUserByScreenName(b->getAuthenticatingUserScreenName());
	if (u) {
		changeAcctAct->setIcon(imagecache->getPixMap(u->image_url));
	}
	changeAcctAct->setData(QVariant(b->getAuthenticatingUserScreenName()));
}

void Ya_st::updateImage(QString url) {
	if (!changeAcctAct || !activeAccount)
		return;
	TweetUser *u = activeAccount->getUserByScreenName(activeAccount->getAuthenticatingUserScreenName());

	if (u && u->image_url==url) {
		changeAcctAct->setIcon(imagecache->getPixMap(url));
	}
}

void Ya_st::handleAccountRemoved(BirdBox *b) {
	if (activeAccount==b) {
		QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
		if (accts.size()>0)
			handleAccountChanged(accts.at(0));
	}
}

void Ya_st::handleExplicitPruneReadTweets() {
	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
	for (int x=0;x<accts.size();x++)
		accts.at(x)->pruneReadTweets();
}

void Ya_st::handlePINRequest(OAuth *auth,QString url) {
	TwitterOAuthPinWidget *widget = new TwitterOAuthPinWidget(this);
	widget->setURL(url);
	widget->setUser(auth->getUser());
	widget->setFocus();
//	QGraphicsEffect *effect = new QGraphicsBlurEffect(this);
//	this->topLevelWidget()->setGraphicsEffect(effect);
	if (widget->exec()==QDialog::Accepted) {
		QString pin = widget->getPIN();

		auth->setPIN(pin);
	} else
		auth->reset();
//	this->topLevelWidget()->setGraphicsEffect(0);
	widget->deleteLater();
}

void Ya_st::doNotifications(bool alwaysShow) {
	QSettings opts;

	uint now = QDateTime::currentDateTime().toTime_t();
	if (tabs
		&& opts.value("notifications",QVariant(true)).toBool()
		&& ((now-lastNotificationTime)>120 || (alwaysShow && (now-lastNotificationTime)>15))
		&& (
				(
				opts.value("notificationswhennotvisible",QVariant(true)).toBool()
				&&
					(
					!isVisible()
					|| isMinimized()
					)
				)
			|| (
				!opts.value("notificationswhennotvisible",QVariant(true)).toBool()
				)
			)
		) {
		NotificationWidget *w = 0;
		for (int x=0;x<tabs->count();x++) {
			TabPanel *p = ((TabPanel*)tabs->widget(x));
			if (p->getType()==0) {
				TweetListTabPanel *tp = (TweetListTabPanel*)p;
				for(int y=0;y<tp->count();y++) {
					TweetListWidget *tl = tp->widgetAt(y);
					if (opts.value("notificationsonunmutedonly",QVariant(true)).toBool() && tl->isMuted())
						continue;
					int unread = tl->getUnreadWidgetCount();
					if (unread>0) {
						if (!w)
							w = new NotificationWidget();
						w->addMessage(tl->getName()+" has "+QString::number(unread)+" unread tweets.",tl->getIcon());
						lastNotificationTime=now;
					}
				}
			}
		}
		if (w)
			w->showNotification();
	}
}

void Ya_st::tweetAndQuit(QString tweet,QString tweetAs) {
	doTweet=tweet;
	doTweetAs=tweetAs;
}

void Ya_st::doTweetAndQuit(QString tweet,QString tweetAs) {
	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
	for (int x=0;x<accts.count();x++) {
		BirdBox *b = accts.at(x);
		if (b->getAuthenticatingUserScreenName()==tweetAs) {
			TweetFetcher *f = b->getUserFetcher();
			if (f)
				f->tweet(tweet);
			QObject::connect(f,SIGNAL(endFetching()),qApp,SLOT(quit()));
			break;
		}
	}
	//QTimer::singleShot(0,qApp,SLOT(quit()));
}
