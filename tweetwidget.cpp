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
/* $Id: tweetwidget.cpp,v 1.59 2010/10/05 18:08:51 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#include "tweetwidget.h"
#include "ya_st.h"
#include "consts.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QSettings>
#include <QPainter>
#include <QFile>
#include <QPixmap>
#include <QStackedLayout>
#include <QVariant>
#include <QLabel>
#include "backend/fetchers/TwitterUserTweetsFetcher.h"
#include "backend/FeedItem.h"
#include "GroupListWidget.h"
#include <QToolTip>

QList<QString> TweetWidget::lastLang;

TweetWidget::TweetWidget(QWidget *parent)
    : AbstractTweetWidget(parent)
{
	ui.setupUi(this);
	setAcceptDrops(true);
	tweet=0;
}


TweetWidget::TweetWidget(Tweet *t, BirdBox *b, ImageCache *c, QWidget *parent,TweetListWidget *myList)
    : AbstractTweetWidget(parent)
{
#ifdef Q_WS_MAC
	QString textType="mac";
#else
	QString textType="true";
#endif
	QSettings opts;
	tweet=0;
	setTweet(t);
	imgcache=c;
	tweets=b;
	ui.setupUi(this);
	if (!imgcache) {
	//	imgcache=new ImageCache();
	}

	if (myList)
		list=myList;
	else
		list=0;

	timer=new QTimer(this);
	showTimer=new QTimer(this);
	hideTimer=new QTimer(this);
//	notNewAnyMoreTimer=new QTimer(this);

//	notNewAnyMoreTimer->setSingleShot(true);
//	notNewAnyMoreTimer->setInterval(5000);

	QObject::connect(this,SIGNAL(reply(Tweet*)),b,SIGNAL(reply(Tweet*)));
	QObject::connect(this,SIGNAL(retweet(Tweet*,bool)),b,SIGNAL(retweet(Tweet*,bool)));
	QObject::connect(this,SIGNAL(directMessage(Tweet*)),b,SIGNAL(directMessage(Tweet*)));
	QObject::connect(this,SIGNAL(deleteTweet(Tweet*)),b,SIGNAL(handleDeleteTweet(Tweet*)));
	QObject::connect(this,SIGNAL(setFavorite(Tweet*)),b,SIGNAL(setFavorite(Tweet*)));
	QObject::connect(this,SIGNAL(clearFavorite(Tweet*)),b,SIGNAL(clearFavorite(Tweet*)));
	QObject::connect(this,SIGNAL(doSearch(QString)),b,SIGNAL(doSearch(QString)));
	QObject::connect(this,SIGNAL(followUserByScreenName(QString)),b,SIGNAL(followUserByScreenName(QString)));
	QObject::connect(this,SIGNAL(unfollowUserByScreenName(QString)),tweets,SIGNAL(unfollowUserByScreenName(QString)));
	QObject::connect(this,SIGNAL(showUserDetails(TweetUser *)),tweets,SIGNAL(showUserDetails(TweetUser *)));
	QObject::connect(this,SIGNAL(showUserDetails(QString)),tweets,SIGNAL(showUserDetails(QString)));
	QObject::connect(imgcache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)),Qt::QueuedConnection);
	QObject::connect(b,SIGNAL(haveUpdatedUser(TweetUser*)),this,SLOT(handleUpdatedUser(TweetUser*)),Qt::QueuedConnection);
	QObject::connect(ui.reply,SIGNAL(clicked()),this,SLOT(handleReplyClick()));
	QObject::connect(ui.retweet,SIGNAL(clicked()),this,SLOT(handleRetweetClick()));
	QObject::connect(ui.directMessage,SIGNAL(clicked()),this,SLOT(handleDirectMessageClick()));
	QObject::connect(ui.trash,SIGNAL(clicked()),this,SLOT(setDeleteMode()));
	QObject::connect(ui.follow,SIGNAL(clicked()),this,SLOT(handleFollowUser()));
	QObject::connect(ui.unfollow,SIGNAL(clicked()),this,SLOT(handleUnfollowUser()));
	QObject::connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(buttonClicked()));
	QObject::connect(ui.fav,SIGNAL(doubleclicked()),this,SLOT(handleFavorite()));
	QObject::connect(ui.text,SIGNAL(linkActivated(QString)),this,SLOT(haveURL(QString)));
	QObject::connect(ui.text,SIGNAL(linkHovered(QString)),this,SLOT(handleLinkHovered(QString)));
	QObject::connect(ui.info,SIGNAL(linkActivated(QString)),this,SLOT(haveURL(QString)));
	QObject::connect(timer,SIGNAL(timeout()),this,SLOT(tick()));
	QObject::connect(showTimer,SIGNAL(timeout()),this,SLOT(showControls()));
	QObject::connect(hideTimer,SIGNAL(timeout()),this,SLOT(hideControls()));
	QObject::connect(&hoverTimer,SIGNAL(timeout()),this,SLOT(handleLinkHoverTimer()));

	hoverTimer.setSingleShot(true);
	//QObject::connect(notNewAnyMoreTimer,SIGNAL(timeout()),this,SLOT(removeNewIndicator()));

	timeOut = 250;
	ui.progressBar->setMinimum(0);
	ui.progressBar->setMaximum(timeOut);
	ui.text->setProperty("tweettext",textType);
	ui.date->setProperty("tweettext",textType);
	ui.info->setProperty("tweettext",textType);

	setFontSize(opts.value("fonts/tweettextsize",DEFAULT_FONT_SIZE).toInt());

	setObjectName("TweetWidget");

	//ui.text->setObjectName("tweet");

	if (t->direct_message) {
		ui.text->setProperty("tweet", "direct");
		setDirectMessageListMode();
	} else {
		ui.text->setProperty("tweet", "normal");
		if (t->in_reply_to_screen_name!="")
			ui.text->setProperty("mention", "true");
		setListMode();
	}

	haveClickedInReplyTo=false;
	widgetState=TweetWidget::stateNormal;
	showTimer->setInterval(50);
	hideTimer->setInterval(50);

	listMode=true;
	reverseReply=false;
	isDirectMessage=false;

	QWidget *w=new QWidget(this);
	ui.toplayout->insertWidget(0,w);
	w->setLayout(&userImageLayout);
	w->setFixedSize(40,40);
	userControls=new QWidget(this);
	QVBoxLayout *blayout = new QVBoxLayout();
	userControls->setLayout(blayout);
	userControls->setFixedWidth(15);
	blayout->addWidget(&refreshUser);
	blayout->addStretch(1);
	blayout->addWidget(&userInfo);
	blayout->setMargin(0);
	blayout->setSpacing(0);
	QHBoxLayout *hlayout = new QHBoxLayout();
	retweetIndicator = new QWidget(this);
	retweetIndicator->setLayout(hlayout);
	retweetIndicator->setFixedWidth(40);
	retweetIndicator->setFixedHeight(16);
	retweetIndicator->setStyleSheet("background:none; border:none;");
	retweetIndicator->hide();
	retweetIndicatorButton = new QPushButton(this);
	retweetIndicatorButton->setFixedSize(16,16);
	retweetIndicatorButton->setIcon(QIcon(":/buttons/retweet.png"));
	hlayout->addStretch(1);
	hlayout->addWidget(retweetIndicatorButton);
	hlayout->setMargin(0);
	hlayout->setSpacing(0);
	//w->hide();
	userImageLayout.setStackingMode(QStackedLayout::StackAll);
	userImageLayout.insertWidget(0,userControls);
	userImageLayout.insertWidget(1,retweetIndicator);
	userImageLayout.insertWidget(2,&image);
	refreshUser.setIcon(QIcon(":/buttons/refresh-overlay.png"));
	refreshUser.setFixedSize(15,15);
	refreshUser.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	refreshUser.setStyleSheet("background:none; border:none;");
	userInfo.setIcon(QIcon(":/buttons/info.png"));
	userInfo.setFixedSize(15,15);
	userInfo.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	userInfo.setStyleSheet("background:none; border:none;");
	image.setFixedSize(40,40);
	image.setScaledContents(true);

	if (tweet->type==0) {
		groupMenu.setTitle(tr("Add to group"));
		userMenu.addMenu(&groupMenu);
		if (list && list->type==1)
			QObject::connect(userMenu.addAction(tr("Remove from this group")),SIGNAL(triggered()),this,SLOT(handleRemoveFromThisGroup()));
		QObject::connect(userMenu.addAction(tr("Mention")),SIGNAL(triggered()),this,SLOT(handleReplyClick()));
		QObject::connect(userMenu.addAction(tr("Retweet")),SIGNAL(triggered()),this,SLOT(handleRetweetClick()));
		QObject::connect(userMenu.addAction(tr("DM user")),SIGNAL(triggered()),this,SLOT(handleDirectMessageClick()));
//		QObject::connect(userMenu.addAction(tr("Retweet")),SIGNAL(clicked()),this,SLOT(setDeleteMode()));
		QObject::connect(userMenu.addAction(tr("Follow")),SIGNAL(triggered()),this,SLOT(handleFollowUser()));
		QObject::connect(userMenu.addAction(tr("Unfollow")),SIGNAL(triggered()),this,SLOT(handleUnfollowUser()));
		userMenu.addSeparator();
		translateToMenu.setTitle(tr("Translate to"));
		translateFromMenu.setTitle(tr("Translate from"));
		userMenu.addMenu(&translateFromMenu);
//		userMenu.addMenu(&translateToMenu);
		QObject::connect(&image,SIGNAL(customContextMenuRequested( const QPoint &)),this,SLOT(handleCustomContextMenuRequested ( const QPoint &)));
		image.setContextMenuPolicy(Qt::CustomContextMenu);
	}

	QObject::connect(&groupMenu,SIGNAL(aboutToShow()),this,SLOT(handleShowGroupMenu()));
	QObject::connect(&translateToMenu,SIGNAL(aboutToShow()),this,SLOT(handleShowTranslateToMenu()));
	QObject::connect(&translateFromMenu,SIGNAL(aboutToShow()),this,SLOT(handleShowTranslateFromMenu()));
	QObject::connect(&groupMenu,SIGNAL(triggered(QAction *)),this,SLOT(handleGroupMenuTriggered(QAction*)));
	QObject::connect(&image,SIGNAL(doubleclicked()),this,SLOT(handleDoubleclick()));
	QObject::connect(&userInfo,SIGNAL(clicked()),this,SLOT(handleUserInfo()));
	QObject::connect(&refreshUser,SIGNAL(clicked()),this,SLOT(handleRefreshUser()));

	ui.text->installEventFilter(this);


	translator = new Translator();

	QObject::connect(translator,SIGNAL(haveTranslation(int,QString,QString,QString)),this,SLOT(handleTranslateResult(int,QString,QString,QString)));

	QMap<QString,QString> lang = translator->getLanguages();
	QList<QString> keys = lang.keys();
	allLanguagesFrom.clear();
	allLanguagesTo.clear();
	allLanguagesFrom.setTitle(tr("All languages"));
	allLanguagesTo.setTitle(tr("All languages"));

	int x=0;
	for(x=0;x<keys.size();x++) {
		QString code = keys.at(x);
		QAction *act = allLanguagesFrom.addAction(lang.value(code));
		act->setData(code);
		QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateFrom()));
		act = allLanguagesTo.addAction(lang.value(code));
		act->setData(code);
		QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateTo()));
	}
	translated=false;

	compact=false;

	hideControls();
	refresh();

}

TweetWidget::~TweetWidget()
{
	if (tweet)
		tweet->refCount--;
	timer->stop();
	showTimer->stop();
	hideTimer->stop();
	//notNewAnyMoreTimer->stop();

/*
	QObject::disconnect(imgcache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)));
	QObject::disconnect(tweets,SIGNAL(haveUpdatedUser(TweetUser*)),this,SLOT(handleUpdatedUser(TweetUser*)));
	QObject::disconnect(ui.reply,SIGNAL(clicked()),this,SLOT(handleReplyClick()));
	QObject::disconnect(ui.retweet,SIGNAL(clicked()),this,SLOT(handleRetweetClick()));
	QObject::disconnect(ui.directMessage,SIGNAL(clicked()),this,SLOT(handleDirectMessageClick()));
	QObject::disconnect(ui.trash,SIGNAL(clicked()),this,SLOT(setDeleteMode()));
	QObject::disconnect(ui.follow,SIGNAL(clicked()),this,SLOT(handleFollowUser()));
	QObject::disconnect(ui.unfollow,SIGNAL(clicked()),this,SLOT(handleUnfollowUser()));
	QObject::disconnect(ui.pushButton,SIGNAL(clicked()),this,SLOT(buttonClicked()));
	QObject::disconnect(this,SIGNAL(reply(Tweet*)),tweets,SIGNAL(reply(Tweet*)));
	QObject::disconnect(this,SIGNAL(retweet(Tweet*)),tweets,SIGNAL(retweet(Tweet*)));
	QObject::disconnect(this,SIGNAL(setFavorite(Tweet*)),tweets,SIGNAL(setFavorite(Tweet*)));
	QObject::disconnect(this,SIGNAL(clearFavorite(Tweet*)),tweets,SIGNAL(clearFavorite(Tweet*)));
	QObject::disconnect(this,SIGNAL(directMessage(Tweet*)),tweets,SIGNAL(directMessage(Tweet*)));
	QObject::disconnect(this,SIGNAL(deleteTweet(Tweet*)),tweets,SIGNAL(handleDeleteTweet(Tweet*)));
	QObject::disconnect(this,SIGNAL(doSearch(QString)),tweets,SIGNAL(doSearch(QString)));
	QObject::disconnect(this,SIGNAL(followUserByScreenName(QString)),tweets,SIGNAL(followUserByScreenName(QString)));
	QObject::disconnect(this,SIGNAL(unfollowUserByScreenName(QString)),tweets,SIGNAL(unfollowUserByScreenName(QString)));
	QObject::disconnect(this,SIGNAL(showUserDetails(TweetUser *)),tweets,SIGNAL(showUserDetails(TweetUser *)));
	QObject::disconnect(&image,SIGNAL(doubleclicked()),this,SLOT(handleDoubleclick()));
	QObject::disconnect(ui.text,SIGNAL(linkActivated(QString)),this,SLOT(haveURL(QString)));
	QObject::disconnect(ui.info,SIGNAL(linkActivated(QString)),this,SLOT(haveURL(QString)));
	QObject::disconnect(ui.fav,SIGNAL(doubleclicked()),this,SLOT(handleFavorite()));
	QObject::disconnect(timer,SIGNAL(timeout()),this,SLOT(tick()));
	QObject::disconnect(showTimer,SIGNAL(timeout()),this,SLOT(showControls()));
	QObject::disconnect(hideTimer,SIGNAL(timeout()),this,SLOT(hideControls()));
	QObject::disconnect(&image,SIGNAL(doubleclicked()),this,SLOT(handleDoubleclick()));
	QObject::disconnect(&userInfo,SIGNAL(clicked()),this,SLOT(handleUserInfo()));
	QObject::disconnect(&refreshUser,SIGNAL(clicked()),this,SLOT(handleRefreshUser()));
	//QObject::disconnect(notNewAnyMoreTimer,SIGNAL(timeout()),this,SLOT(removeNewIndicator()));
*/

	timer->deleteLater();
	showTimer->deleteLater();
	hideTimer->deleteLater();
	//delete notNewAnyMoreTimer;
}

void TweetWidget::buttonClicked() {
	timer->stop();
	if(compact)
		setCompactMode(compact);
	switch(widgetState) {
		case TweetWidget::stateRetweet:
			emit retweet(tweet,false);
		case TweetWidget::stateUnfollow :
		case TweetWidget::stateDeleting :
			setListMode();
			showControls();
			break;
		case TweetWidget::stateTimer :
			emit timerCancel(this);
			break;
		case TweetWidget::stateNormal :
		default:
			emit cancel();
			break;
	}

/*

	if (timer->isActive()) {
		timer->stop();
		if (amDeleting) {
			showControls();
			setListMode();
		} else
			emit timerCancel(this);
	} else
		emit cancel();
*/
}

void TweetWidget::setListMode() {
	ui.pushButton->hide();
	ui.progressBar->hide();
	ui.reply->show();
	ui.retweet->show();
	ui.info->show();
	ui.directMessage->show();
	ui.date->show();
	if (tweets->getAuthenticatingUserScreenName()==tweet->screen_name) {
		ui.trash->show();
		ui.follow->hide();
		ui.unfollow->hide();
	} else {
		ui.trash->hide();
		ui.follow->hide();
		ui.unfollow->hide();
	}
	TweetUser *u = tweets->getUserByScreenName(tweet->screen_name);
	if (!u || !u->is_friend) {
		ui.unfollow->hide();
		ui.follow->show();
	} else {
		ui.unfollow->show();
		ui.follow->hide();
	}
	if (tweet->type==2) {
		ui.trash->hide();
		ui.follow->hide();
		ui.unfollow->hide();
		ui.reply->hide();
		ui.directMessage->hide();
		refreshUser.hide();
		userInfo.hide();
	}
	currentTime=0;
	ui.secondRow->show();
	listMode=true;
	widgetState=TweetWidget::stateNormal;
	isDirectMessage=false;
}

void TweetWidget::setDirectMessageListMode() {
	ui.pushButton->hide();
	ui.progressBar->hide();
	ui.reply->hide();
	ui.retweet->hide();
	ui.unfollow->hide();
	ui.fav->hide();
	ui.info->show();
	ui.directMessage->show();
	ui.trash->show();
	ui.date->show();
	ui.secondRow->show();
	currentTime=0;
	listMode=true;
	widgetState=TweetWidget::stateNormal;
	isDirectMessage=true;
}

void TweetWidget::setInReplyToMode() {
//	setBackgroundRole(QPalette::Midlight);
//	ui.text->setBackgroundRole(QPalette::Base);
	ui.text->setProperty("tweet", "reverse_reply");
	reverseReply=true;
	refresh();
}

void TweetWidget::setReplyMode() {
	ui.pushButton->show();
	ui.pushButton->setText(tr("Cancel reply"));
	ui.progressBar->hide();
	ui.reply->hide();
	ui.retweet->hide();
	ui.info->hide();
	ui.directMessage->hide();
	ui.follow->hide();
	ui.unfollow->hide();
	ui.trash->hide();
	ui.date->hide();
	ui.secondRow->hide();
	currentTime=0;
	listMode=false;
	widgetState=TweetWidget::stateNormal;
}

void TweetWidget::setDeleteMode() {
	ui.pushButton->show();
	ui.pushButton->setText(tr("Cancel delete"));
	ui.progressBar->show();
	ui.reply->hide();
	ui.retweet->hide();
	//ui.info->hide();
	ui.directMessage->hide();
	ui.trash->hide();
	ui.follow->hide();
	ui.unfollow->hide();
	ui.date->hide();
	currentTime=0;
	ui.secondRow->hide();
	timer->start(40);
	hideControls();
	listMode=false;
	widgetState=TweetWidget::stateDeleting;
}

void TweetWidget::setUnfollowMode() {
	ui.pushButton->show();
	ui.pushButton->setText(tr("Cancel unfollow"));
	ui.progressBar->show();
	ui.reply->hide();
	ui.retweet->hide();
	//ui.info->hide();
	ui.directMessage->hide();
	ui.trash->hide();
	ui.follow->hide();
	ui.unfollow->hide();
	ui.date->hide();
	currentTime=0;
	ui.secondRow->hide();
	timer->start(40);
	hideControls();
	listMode=false;
	widgetState=TweetWidget::stateUnfollow;
}

void TweetWidget::setRetweetMode() {
	ui.pushButton->show();
	ui.pushButton->setText(tr("Edit retweet"));
	ui.progressBar->show();
	ui.reply->hide();
	ui.retweet->hide();
	//ui.info->hide();
	ui.directMessage->hide();
	ui.trash->hide();
	ui.follow->hide();
	ui.unfollow->hide();
	ui.date->hide();
	currentTime=0;
	ui.secondRow->hide();
	timer->start(40);
	hideControls();
	listMode=false;
	widgetState=TweetWidget::stateRetweet;
}

void TweetWidget::setSendMode() {
	QSettings settings;
	ui.pushButton->show();
	ui.pushButton->setText(tr("Cancel send"));
	ui.progressBar->show();
	ui.reply->hide();
	ui.retweet->hide();
	ui.info->hide();
	ui.directMessage->hide();
	ui.trash->hide();
	ui.follow->hide();
	ui.unfollow->hide();
	ui.date->hide();
	currentTime=0;
	ui.secondRow->hide();
	timer->start(settings.value("sendingDelay",5).toInt()*4);
	hideControls();
	listMode=false;
	widgetState=TweetWidget::stateTimer;
}

void TweetWidget::setResendMode() {
	QSettings settings;
	setSendMode();
	ui.pushButton->setText(tr("Cancel resend"));
	timer->start(settings.value("sendingDelay",5).toInt()*4);
}

void TweetWidget::tick() {
	QSettings opts;
	bool displayRetweets = opts.value("displayretweetsfromoriguser",QVariant(false)).toBool();
	currentTime+=1;
	ui.progressBar->setValue(currentTime);
	if (currentTime>=timeOut) {
		timer->stop();
		switch (widgetState) {
			case TweetWidget::stateRetweet :
				emit retweet(this->tweet,true);
				setListMode();
				break;
			case TweetWidget::stateDeleting :
				emit deleteTweet(this->tweet);
				setListMode();
				break;
			case TweetWidget::stateUnfollow :
				if (displayRetweets && tweet->retweet)
					emit unfollowUserByScreenName(tweet->retweet_screen_name);
				else
					emit unfollowUserByScreenName(tweet->screen_name);
				setListMode();
				break;
			default:
				emit timerComplete(this);
		}
	}
}

void TweetWidget::updateImage(QString url) {
	QSettings opts;
	bool displayRetweets = opts.value("displayretweetsfromoriguser",QVariant(false)).toBool();
	TweetUser *user;
	if (displayRetweets && tweet->retweet) {
		user = tweets->getUserByScreenName(tweet->retweet_screen_name);
	} else {
		user = tweets->getUserByScreenName(tweet->screen_name);
	}
	if (user) {
		if (url!=user->image_url)
			return;
		image.setPixmap(imgcache->getPixMap(url));
	}
//	emit updated();
}

void TweetWidget::refresh() {
	if (!this->tweet)
		return;
	QSettings opts;
	bool displayRetweets = opts.value("displayretweetsfromoriguser",QVariant(false)).toBool();
	QString info="<style>"+Ya_st::labelCSS+"</style><p class=\"info\">";
	if (tweet->type<2) {
		TweetUser *u;
		if (displayRetweets && tweet->retweet) {
			retweetIndicator->show();
			u = tweets->getUserByScreenName(tweet->retweet_screen_name);
			if (u) {
				info+=u->screen_name;
			} else
				info+=tweet->retweet_screen_name;

			displayScreenName=tweet->retweet_screen_name;
			TweetUser *origU = tweets->getUserByScreenName(tweet->screen_name);
			if (origU) {
				retweetIndicatorButton->setIcon(QIcon(imgcache->getPixMap(origU->image_url)));
			}
		} else {
			retweetIndicator->hide();
			u = tweets->getUserByScreenName(tweet->screen_name);
			if (u)
				info+=u->screen_name;
			else
				info+=tweet->screen_name;
			displayScreenName=tweet->screen_name;
		}
		if (!u || !u->is_friend) {
			ui.unfollow->hide();
			ui.follow->show();
		} else {
			ui.unfollow->show();
			ui.follow->hide();
		}
		if (displayRetweets && tweet->retweet) {
			if (tweet->retweet_in_reply_to_status_id!="") {
				if (haveClickedInReplyTo)
					info += tr(" replying");
				else
					info += " @<a href=\"twitter:/inreplyto\">"+tweet->retweet_in_reply_to_screen_name+"</a>";
			}
		} else {
			if (tweet->in_reply_to_status_id!="") {
				if (haveClickedInReplyTo)
					info += tr(" replying");
				else
					info += " @<a href=\"twitter:/inreplyto\">"+tweet->in_reply_to_screen_name+"</a>";
			}
		}
		if (displayRetweets && tweet->retweet) {
			if (tweet->retweet_source!="")
				info += tr(" via ")+tweet->retweet_source;
		} else {
			if (tweet->source!="")
				info += tr(" via ")+tweet->source;
		}
		ui.info->setText(info+"</p>");
//		ui.date->setText(tweet->created_at.toLocalTime().toString("ddd, MMM d HH:mm"));
		ui.date->setText(tweet->created_at.toLocalTime().toString(Qt::SystemLocaleLongDate));
		QString text;
		if (translated)
			text = convertToHTML(translation)+"<br><a href=\"removetranslation:/\">"+tr("Remove translation")+"</a>";
		else {
			if (displayRetweets && tweet->retweet)
				text=convertToHTML(tweet->retweet_message)+"(via <a href=\"twitter:/userinfo/"+tweet->screen_name+"\">@"+tweet->screen_name+"</a>)\n";//Doesn't render correctly without the NL
			else
				text=(tweet->html_message!=""?tweet->html_message:convertToHTML(tweet->message))+"\n";//Doesn't render correctly without the NL
		}
		ui.text->setText(text);
		if (!compact) {
			if (displayRetweets && tweet->retweet)
				image.setToolTip(tr("Double click to view %1's profile in your web browser.\nRight click for more options.").arg(tweet->retweet_screen_name));
			else
				image.setToolTip(tr("Double click to view %1's profile in your web browser.\nRight click for more options.").arg(tweet->screen_name));
		} else
			image.setToolTip(tr("Double click to mention this tweet.\nRight click for more options."));
		userInfo.setToolTip(tr("Click to view %1's profile and latest tweets in a new column.").arg(tweet->screen_name));
		refreshUser.setToolTip(tr("Click to load %1's latest tweets (including mentions)").arg(tweet->screen_name));
		updateImage((u)?u->image_url:"");

		if (this->tweet->favorite) {
			fav=true;
			ui.fav->setPixmap(QPixmap(":/buttons/fav-on.png"));
		} else {
			fav=false;
			ui.fav->setPixmap(QPixmap(":/buttons/fav-off.png"));
		}
	} else if (tweet->type==2) {
		FeedItem *item = (FeedItem*)tweet;
		info += "<a href=\""+item->url+"\">View...</a>";
		ui.info->setText(info+tr("</p>"));
		ui.fav->hide();
		image.setPixmap(QPixmap(":/buttons/feed.png"));
		ui.text->setText("<style>"+Ya_st::labelCSS+"</style><b>"+item->message+"</b><br>"+item->content);
		if (item->feed_item_date!="")
			ui.date->setText(item->feed_item_date);
		else
			ui.date->setText(tweet->created_at.toLocalTime().toString(Qt::SystemLocaleLongDate));
			//ui.date->setText(tweet->created_at.toLocalTime().toString("ddd, MMM d HH:mm"));
	}
	if (tweet->read && property("tweet")=="new")
		removeNewIndicator();
/*	if (u && !u->is_friend)
		ui.follow->show();
	else
		ui.follow->hide();*/
//	if (!u)
//		QTimer::singleShot(10000, this, SLOT(refresh()));
}

QString TweetWidget::convertToHTML(QString in) {
	QString out;
	QRegExp match("https*://[^ \\r\\n]+[^ .\\r\\n\\(\\)]");
	QRegExp idmatch("@([0-9a-zA-Z_]+)");
	QRegExp hashmatch("#([0-9a-zA-Z_]+)");
	QSettings opts;
	int pos=0,lastpos=0;
	//QString in = tweet->message;

	out="<style>"+Ya_st::labelCSS+"</style>";

	if (reverseReply)
		out+="<p class=\"revreply\">";
	else
		out+="<p>";

/*
	if (reverseReply)
		QMessageBox::information (0,"assdf",out);
*/
	while ((pos = match.indexIn(in,pos))>=0) {
		out+=in.mid(lastpos,pos-lastpos);
		pos += match.cap(0).length();
		QString url = match.cap(0);
//		if (url.endsWith('.')) {
//			url.chop(1);
//		}

		QString urlText = (opts.value("shorturls",QVariant(false)).toBool())?"[link]":match.cap(0);
		out+="<a class=\"link\" href=\""+QUrl::fromPercentEncoding(url.toUtf8())+"\">"+urlText+"</a>";
		lastpos=pos;
	}
	out+=in.mid(lastpos,-1);

	in=out;
	pos=0;
	lastpos=0;
	out="";
	while ((pos = idmatch.indexIn(in,pos))>=0) {
		out+=in.mid(lastpos,pos-lastpos);
		pos += idmatch.cap(0).length();
		out+="<a href=\"twitter:/userinfo/"+idmatch.cap(1)+"\">@"+idmatch.cap(1)+"</a>";
		lastpos=pos;
	}
	out+=in.mid(lastpos,-1);

	in=out;
	pos=0;
	lastpos=0;
	out="";
	while ((pos = hashmatch.indexIn(in,pos))>=0) {
		out+=in.mid(lastpos,pos-lastpos);
		pos += hashmatch.cap(0).length();
		tweets->addHashTag(hashmatch.cap(1));
		out+="<a class=\"hash\" href=\"twitter:/hashsearch/"+hashmatch.cap(1)+"\">#"+hashmatch.cap(1)+"</a>";
		lastpos=pos;
	}
	out+=in.mid(lastpos,-1);

	return out+"</p>";
}

void TweetWidget::haveURL(QString url) {
//	QMessageBox::information (0,"assdf",url);
	if (url=="twitter:/reply") {
		if (!compact)
			emit reply(tweet);
	} else if (url.startsWith("twitter:/userinfo")) {
		if (!compact)
			emit showUserDetails(url.mid(18));
	} else if (url=="twitter:/retweet") {
		if (!compact)
			emit retweet(tweet,false);
	} else if (url.startsWith("twitter:/hashsearch/")) {
		if (!compact)
			emit doSearch("#"+url.mid(20));
	} else if (url=="twitter:/inreplyto") {
		emit inReplyTo(this);
		haveClickedInReplyTo=true;
		refresh();
	} else if (url=="removetranslation:/") {
		translated=false;
		translation="";
		refresh();
	} else
		QDesktopServices::openUrl(QUrl(url));

}

void TweetWidget::handleDoubleclick() {
	if (compact)
		emit reply(tweet);
	else
		QDesktopServices::openUrl(QUrl("http://www.twitter.com/"+displayScreenName));
}

void TweetWidget::handleUserInfo() {
//	QMessageBox::information (0,"assdf",displayScreenName);
	emit showUserDetails(displayScreenName);
}

void TweetWidget::handleReplyClick() {
	emit reply(tweet);
}

void TweetWidget::handleRetweetClick() {
	if (tweet && tweet->type==2) // feed item
		emit retweet(tweet,false);
	else
		setRetweetMode();
//	emit retweet(tweet,false);
}

void TweetWidget::handleDirectMessageClick() {
	emit directMessage(tweet);
}

void TweetWidget::setTweet(Tweet* t) {
	if (tweet)
		tweet->refCount--;
	t->refCount++;
	tweet=t;
}

Tweet* TweetWidget::getTweet() {
	return tweet;
}

void TweetWidget::handleFollowUser() {
	QSettings opts;
	bool displayRetweets = opts.value("displayretweetsfromoriguser",QVariant(false)).toBool();
	if (displayRetweets && tweet->retweet)
		emit followUserByScreenName(tweet->retweet_screen_name);
	else
		emit followUserByScreenName(tweet->screen_name);
}

void TweetWidget::handleUnfollowUser() {
	setUnfollowMode();
}

void TweetWidget::handleUpdatedUser(TweetUser* u) {
	QSettings opts;
	bool displayRetweets = opts.value("displayretweetsfromoriguser",QVariant(false)).toBool();
	QString screen_name = tweet->screen_name;
	if (tweet && tweet->retweet && displayRetweets)
		screen_name = tweet->retweet_screen_name;

	if (u->screen_name==screen_name&&tweets->getAuthenticatingUserScreenName()!=screen_name) {
		if (u->is_friend) {
			ui.unfollow->show();
			ui.follow->hide();
		} else {
			ui.unfollow->hide();
			ui.follow->show();
		}
	}
}

bool TweetWidget::event(QEvent *e) {
	QEvent::Type t=e->type();
	if (t==QEvent::Enter) {
	//	showTimer->start();
		if (listMode)
			showControls();
	} else if (t==QEvent::Leave) {
	//	hideTimer->start();
		hideControls();
	}
	return QWidget::event(e);
}

void TweetWidget::showControls() {
//	ui.secondRow->show();
	ui.buttonContainer->show();
	if (tweet->type<2)
		refreshUser.show();
	userControls->show();
/*
	double o = ui.buttonContainer->windowOpacity();
	hideTimer->stop();

	o+=0.1;
	if (o>=1.0){
		showTimer->stop();
		o=1.0;
	}
	ui.buttonContainer->setWindowOpacity(o);
*/
}

void TweetWidget::hideControls() {
//	ui.secondRow->hide();
	ui.buttonContainer->hide();
	userControls->hide();
/*
	double o = ui.buttonContainer->windowOpacity();
	showTimer->stop();

	o-=0.1;
	if (o<=0.0){
		hideTimer->stop();
		o=0.0;
	}
	ui.buttonContainer->setWindowOpacity(o);
*/
}

void TweetWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QWidget::style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*
void TweetWidget::changeEvent ( QEvent * e) {
	if (newTweet && e->type()==QEvent::ActivationChange) {
		QMessageBox::information (0,"assdf","sdfg");
		if (windowState()&Qt::WindowActive) {
			notNewAnyMoreTimer->start();
		} else {
			notNewAnyMoreTimer->stop();
		}
		//close();
	}
}
*/

void TweetWidget::removeNewIndicator() {
//	QMessageBox::information (0,"assdf","sdfg");
	setProperty("tweet", "");
	setStyleSheet("");
/*
	if (tweet && !tweet->read) {
		tweet->read=true;
		emit tweetUpdated(tweet);
	}
*/
//	notNewAnyMoreTimer->stop();
//	if (newTweet) {
//		newTweet=false;
//		windowWidget->removeEventFilter(this);
//	}
}

/*
bool TweetWidget::eventFilter(QObject* obj,QEvent *e) {
	if (e->type()==QEvent::ActivationChange && newTweet) {
//		QMessageBox::information (0,"assdf","sdfg");
		if (windowWidget->isActiveWindow()) {
			notNewAnyMoreTimer->start();
			newTweet=false;
			windowWidget->removeEventFilter(this);
		} else {
			notNewAnyMoreTimer->stop();
		}
		//close();
	}
	return QObject::eventFilter(obj,e);
}
*/

void TweetWidget::setNew() {
	if (tweet && !tweet->read) {
		setProperty("tweet","new");
		setStyleSheet("");
	}
}

void TweetWidget::clearNew(bool fast) {
	setProperty("tweet", "");
	if (!fast)
		setStyleSheet("");
	if (tweet && !tweet->read) {
		tweet->read=true;
	}
}

bool TweetWidget::isNew() {
	bool isNew = !(tweet->read);
	if (property("tweet")=="new" && !isNew) {
		clearNew();
	}
	return isNew;
}

void TweetWidget::handleFavorite() {
	if (fav)
		emit clearFavorite(this->tweet);
	else
		emit setFavorite(this->tweet);
}

void TweetWidget::handleRefreshUser() {
	if(!tweets->hasCredentials())
		return;
	TwitterUserTweetsFetcher *ufetcher = new TwitterUserTweetsFetcher();
	ufetcher->setFetchScreenName(displayScreenName);
	ufetcher->periodic=false;
	tweets->addFetcher(ufetcher);
	ufetcher->populate();
	refreshUser.hide();
}

void TweetWidget::handleShowGroupMenu() {
	QList<Group*> groups = tweets->getGroups();

	groupMenu.clear();
	for(int x=0;x<groups.size();x++) {
		Group* g=groups.at(x);
		if (g && ((g->twitterList && g->user==g->ownerScreenName) || !g->twitterList)) {
			QAction *act = groupMenu.addAction(g->name);
			act->setData(x);
			if (g->twitterList)
				act->setIcon(QIcon(":/buttons/twitter.png"));
		}
	}
/*
	QList<QObject*> groups = tweets->getGroups();

	groupMenu.clear();
	for(int x=0;x<groups.size();x++) {
		GroupListWidget *group = (GroupListWidget*)groups.at(x);
		groupMenu.addAction(group->getName())->setData(x);
	}
*/

}

void TweetWidget::handleShowTranslateFromMenu() {
	QMap<QString,QString> lang = TweetWidget::translator->getLanguages();
	translateFromMenu.clear();

	QAction *act = translateFromMenu.addAction(tr("Guess from tweet"));
	act->setData("");
	QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateFrom()));

	int x=0;
	for(x=0;x<lastLang.size();x++) {
		QAction *act = translateFromMenu.addAction(lang.value(lastLang.at(x)));
		act->setData(lastLang.at(x));
		QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateFrom()));
	}
	translateFromMenu.addSeparator();
	translateFromMenu.addMenu(&allLanguagesFrom);
}

void TweetWidget::handleShowTranslateToMenu() {
	QMap<QString,QString> lang = TweetWidget::translator->getLanguages();
	translateToMenu.clear();

	int x=0;
	if (lastLang.size()==0) {
		QList<QString> keys = lang.keys();
		for (x=0;x<keys.size();x++) {
			QAction *act = translateToMenu.addAction(lang.value(keys.at(x)));
			act->setData(keys.at(x));
			QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateTo()));
		}
		return;
	}
	for(x=0;x<lastLang.size();x++) {
		QAction *act = translateToMenu.addAction(lang.value(lastLang.at(x)));
		act->setData(lastLang.at(x));
		QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateTo()));
	}
	translateToMenu.addSeparator();
	translateToMenu.addMenu(&allLanguagesTo);
}

void TweetWidget::handleTranslateFrom() {
	QSettings opts;
	QAction *act = qobject_cast<QAction*>(sender());
	QString from = act->data().toString();
	QString to = opts.value("baseLanguage","en").toString();

	if (from!="" && !lastLang.contains(from))
		lastLang.insert(0,from);
	if (TweetWidget::lastLang.size()>5)
		TweetWidget::lastLang.removeLast();

	TweetWidget::translator->translate(from,to,tweet->message);
}

void TweetWidget::handleTranslateTo() {
	QSettings opts;
	QAction *act = qobject_cast<QAction*>(sender());
	QString to = act->data().toString();
	QString from = opts.value("baseLanguage","en").toString();

	if (from!="" && !lastLang.contains(to))
		lastLang.insert(0,to);
	if (TweetWidget::lastLang.size()>5)
		TweetWidget::lastLang.removeLast();

	TweetWidget::translator->translate(from,to,tweet->message);
}

void TweetWidget::handleTranslateResult(int,QString,QString,QString resp) {
	translated = true;
	translation=resp;
	refresh();
}

void TweetWidget::handleCustomContextMenuRequested ( const QPoint & pos ) {
	userMenu.popup(QWidget::mapToGlobal(pos));
}

void TweetWidget::handleGroupMenuTriggered(QAction *act) {
	QList<Group*> groups = tweets->getGroups();
	int x = act->data().toInt();

	if (groups.at(x)) {
		groups.at(x)->updateGroupMembers(displayScreenName);
	}
}

void TweetWidget::handleRemoveFromThisGroup() {
	if (list && list->type==1)
		((GroupListWidget*)list)->removeUserByScreenName(tweet->screen_name);
}

void TweetWidget::setCompactMode(bool state) {
	if (state) {
		ui.reply->hide();
		ui.retweet->hide();
//		ui.info->hide();
		ui.directMessage->hide();
		ui.trash->hide();
		ui.follow->hide();
		ui.unfollow->hide();
		ui.date->hide();
		currentTime=0;
		ui.secondRow->hide();
		hideControls();
		listMode=false;
		image.setToolTip(tr("Double click to mention this tweet.\nRight click for more options."));
	} else {
		image.setToolTip(tr("Double click to view %1's profile in your web browser.\nRight click for more options.").arg(tweet->screen_name));
		if (isDirectMessage)
			setDirectMessageListMode();
		else
			setListMode();
	}
	compact=state;
}

QString TweetWidget::getReplyId() {
	QSettings opts;
	bool displayRetweets = opts.value("displayretweetsfromoriguser",QVariant(false)).toBool();
	if (tweet) {
		if (displayRetweets && tweet->retweet)
			return tweet->retweet_in_reply_to_status_id;
		else
			return tweet->in_reply_to_status_id;
	}
	return "";
}

BirdBox *TweetWidget::getAccount() {
	return tweets;
}

void TweetWidget::setFontSize(int point) {
	QFont font = ui.text->font();
	font.setPointSize(point);
	ui.text->setFont(font);
}

QSize TweetWidget::sizeHint() const {
	return optimisticSizeHint;
}

bool TweetWidget::eventFilter(QObject *, QEvent *event) {
	if (event->type()==QEvent::MouseButtonDblClick) {
		clearNew();
		return true;
	}
	return false;
}

void TweetWidget::handleLinkHovered(QString url) {
	if (url!=hoverUrl) {
		if (!url.startsWith("twitter")) {
			hoverTimer.stop();
			hoverUrl=url;
			hoverPos=QCursor::pos();
			hoverTimer.start(400);
		}

	}
}

void TweetWidget::handleLinkHoverTimer() {
	QPoint now = QCursor::pos();
	int xdiff = now.x()-hoverPos.x();
	int ydiff = now.y()-hoverPos.y();
	if ( (xdiff<15 && xdiff>-15) && (ydiff<15 && ydiff>-15) )
		QToolTip::showText(hoverPos,hoverUrl);
}
