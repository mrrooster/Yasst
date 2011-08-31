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
/* $Id: tweetlistwidget.cpp,v 1.73 2010/09/14 13:49:05 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#include "tweetlistwidget.h"
#include "ya_st.h"
#include "consts.h"
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include "backend/fetchers/WebSingleTweetFetcher.h"
#include <QMessageBox>
#include <QDateTime>
#include <QMutexLocker>
#include <QSettings>
#include <QScrollBar>
#include <QPoint>
#include <QCursor>
#include "tweetwidget.h"
#include "AbstractTweetWidget.h"
#include "yasstFns.h"
#include "YasstApplication.h"
#include "TweetListWidgetSpacer.h"

int TweetListWidget::masterCount=0;

TweetListWidget::TweetListWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	type=0;
}

TweetListWidget::TweetListWidget(BirdBox *b, ImageCache *c,QWidget *parent)
	: QWidget(parent),tweetsLock(QMutex::Recursive),knownTweetsLock(QMutex::Recursive),doingResize(false),doingDrag(false),otherDrag(false),filler(0)
{
	ui.setupUi(this);

	tweets=b;
	images=c;
	if (!images)
		images=new ImageCache();

	filterTimer = new QTimer(this);
	clearNewTimer = new QTimer(this);

	clearNewTimer->setSingleShot(true);

	this->setAttribute(Qt::WA_DeleteOnClose);
//	this->setAttribute(Qt::WA_QuitOnClose,false);

	ui.scrollArea->widget()->setObjectName("scrollareawidget");
	ui.scrollArea->widget()->layout()->setMargin(1);
	ui.stack->widget(0)->layout()->setMargin(1);
	ui.tweets->layout()->setMargin(0);
	ui.tweets->layout()->setSpacing(1);
	((QVBoxLayout*)ui.scrollArea->widget()->layout())->addStretch(1);

	ui.count->setText("");

	firstRefresh=true;
	inRefresh=false;
	canClearFirstRefresh=false;
	muted=false;


	((QVBoxLayout*)ui.tweets->layout())->setAlignment(Qt::AlignTop);
	QObject::connect(tweets,SIGNAL(haveTweet(Tweet *)),this,SLOT(tweetUpdate(Tweet *)));
	QObject::connect(tweets,SIGNAL(haveTweets(QList<Tweet *>&)),this,SLOT(tweetUpdates(QList<Tweet *>&)));
	QObject::connect(tweets,SIGNAL(deleteTweet(Tweet *)),this,SLOT(handleDeleteTweet(Tweet *)));
	QObject::connect(tweets,SIGNAL(finishedFirstFetch()),this,SLOT(handleFinishedFirstFetch()));
	QObject::connect(tweets,SIGNAL(accountRemoved()),this,SLOT(slowDelete()));
	QObject::connect(this,SIGNAL(removeTweet(Tweet *)),tweets,SIGNAL(deleteTweet(Tweet *)));
//	QObject::connect(tweets,SIGNAL(tweetUpdated(Tweet*)),this,SLOT(refreshTweet(Tweet*)));
	QObject::connect(ui.closeButton,SIGNAL(clicked()),this,SLOT(slowDelete()));
	QObject::connect(ui.refreshButton,SIGNAL(clicked()),this,SLOT(handleRefreshButton()));
	QObject::connect(ui.markasreadButton,SIGNAL(clicked()),this,SLOT(markAllAsRead()));
	QObject::connect(ui.muteButton,SIGNAL(clicked()),this,SLOT(toggleMute()));
	QObject::connect(ui.pruneButton,SIGNAL(clicked()),this,SLOT(handlePruneReadSingle()));
	QObject::connect(ui.popoutButton,SIGNAL(clicked()),this,SLOT(handlePopout()));
	QObject::connect(ui.filterButton,SIGNAL(clicked()),this,SLOT(showFiltering()));
	QObject::connect(ui.filterText,SIGNAL(editingFinished()),this,SLOT(hideFilteringHelp()));
	QObject::connect(ui.filterText,SIGNAL(textChanged(QString)),this,SLOT(filterTextChanged(QString)));
	QObject::connect(ui.filterText,SIGNAL(gotFocus()),this,SLOT(showFilteringHelp()));
	QObject::connect(ui.filterText,SIGNAL(lostFocus()),this,SLOT(hideFilteringHelp()));
	QObject::connect(ui.filterCloseButton,SIGNAL(clicked()),this,SLOT(hideFiltering()));
	QObject::connect(ui.count,SIGNAL(doubleclicked()),this,SLOT(markAllAsRead()));
	QObject::connect(filterTimer,SIGNAL(timeout()),this,SLOT(filterTimerFired()));
	QObject::connect(clearNewTimer,SIGNAL(timeout()),this,SLOT(expireNewIndicator()));
	QObject::connect(ui.scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(handleScrollbarValueChanged(int)));
//	QTimer::singleShot(0,this,SLOT(repopulate()));
	hideFiltering();
	hideTopText();
	defaultTitleStyle="";
	layout=0;

	timer=new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(prune()));
	timer->start(60000);
	minCount=200;
	maxAgeMins=30;
	isActive=false;
	inRefreshCount=0;
	lastWasMe=false;
	filteredCount=0;
	setObjectName(tr("tweetlistwidget"));
	ui.filterHelp->setText(tr("<style>")+Ya_st::labelCSS+tr("</style><p>Press RETURN to close this help.</p><p>To show tweets with <b class=\"hi\">certain words</b> type <i>certain words</i>. To hide other <b class=\"hi\">salacious</b> terms use <i>-salacious</i>.</p><p>For users use <i>@Fred</i>, and <i>-@Barny</i>.</p><p>For clients use <i>@@yasst</i>, and <i>-@@Spambot</i>.</p>"));
	tweetClamp=0;
	type=0;
	popoutMode=false;
	havePrunedTweets=false;
	maxCreateLimit=20;
	fillerTimestamp=0;

	ui.resizespacer->changeSize(0,0,QSizePolicy::Minimum,QSizePolicy::Minimum);

	ui.title->setToolTip(tr("Drag to move."));

	ui.pruneButton->setMenu(&pruneMenu);
	QObject::connect(pruneMenu.addAction(tr("Prune read from this column only.")),SIGNAL(triggered()),this,SLOT(handlePruneReadSingle()));
	QObject::connect(pruneMenu.addAction(tr("Prune read tweets in this column from all columns.")),SIGNAL(triggered()),this,SLOT(handlePruneRead()));


	setAccountIcon();

//	layout->setAlignment(Qt::AlignTop);
}

TweetListWidget::~TweetListWidget()
{
	filterTimer->stop();
	timer->stop();
/*
	QObject::disconnect(tweets,SIGNAL(haveTweet(Tweet *)),this,SLOT(tweetUpdate(Tweet *)));
	QObject::disconnect(tweets,SIGNAL(haveTweets(QList<Tweet *>&)),this,SLOT(tweetUpdates(QList<Tweet *>&)));
	QObject::disconnect(tweets,SIGNAL(deleteTweet(Tweet *)),this,SLOT(handleDeleteTweet(Tweet *)));
	QObject::disconnect(tweets,SIGNAL(finishedFirstFetch()),this,SLOT(handleFinishedFirstFetch()));
	QObject::disconnect(ui.closeButton,SIGNAL(clicked()),this,SLOT(close()));
	QObject::disconnect(ui.moveleftButton,SIGNAL(clicked()),this,SLOT(moveLeft()));
	QObject::disconnect(ui.moverightButton,SIGNAL(clicked()),this,SLOT(moveRight()));
	QObject::disconnect(ui.muteButton,SIGNAL(clicked()),this,SLOT(toggleMute()));
	QObject::disconnect(ui.filterButton,SIGNAL(clicked()),this,SLOT(showFiltering()));
	QObject::disconnect(ui.filterText,SIGNAL(editingFinished()),this,SLOT(hideFilteringHelp()));
	QObject::disconnect(ui.filterText,SIGNAL(textChanged(QString)),this,SLOT(filterTextChanged(QString)));
	QObject::disconnect(ui.filterText,SIGNAL(gotFocus()),this,SLOT(showFilteringHelp()));
	QObject::disconnect(ui.filterText,SIGNAL(lostFocus()),this,SLOT(hideFilteringHelp()));
	QObject::disconnect(ui.filterCloseButton,SIGNAL(clicked()),this,SLOT(hideFiltering()));
	QObject::disconnect(ui.count,SIGNAL(doubleclicked()),this,SLOT(markAllAsRead()));
	QObject::disconnect(filterTimer,SIGNAL(timeout()),this,SLOT(filterTimerFired()));
	QObject::disconnect(timer, SIGNAL(timeout()), this, SLOT(prune()));
	QObject::disconnect(clearNewTimer,SIGNAL(timeout()),this,SLOT(expireNewIndicator()));
	QObject::disconnect(ui.scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(handleScrollbarValueChanged(int)));
*/
	filterTimer->deleteLater();
	timer->deleteLater();
	for(int x=0;x<pendingTweets.size();x++)
		pendingTweets.at(x)->refCount--;
}

void TweetListWidget::filterTextChanged(QString ) {
	filterTimer->stop();
	filterTimer->start(200);
}

void TweetListWidget::prune() {
	QMutexLocker locker(&knownTweetsLock);
	QSettings opts;
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int count=getWidgetCount();
	int maxAgeMins = opts.value("maxTweetAgeInListMins",this->maxAgeMins).toInt();
	int minCount = opts.value("minTweetsPerList",this->minCount).toInt();
	if (maxAgeMins<1)
		maxAgeMins=1;
	if (minCount<1)
		minCount=1;

	if (tweetClamp>0) {
		if (tweetClamp<minCount)
			minCount=tweetClamp;
		if (tweetClamp<=count)
			maxAgeMins=1;
	}

	QDateTime cutoff = QDateTime::currentDateTime().addSecs(-(60*maxAgeMins));
	for(int x=minCount;x<count;x++) {
		AbstractTweetWidget *w = (AbstractTweetWidget*)layout->itemAt(x)->widget();
		if ((w->getTweet()->timestamp)<cutoff) {
//			w->deleteLater();
			knownTweets.remove(w->getTweet());
			layout->removeWidget(w);
			w->deleteLater();
			x--;
			count--;
		}
	}
}

bool TweetListWidget::widgetIsFiltered(AbstractTweetWidget *w) {
	QSettings opts;
	QString globalFilter=opts.value("globalfiltertext","").toString();
	if (filterText.size()==0&&globalFilter.size()==0)
		return false;

	if (!w->getTweet())
		return false;

	bool matched=true;
	QString text = w->getTweet()->message;
	if (text=="")
		text=deHTML(w->getTweet()->html_message);

	QStringList words = filterText.split(" ");
	words += globalFilter.split(" ");
	for (int x=0;x<words.size()&&matched;x++) {
		QString word = words.at(x);
		bool match,negate=false;
		if (word.left(1)=="-") {
			negate=true;
			word=word.mid(1,-1);
		} else if (word.left(1)=="+") {
			word=word.mid(1,-1);
		}
		if (word.left(1)=="@") {
			word = word.mid(1,-1);
			if (word.left(1)=="@") {
				match = (w->getTweet()->getTextSource().compare(word.mid(1,-1),Qt::CaseInsensitive)==0);
				//QMessageBox::information(0,"","="+w->getTweet()->getTextSource());
			} else
				match = (tweets->getUser(w->getTweet()->user_id)->screen_name.compare(word,Qt::CaseInsensitive)==0);
		} else
			match = text.contains(word,Qt::CaseInsensitive);

		if (negate)
			match=!match;
		matched = matched && match;
	}
	return !matched;
}

void TweetListWidget::filterTimerFired() {
	filterTimer->stop();
	refresh();
}
void TweetListWidget::refresh() {
	QSettings opts;
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int filtercount=0;

	filterText=ui.filterText->text();
	if (filterText!="")
		ui.title->setStyleSheet("color: red;");
	else
		ui.title->setStyleSheet(defaultTitleStyle);

	int count=getWidgetCount();
	for (int x=0;!inRefresh&&x<count;x++) {
		AbstractTweetWidget *w = (AbstractTweetWidget*)layout->itemAt(x)->widget();
		if (w->type==0) {
			w->refresh();
			((TweetWidget*)w)->setFontSize(opts.value("fonts/tweettextsize",DEFAULT_FONT_SIZE).toInt());
			w->setVisible(!widgetIsFiltered(w));
			if (!widgetIsFiltered(w))
				filtercount++;
		}
	}
	filteredCount=filtercount;
	ui.tweets->update();
	setTweetCount();
	setAccountIcon();
}

void TweetListWidget::hideFiltering() {
	hideFilteringHelp();
	ui.filterText->setVisible(false);
	ui.filterCloseButton->setVisible(false);
	ui.filterText->setText("");
	filterTimerFired();
}

void TweetListWidget::hideFilteringHelp() {
	ui.filterHelp->setVisible(false);
}

void TweetListWidget::showFilteringHelp() {
	ui.filterHelp->setVisible(true);
}

void TweetListWidget::showFiltering() {
	ui.filterHelp->setVisible(false);
	ui.filterText->setVisible(true);
	ui.filterCloseButton->setVisible(true);
}

void TweetListWidget::hideTopText() {
	ui.topText->hide();
	ui.topCloseButton->hide();
}

void TweetListWidget::showTopText() {
	ui.topText->show();
	ui.topCloseButton->show();
}



void TweetListWidget::setTweetsVisible() {
	setStack(0);
}

void TweetListWidget::setStack(int x) {
	if (layout)
		layout->setEnabled(false);
	ui.stack->setCurrentIndex(x);
	if (layout)
		layout->setEnabled(true);
	emit showMe(this);
}

void TweetListWidget::childUpdate() {
	this->show();
}

void TweetListWidget::tweetUpdate(Tweet *t) {
	if ((canDoUpdate(t)||inReplyToTweets.contains(t->id))&&!t->deleted) {
//		QMessageBox::information (0,"assdf",t->message);
		if (!havePrunedTweets || !(t->read))
			doTweetUpdate(t);
	}
}

void TweetListWidget::tweetUpdates(QList<Tweet*> &tweets) {
	QMutexLocker tweetsLocker(&tweetsLock);
	int count=0;
//	startDrainTimer();
	for(int x=0;x<tweets.size();x++) {
		Tweet *t=tweets.at(x);
		if ((canDoUpdate(t)||inReplyToTweets.contains(t->id))&&!t->deleted) {
			t->refCount++;
			pendingTweets.append(t);
			count++;
		} else
			refreshTweet(t);
	}
	if (!firstRefresh && count) {
		emit tweetsAdded(muted);
//		QMessageBox::information (0,"assdf",QString::number(count));
	}
	if (count)
		emit tweetsPending();
}

void TweetListWidget::startDrainTimer() {
	QSettings opts;
	if (
			!opts.value("dansannoyingscrollything",QVariant(false)).toBool()||
			((pendingTweets.size()>100)||(!pendingTweets.size()&&(isActive||firstRefresh)))
		)
		QTimer::singleShot(0, this, SLOT(drainTweets()));
}

bool TweetListWidget::canDoUpdate(Tweet *t) {
	QMutexLocker tweetsLocker(&tweetsLock);
	QMutexLocker locker(&knownTweetsLock);
	if (knownTweets.contains(t)
			/*||pendingTweets.contains(t)*/
			)
		return false;
	return (t->type==0);
}

void TweetListWidget::doTweetUpdate(Tweet *t) {
	QMutexLocker locker(&knownTweetsLock);
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	TweetWidget *w = 0;
	int x;

//	w->setStyleSheet(tr("background-color: rgb(10,10,10);"));

	int y=getWidgetCount();
	if (inReplyToTweets.contains(t->id)) {
		w = new TweetWidget(t,tweets,images,this,this);
		w->setInReplyToMode();
		AbstractTweetWidget *nextTo = inReplyToTweets.take(t->id);
		for (x=0;x<y&&(((AbstractTweetWidget*)layout->itemAt(x)->widget())!=nextTo);x++) ;
		//QMessageBox::information (0,"assdf",QString::number(x));
		x++;

	} else {
		QDateTime date=t->created_at;

		for (x=0;x<y;x++) {
			AbstractTweetWidget *item=(AbstractTweetWidget*)layout->itemAt(x)->widget();
			if ((item->getTweet()->id)==(t->id)) {
				item->refresh();
//				delete w;
				return;
			}
		}
		for(x=0;x<y&&(  date<((AbstractTweetWidget*)layout->itemAt(x)->widget())->getTweet()->created_at  );x++) ;
	}
	if (!w) {
		w = new TweetWidget(t,tweets,images,this,this);
		if (layout->count()>0) {
			w->optimisticSizeHint=layout->itemAt(layout->count()-1)->widget()->size();
		}
	}

	if (/*!firstRefresh&&*/(t->created_at>QDateTime::currentDateTime().addSecs(-3600)))
		w->setNew();
	else
		t->read=true;
	if (widgetIsFiltered(w)) {
		w->setVisible(false);
	}
	w->setProperty("tweetwidget", "true");
	w->setCompactMode(popoutMode);
	layout->insertWidget(x,w);
	knownTweets.insert(t,w);
//	refresh();

//	QObject::connect(w,SIGNAL(updated()),this,SLOT(refresh()));
	QObject::connect(w,SIGNAL(inReplyTo(TweetWidget*)),this,SLOT(handleInReplyTo(TweetWidget*)));

}

void TweetListWidget::setTweetCount() {
	int x=getUnreadWidgetCount();
	int y=getWidgetCount(true);
	QString countText = QString::number(y);
	QString countTip = QString::number(y)+tr(" tweet")+((y!=1)?tr("s"):"");
	if (x) {
		countText+=tr("(")+QString::number(x)+tr(")");
		countTip += tr(", ")+QString::number(x)+tr(" unread");
	}
	QSettings opts;
	QString globalFilter=opts.value("globalfiltertext","").toString();
	if (!inRefresh) {
		if (ui.filterText->isVisible()||(globalFilter.size()>0&&y!=filteredCount)) {
			countText += tr(" [")+QString::number(filteredCount)+tr("]");
			countTip += tr(", ")+QString::number(y-filteredCount)+tr(" filtered out");
		}
	}
	countTip += tr(".");
	if (x)
		countTip += tr("\nDouble click to mark all read.");

	ui.count->setText(countText);
	ui.count->setToolTip(countTip);
}

void TweetListWidget::handleInReplyTo(TweetWidget *w) {
	QString status = w->getReplyId();

	inReplyToTweets.insert(status,w);
	if (tweets->hasTweet(status)) {
		doTweetUpdate(tweets->getTweet(status));
	} else {
		WebSingleTweetFetcher *fetcher=new WebSingleTweetFetcher();

		fetcher->setPreText(tr("<style>")+Ya_st::labelCSS+tr("</style>"));
		tweets->addFetcher(fetcher);
		fetcher->fetchSingleTweet(w->getTweet()->in_reply_to_screen_name,w->getTweet()->in_reply_to_user_id,status);
	}
}

void TweetListWidget::removeAllTweets(bool tweetsOnly) {
	QMutexLocker locker(&knownTweetsLock);
	QVBoxLayout *l = (QVBoxLayout*)ui.tweets->layout();
	AbstractTweetWidget *w;
	int skipCount=0;

	while (l->count()>skipCount) {
		w = (AbstractTweetWidget*)l->itemAt(skipCount)->widget();
		if (!tweetsOnly || w->type==0) {
			l->removeWidget(w);
			w->deleteLater();
		} else
			skipCount++;
	}
	knownTweets.clear();
}

void TweetListWidget::writeSettings(QSettings &settings) {
	settings.setValue("type",type);
	settings.setValue("muted",muted);
	if (filterText!="")
		settings.setValue("filter",filterText);
	settings.setValue("width",width());
	settings.setValue("account",tweets->getId());
}

void TweetListWidget::readSettings(QSettings &settings) {
	muted=settings.value("muted",muted).toBool();
	QString oldFilter = settings.value("filter","").toString();
	if (oldFilter!="") {
		ui.filterText->setText(oldFilter);
		showFiltering();
		filterTimerFired();
	}
	ui.muteButton->setChecked(muted);
	int w = settings.value("width",272).toInt();
	if (w<250)
		w=250;
	else if (w>1000)
		w=1000;
	setMaximumWidth(w);
	setMinimumWidth(w);

}

void TweetListWidget::repopulate() {
	QMutexLocker tweetsLocker(&tweetsLock);
	QList<Tweet *> tweets;
//	removeAllTweets();
	removeUnwantedTweets();
	tweets = this->tweets->getTweets();
	for(int x=0;x<tweets.size();x++) {
		Tweet *t=tweets.at(x);
		if ((canDoUpdate(t)||inReplyToTweets.contains(t->id))&&!t->deleted) {
			t->refCount++;
			pendingTweets.append(t);
		}
	}
	startDrainTimer();

}

void TweetListWidget::drainTweets() {
	QMutexLocker tweetsLocker(&tweetsLock);
	QSettings opts;
//	QBoxLayout *layout = (QBoxLayout*)ui.scrollArea->widget()->layout();
	//ui.tweets->layout()->setEnabled(false);
//	ui.scrollArea->widget()->setUpdatesEnabled(false);
	if ((!inRefresh)&&(!pendingTweets.size()))
		return;

	QScrollBar *bar = ui.scrollArea->verticalScrollBar();
	if (!inRefresh) {
		scrollBarLength = bar->maximum();
		inRefresh=true;
		inRefreshCount=0;
		lastWasMe=false;
		TweetListWidget::masterCount++;
//		layout->setEnabled(false);
	}

	if (!pendingTweets.size()) {
		inRefresh=false;
		refresh();
		TweetListWidget::masterCount--;
		if (TweetListWidget::masterCount<0)
			TweetListWidget::masterCount=0;
//		QMessageBox::information (0,"assdf","Hello!");
		if (opts.value("dansannoyingscrollything",QVariant(false)).toBool()&&
				((!firstRefresh)&&(!(lastWasMe&&(inRefreshCount==1))))
			)
			bar->setValue((bar->value())+(bar->maximum()-scrollBarLength));
		if (canClearFirstRefresh) {
			firstRefresh=false;
			canClearFirstRefresh=false;
		}
//		layout->setEnabled(true);
//		layout->activate();
		clearNewTimer->start(5000);
		//QTimer::singleShot(5000,this,SLOT(expireNewIndicator()));
		return;
	}

//	layout->setEnabled(false);
	int limit = pendingTweets.size();
	if (limit>maxCreateLimit)
		limit=maxCreateLimit;
	if (limit<maxCreateLimit)
		limit=(limit/5)+1;
	for (int x=0;x<limit;x++) {
		Tweet *t=pendingTweets.takeLast();
//		QMessageBox::information(this,"",t->message);
//		QTimer::singleShot(0, this, SLOT(drainTweets()));
		tweetUpdate(t);
		t->refCount--;
		inRefreshCount++;
		lastWasMe=(t->screen_name==tweets->getAuthenticatingUserScreenName());
	}
//	QMessageBox::information(this,"",QString::number(limit));
///	pendingTweets.clear();
//	layout->setEnabled(true);
	//layout->activate();
/*
	if (inRefresh) {
		if (!firstRefresh) {
			bar->setValue((bar->value())+(bar->maximum()-scrollBarLength));
			scrollBarLength = bar->maximum();
		}
	}
*/
	inRefresh=true;
	refresh();
//	QTimer::singleShot((TweetListWidget::masterCount*78)+(limit<=3)?0:60+(yasst_rand()%400), this, SLOT(drainTweets()));
//	ui.scrollArea->widget()->setUpdatesEnabled(true);
	//ui.tweets->layout()->setEnabled(true);
}

void TweetListWidget::haveURL(QString ) {
/*	if (url=="twitter:/panelleft") {
	} else if (url=="twitter:/panelright") {
	} else if (url=="twitter:/filter") {
		showFiltering();
	}*/
}

void TweetListWidget::setLayout(QLayout *l) {
	layout=l;
}

void TweetListWidget::handleDeleteTweet(Tweet *t) {
	QMutexLocker locker(&knownTweetsLock);
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int y=getWidgetCount();
	int x;

	//QMessageBox::information (0,"assdf","Hello!"+QString::number(y));
	for (x=0;x<y;x++) {
		AbstractTweetWidget *w=(AbstractTweetWidget*)layout->itemAt(x)->widget();
		if (w->getTweet()==t) {
			bool filtered = widgetIsFiltered(w);
			delete layout->takeAt(x);
			w->deleteLater();
			knownTweets.remove(t);
			if (!filtered && filteredCount>0)
				filteredCount--;
			setTweetCount();
			return;
		}
	}
}

void TweetListWidget::handleFinishedFirstFetch() {
	canClearFirstRefresh=true;
}

void TweetListWidget::setActive(bool active) {
	if (!isActive && active) {
		if (pendingTweets.size())
			QTimer::singleShot(0, this, SLOT(drainTweets()));
	}
	isActive=active;
	if (active)
		handleScrollbarValueChanged(0);
}

void TweetListWidget::toggleMute() {
	muted=!muted;
}

QString TweetListWidget::deHTML(QString text) {
	long otherPos=0;
	QRegExp deLt("<[^>]*>");
	while ((otherPos=deLt.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+text.mid(otherPos+deLt.matchedLength(),-1);
	return text;

}

int TweetListWidget::getWidgetCount(bool tweetsOnly) {
	int count=ui.tweets->layout()->count();
	if (!tweetsOnly)
		return (count<0)?0:count;
	int x,y;
	for (x=0,y=0;x<count;x++) {
		if (((AbstractTweetWidget*)ui.tweets->layout()->itemAt(x)->widget())->type==0)
			y++;
	}
	return (y<0)?0:y;
}

int TweetListWidget::getUnreadWidgetCount() {
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int x,y=ui.tweets->layout()->count();
	for (x=0;y>0;y--) {
		if (((AbstractTweetWidget*)layout->itemAt(y-1)->widget())->isNew())
			x++;
	}
	return (x<0)?0:x;
}

bool TweetListWidget::lastUnreadIsVisible() {
	AbstractTweetWidget *lastw=lastUnreadWidget();
	QWidget *win=window();

	if (!lastw || !win || (win->windowState()&Qt::WindowMinimized) || !win->isActiveWindow())
		return false;

	int top=lastw->pos().y();
	int bottom=top+lastw->height();
	QRect rect = ui.scrollArea->viewport()->rect();
	int scrollTop = ui.scrollArea->verticalScrollBar()->value();
	int wtop=rect.topLeft().y();
	int wbottom=rect.bottomLeft().y();
	wtop+=scrollTop;
	wbottom+=scrollTop;


/*
	if (wtop>0)
		QMessageBox::information(0,"",QString::number(top)+":"+QString::number(bottom)+" "+QString::number(wtop)+":"+QString::number(wbottom));
*/

	return (bottom<=wbottom && top>=wtop);
}

AbstractTweetWidget* TweetListWidget::lastUnreadWidget() {
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int count=getWidgetCount();
	AbstractTweetWidget *lastw=0;

	for(int x=0;x<count;x++) {
		AbstractTweetWidget *w=(AbstractTweetWidget*)layout->itemAt(x)->widget();
		if (w->isNew())
			lastw=w;
	}
	return lastw;
}

void TweetListWidget::expireNewIndicator() {
	QSettings opts;
	if (isVisible()&&!inRefresh&&lastUnreadIsVisible() && opts.value("automarkread",QVariant(true)).toBool()) {
	//	QMessageBox::information(0,"","f");
		AbstractTweetWidget *lastw=lastUnreadWidget();
		lastw->clearNew();
		setTweetCount();
		clearNewTimer->start(200);
	} else
		clearNewTimer->stop();
}

void TweetListWidget::handleScrollbarValueChanged(int) {
	QSettings opts;
	if (!inRefresh) {
//		if (!clearNewTimer->isActive())
		clearNewTimer->start(opts.value("automarkreadtime",5000).toInt());
//		expireNewIndicator();
//			QMessageBox::information(0,"","f");
//		QTimer::singleShot(400,this,SLOT(expireNewIndicator()));
	}
}

void TweetListWidget::refreshTweet(Tweet *t) {
	QMutexLocker locker(&knownTweetsLock);

	AbstractTweetWidget *w=knownTweets.value(t);
	if (w)
		w->refresh();
}

void TweetListWidget::markAllAsRead() {
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int count=getWidgetCount();

	for(int x=0;x<count;x++) {
		TweetWidget *w=(TweetWidget*)layout->itemAt(x)->widget();
		if (w->isNew())
			w->clearNew();
	}
//	setStyleSheet(styleSheet());
	setTweetCount();
	clearNewTimer->stop();
}

void TweetListWidget::handlePopout() {
	emit popout(this);
}

void TweetListWidget::setPopoutMode(bool popout) {
	QBoxLayout *layout = (QBoxLayout*)ui.tweets->layout();
	int count=getWidgetCount();
	popoutMode=popout;

	if (popout) {
		hideTopText();
		ui.title->hide();
		ui.closeButton->hide();
		ui.count->hide();
		ui.filterButton->hide();
		ui.muteButton->hide();
		ui.popoutButton->hide();
		ui.type->hide();
		ui.mover->hide();
		ui.pruneButton->hide();
		ui.refreshButton->hide();
		popoutWidth=maximumWidth();
		setMaximumWidth(100000);
		handleScrollbarValueChanged(0);
	} else {
		ui.title->show();
		ui.closeButton->show();
		ui.count->show();
		ui.filterButton->show();
		ui.muteButton->show();
		ui.popoutButton->show();
		ui.type->show();
		ui.mover->show();
		ui.pruneButton->show();
		ui.refreshButton->show();
		setMaximumWidth(popoutWidth);
	}
	for(int x=0;x<count;x++) {
		TweetWidget *w=(TweetWidget*)layout->itemAt(x)->widget();
		w->setCompactMode(popout);
	}
}

QString TweetListWidget::title() {
	return ui.title->text();
}

void TweetListWidget::setTitle(QString title) {
	ui.title->setText(title);
	emit titleChanged(title);
}

void TweetListWidget::mouseMoveEvent ( QMouseEvent * event ) {
	QPoint pos = event->pos();
	if (otherDrag)
		return;
	if (!doingResize && !doingDrag) {
		if ((pos.x()>=ui.mover->x() && pos.x()<=(ui.mover->x()+ui.mover->width()))
			&& (pos.y()>=ui.mover->y() && pos.y()<=(ui.mover->y()+ui.mover->height())
		)) {
			//QMessageBox::information(0,"","f");
			doingResize=true;
		} else 	if ((pos.x()>=ui.title->x() && pos.x()<=(ui.title->x()+ui.title->width()))) {
			doingDrag=true;
			fillerTimestamp = QDateTime::currentDateTime().toTime_t();
			QBoxLayout *l = (QBoxLayout*)this->parentWidget()->layout();
			int x = l->indexOf(this);
			if (x>-1) {
				if (filler)
					filler->deleteLater();
				filler = new TweetListWidgetSpacer(tweets,images);
				filler->setObjectName("Filler");
				filler->setMinimumWidth(this->width());
				l->insertWidget(x,filler);
				l->removeWidget(this);
				l->update();
				this->raise();
				xOff=pos.x();
				yOff=pos.y();
			}
			pos = this->parentWidget()->mapFromGlobal(QCursor::pos());
			this->move(pos.x()-xOff,pos.y()-yOff);
		} else {
			otherDrag=true;
		}
	} else {
		if (doingResize) {
			int w = event->x();
			if (w<255)
				w=255;
			else if (w>1000)
				w=1000;
			setMaximumWidth(w);
			setMinimumWidth(w);
		} else {
			unsigned int time = QDateTime::currentDateTime().toTime_t();
			QBoxLayout *l = (QBoxLayout*)this->parentWidget()->layout();
			pos = this->parentWidget()->mapFromGlobal(QCursor::pos());
			this->move(pos.x()-xOff,pos.y()-yOff);
//			if ((fillerTimestamp-time) > 100) {
				for (int x=0;x<l->count();x++) {
					QWidget *w = l->itemAt(x)->widget();
					if (w->x()<=pos.x() && (w->x()+w->width())>=pos.x()) {
						if (w!=filler) {
//							if (pos.x()-w->x()>(w->width()/2))
//								x++;
//							int y = l->indexOf(filler);
//							l->takeAt(y);
//							if (x<y) {
//								l->insertWidget(x,filler);
//							} else {
								l->insertWidget(x,filler);
//							}
							l->update();
							fillerTimestamp=time;
						}
						break;
					}
				}
//			}
		}
	}
	event->accept();
}

void TweetListWidget::mousePressEvent ( QMouseEvent * event ) {
	QPoint pos = event->pos();
	if ((pos.x()>=ui.mover->x() && pos.x()<=(ui.mover->x()+ui.mover->width()))) {
		//QMessageBox::information(0,"","f");
		doingResize=true;
		ui.scrollArea->setVisible(false);
		ui.resizespacer->changeSize(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
		event->accept();
	} else 	if ((pos.x()>=ui.title->x() && pos.x()<=(ui.title->x()+ui.title->width()))) {
	}

}

void TweetListWidget::mouseReleaseEvent ( QMouseEvent *  ) {
	otherDrag=doingResize=false;
	if (doingDrag) {
		QBoxLayout *l = (QBoxLayout*)this->parentWidget()->layout();
		if (filler) {
			int x = l->indexOf(filler);
			if (x>-1) {
				l->insertWidget(x,this);
				l->removeWidget(filler);
				delete filler;
				filler=0;
			}
		}
		doingDrag=false;
	}
	ui.resizespacer->changeSize(0,0,QSizePolicy::Fixed,QSizePolicy::Fixed);
	ui.scrollArea->setVisible(true);
}

void TweetListWidget::slowDelete() {
	QVBoxLayout *l = (QVBoxLayout*)ui.tweets->layout();
	AbstractTweetWidget *w;

	this->hide();
	if (l->count()) {
		w = (AbstractTweetWidget*)l->itemAt(0)->widget();
		l->removeWidget(w);
		w->deleteLater();
		QTimer::singleShot(0,this,SLOT(slowDelete()));
	} else {
		this->setAttribute(Qt::WA_DeleteOnClose);
		this->close();
	}
}

void TweetListWidget::setAccountIcon() {
	YasstApplication *app = (YasstApplication*)qApp;

	if (app->getAccountsCount()>1 && tweets->getAuthenticatingUserScreenName()!="") {
		TweetUser *u = tweets->getUserByScreenName(tweets->getAuthenticatingUserScreenName());
		if (u) {
			if (images->contains(u->image_url))
				ui.account->setPixmap(images->getPixMap(u->image_url));
			else
				QTimer::singleShot(2000,this,SLOT(setAccountIcon()));
		} else
			QTimer::singleShot(2000,this,SLOT(setAccountIcon()));
		ui.account->show();
	} else
		ui.account->hide();
}

bool TweetListWidget::hasPendingTweets() {
	QMutexLocker tweetsLocker(&tweetsLock);
	return (pendingTweets.size()>0);
}

void TweetListWidget::handlePruneRead() {
	pruneRead(true);
}

void TweetListWidget::handlePruneReadSingle() {
	pruneRead(false);
}

void TweetListWidget::pruneRead(bool all) {
	QMutexLocker locker(&knownTweetsLock);
	QList<Tweet*> keys = knownTweets.keys();
	for (int x=0;x<keys.size();x++) {
		Tweet *t = keys.at(x);
		if (t->read) {
			if (all)
				emit removeTweet(t);
			else
				handleDeleteTweet(t);
			t->deleted=true;
		}
	}
	havePrunedTweets=true;
}

void TweetListWidget::removeUnwantedTweets() {
	QMutexLocker locker(&knownTweetsLock);
	QVBoxLayout *l = (QVBoxLayout*)ui.tweets->layout();
	QList<Tweet*> keys = knownTweets.keys();

	for (int x=0;x<keys.size();x++) {
		Tweet *t = keys.at(x);
		if (! ((canDoUpdate(t)||inReplyToTweets.contains(t->id))&&!t->deleted) ) {
			AbstractTweetWidget *w = knownTweets.take(t);
			l->removeWidget(w);
			w->deleteLater();
		}
	}
}

QString TweetListWidget::getName() {
	return ui.title->text();
}

QPixmap TweetListWidget::getIcon() {
	return ui.type->pixmap()->copy();
}

bool TweetListWidget::isMuted() {
	return muted;
}
