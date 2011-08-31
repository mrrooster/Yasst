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
/* $Id: TweetPanel.cpp,v 1.34 2010/10/05 18:08:51 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "TweetPanel.h"
// Qt
#include <QMessageBox>
#include <QPalette>
#include <QSettings>
#include <QFileDialog>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QToolButton>
#include <QMap>
#include <QList>
#include <QTextCursor>
// Backend
#include "backend/HTTPHandler.h"
#include "backend/FeedItem.h"
// Frontend
#include "YasstApplication.h"
#include "AutocompleteUserlistWidget.h"
#include "AutocompleteHashlistWidget.h"

TweetPanel::TweetPanel(ImageCache *c ,QString user, QWidget *parent ) : QWidget(parent),tweets(0),prevtweets(0),autocompleter(0) {
	YasstApplication *app = (YasstApplication*)qApp;
	ui.setupUi(this);
	imgcache=c;
	QObject::connect(app,SIGNAL(accountsChanged(int)),this,SLOT(handleAccountsChanged(int)));
	QObject::connect(imgcache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)),Qt::QueuedConnection);
	QObject::connect(ui.tweet,SIGNAL(clicked()),this,SLOT(tweetClicked()));
	QObject::connect(ui.shorten,SIGNAL(clicked()),this,SLOT(shortenClicked()));
	QObject::connect(ui.message,SIGNAL(returnPressed()),this,SLOT(returnPressed()));
	QObject::connect(ui.message,SIGNAL(escapePressed()),this,SLOT(escapePressed()));
	QObject::connect(ui.message,SIGNAL(textChanged()),this,SLOT(textChanged()));
	QObject::connect(ui.message,SIGNAL(urlPasted(QString)),this,SLOT(handlePastedUrl(QString)));
	QObject::connect(ui.imageupload,SIGNAL(clicked()),this,SLOT(handleImageUpload()));
	QObject::connect(&shortener,SIGNAL(shortURL(QString,QString)),this,SLOT(handleShortUrl(QString,QString)));
	replyWidget=0;
	this->user=user;
	highlighter=new TweetTextHighlighter(ui.message->document());
	cancelReply();
	textChanged();
	uploadImageFilename="";

	QWidget *w=new QWidget(this);
	ui.tweetlayout->insertWidget(1,w);
	w->hide();
	w->setLayout(&imageUploadLayout);
	w->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
	imageUploadLayout.setStackingMode(QStackedLayout::StackAll);
	imageUploadLayout.insertWidget(0,&cancelImage);
	imageUploadLayout.insertWidget(1,&image);
	cancelImage.setIcon(QIcon(":/buttons/close.png"));
	cancelImage.setFixedSize(15,15);
	cancelImage.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	cancelImage.setStyleSheet("background:none; border:none;");
	image.setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
	QObject::connect(&cancelImage,SIGNAL(clicked()),this,SLOT(cancelUploadImage()));
	setAcceptDrops(true);

	toolBar = new QToolBar(this);
	ui.toolbar->addWidget(toolBar);

	QObject::connect(app,SIGNAL(accountAdded(BirdBox*)),this,SLOT(addAccount(BirdBox*)));
	QObject::connect(app,SIGNAL(accountRemoved(BirdBox*)),this,SLOT(removeAccount(BirdBox*)));
	QList<BirdBox*> accts = app->getAccounts();
	for(int x=0;x<accts.length();x++)
		addAccount(accts.at(x));

	rssAccount=0;
	handleAccountsChanged(app->getAccountsCount());

	// Translate
	QObject::connect(&translateToMenu,SIGNAL(aboutToShow()),this,SLOT(handleShowTranslateToMenu()));
	ui.translate->setMenu(&translateToMenu);
	translator = new Translator();

	QMap<QString,QString> lang = translator->getLanguages();
	QList<QString> keys = lang.keys();
	allLanguagesTo.clear();
	allLanguagesTo.setTitle(tr("All languages"));

	int x=0;
	for(x=0;x<keys.size();x++) {
		QString code = keys.at(x);
		QAction *act = allLanguagesTo.addAction(lang.value(code));
		act->setData(code);
		QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleTranslateTo()));
	}
	QObject::connect(translator,SIGNAL(haveTranslation(int,QString,QString,QString)),this,SLOT(handleTranslateResult(int,QString,QString,QString)));
	// Translate end
}

TweetPanel::~TweetPanel()
{
	delete translator;
}

void TweetPanel::shortenClicked() {
	for(int x=0;x<urlsToShorten.size();x++) {
		shortener.shortenURL(urlsToShorten.at(x));
	}
}

void TweetPanel::handleShortUrl(QString longUrl,QString shortUrl) {
	QString message = ui.message->toPlainText();
	int i;

//	QMessageBox::information (0,"assdf",longUrl);
	while((i=message.indexOf(longUrl))>-1) {
		message = message.left(i)+shortUrl+message.right(message.size()-i-longUrl.size());
	}
	ui.message->setPlainText(message);
}

void TweetPanel::returnPressed() {
	QSettings opts;
	if (autocompleter) {
		QString text = autocompleter->getCurrentEntry();
		if (text.size()>0) {
			QTextCursor cursor = ui.message->textCursor();
			cursor.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
			if (autocompleter->type==1)
				cursor.insertText("@"+text+" ");
			else
				cursor.insertText("#"+text+" ");
		}
		autocompleter->deleteLater();
		autocompleter=0;
	} else {
		if (opts.value("returnSends",QVariant(true)).toBool()) {
			QString msg = ui.message->toPlainText();
			if (msg.size()>0) {
	//			ui.message->undo();
			}
			tweetClicked();
		}
	}
}

void TweetPanel::escapePressed() {
	if (autocompleter) {
		autocompleter->deleteLater();
		autocompleter=0;
	} else {
		QTextCursor cursor = ui.message->textCursor();
		cursor.select(QTextCursor::Document);
		cursor.removeSelectedText();
		cancelUploadImage();
	}
}

void TweetPanel::textChanged() {
	long size = HTTPHandler::htmlEncode(ui.message->toPlainText()).size();
	long count = 140-(size);
	long urlCount = 0;
	QPalette pal=this->palette();
	bool directMessage=(ui.message->toPlainText().startsWith("D ",Qt::CaseInsensitive));
	QSettings opts;

	if (count==140 && replyWidget) {
		cancelReply();
	}
	if (count==140 && prevtweets) {
		setAccount(prevtweets);
		prevtweets=0;
	}

	if (directMessage) {
		int i=ui.message->toPlainText().mid(2).indexOf(' ');
		if (i) {
			highlighter->setDirectMessagePrefix(3+i);
			count += 3+i;
		}
	} else
		highlighter->setDirectMessagePrefix(0);

	QString style="";
	//QMessageBox::information (0,"assdf",style);
	//background-color: qconicalgradient(cx:0, cy:0, angle:90, stop:0 rgba(189, 189, 189, 255), stop:1 rgba(255, 246, 227, 255));
	//background-color: qconicalgradient(cx:0, cy:0, angle:90, stop:0 rgba(140, 212, 181, 255), stop:1 rgba(255, 255, 255, 255));
	int r=140;
	int g=212;
	int b=181;
	if (directMessage) {
		r=92;
		g=135;
		b=238;
		ui.message->setProperty("tweet", "direct");
	} else
		ui.message->setProperty("tweet",0);
	if (count<0) {
		style="background-color: black; color: white;";
	} else if (count <= 30) {
		int mycount=(count*100)/30;
	//	QMessageBox::information (0,"assdf",QString::number(mycount));
//		style="background-color: qconicalgradient(cx:0, cy:0, angle:90, stop:0 rgba(189, 189, 189, 255), stop:1 rgba(255, "+QString::number(246*mycount/100)+", "+QString::number(10+(217*mycount/100))+", 255));";
		if (mycount<=50)
			r+=(115*(((100-mycount)-50)*2)/100);
		g=10+((g-10)*mycount/100);
		b=10+((b-10)*mycount/100);
		style="background-color: qconicalgradient(cx:0, cy:0, angle:90, stop:0 rgba("+QString::number(r)+", "+QString::number(g)+", "+QString::number(b)+", 255), stop:1 rgba(255, 255, 255, 255));";
	}
	ui.message->setStyleSheet(style);
	ui.lettercount->setText(QString::number(count));
	ui.tweet->setVisible((size!=0)&&((YasstApplication*)qApp)->getAccounts().size()>0);
	ui.translate->setVisible(opts.value("tweettranslatebutton",QVariant(false)).toBool()&&(size!=0)&&((YasstApplication*)qApp)->getAccounts().size()>0);


	urlsToShorten.clear();
	for (int x=0;x<ui.message->blockCount();x++) {
		TweetTextHighlighter::urlData *urlData = (TweetTextHighlighter::urlData*)ui.message->document()->findBlockByNumber(x).userData();
		if (urlData && urlData->urls.size()>0) {
			urlCount+=urlData->urls.size();
			for (int y=0;y<urlData->urls.size();y++)
				urlsToShorten.append(urlData->urls.value(y));
		}
	}
	ui.shorten->setVisible((urlCount!=0));
	// autocomplete
	QTextCursor cursor = ui.message->textCursor();
	if (autocompleter) {
		QString text = ui.message->toPlainText();
		QString part = "";
		int pos;
		for(pos = cursor.position()-1;pos>=0 && text.at(pos)!='@' && text.at(pos)!='#';pos--) {
			part = text.at(pos)+part;
		}
		if (text.size()==0||
				part.contains(' ')||
				(
				text.at((pos<0)?0:pos)!='@'&&
				text.at((pos<0)?0:pos)!='#'
				)) {
			autocompleter->deleteLater();
			autocompleter=0;
		} else
			autocompleter->setMatchText(part);
	} else {
		cursor.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
		QString text = cursor.selectedText();
		if (text.size()>0) {
			if (text.at(0)=='@') {
				autocompleter = new AutocompleteUserlistWidget(tweets,ui.message,this);
			} else if (text.at(0)=='#') {
				autocompleter = new AutocompleteHashlistWidget(tweets,ui.message,this);
			}
			if (autocompleter) {
				QObject::connect(autocompleter,SIGNAL(exactMatch()),this,SLOT(escapePressed()));
				QObject::connect(autocompleter,SIGNAL(noMatch()),this,SLOT(escapePressed()));
				QTextCursor currentCursor = ui.message->textCursor();
				cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor);
				ui.message->setTextCursor(cursor);
				QRect r = ui.message->cursorRect();
				int x = r.left();
				int y = r.bottom()+5;
				QWidget *w=ui.message;
				while(w&&w!=this) {
					x += w->pos().x();
					y += w->pos().y();
					w=w->parentWidget();
				}
				ui.message->setTextCursor(currentCursor);
				autocompleter->move(x,y);
				autocompleter->show();
				autocompleter->raise();
				autocompleter->setMatchText(text.mid(1));
			}
		}
	}
	// autocomplete end
}

void TweetPanel::cancelReply() {
	ui.tweetbox->removeWidget(replyWidget);
	QObject::disconnect(this,SLOT(cancelReply()));
	delete replyWidget;
	replyWidget=0;
	replyId="";
	ui.message->setPlainText("");
	ui.fixreplies->hide();
}

void TweetPanel::tweetClicked() {
	QString message = HTTPHandler::htmlEncode(ui.message->toPlainText());
	QString inReplyTo = "";
	QSettings opts;
	int i=0;
	bool directMessage=(message.startsWith("D ",Qt::CaseInsensitive));
	BirdBox *tweets = this->tweets;

	if (message=="")
		return;

	if (directMessage) {
		message=message.mid(2);
		i=message.indexOf(' ');
		if (i==-1)
			return;
	}

	if (replyWidget && !ui.fixreplies->isChecked()) {
		inReplyTo = replyWidget->getTweet()->id;
	}
	ui.message->setPlainText("");
	if (opts.value("delaySending",QVariant(false)).toBool()) {
		Tweet *t = new Tweet();
		QString user = tweets->getAuthenticatingUserScreenName();
		TweetUser *u = tweets->getUserByScreenName(user);
		if (directMessage) {
			t->in_reply_to_screen_name=message.left(i);
			message=message.mid(i+1);
		}
		t->message=message;
		t->direct_message=directMessage;
		t->in_reply_to_status_id=inReplyTo;
		t->screen_name=user;
		t->image_url=uploadImageFilename;
		if (u) {
			t->user_id=u->id;
		}
		TweetWidget *w=new TweetWidget(t,tweets,imgcache);
		w->setSendMode();
		QObject::connect(w,SIGNAL(timerCancel(TweetWidget*)),this,SLOT(cancelSend(TweetWidget*)));
		QObject::connect(w,SIGNAL(timerComplete(TweetWidget*)),this,SLOT(doSend(TweetWidget*)));
		ui.tweetbox->insertWidget(0,w);
	} else {
		if (directMessage) {
			inReplyTo=message.left(i);
			message=message.mid(i+1);
			emit twitterDirectMessage(tweets,message,inReplyTo,uploadImageFilename);
		} else
			emit twitter(tweets,message,inReplyTo,uploadImageFilename);
	}
	cancelUploadImage();
}

void TweetPanel::cancelSend(TweetWidget *w) {
	Tweet *t = w->getTweet();
	QString message = HTTPHandler::htmlDecode(t->message);
	if (t->direct_message)
		message = tr("D ")+t->in_reply_to_screen_name+tr(" ")+message;
	ui.tweetbox->removeWidget(w);
	ui.message->setPlainText(message);
	ui.message->setFocus();
	if (t->image_url!="")
		setUploadImage(t->image_url);
	delete w;
	if (t->in_reply_to_status_id!="") {
		Tweet *r = tweets->getTweet(t->in_reply_to_status_id);
		if (r && !replyWidget) {
			TweetUser *user = tweets->getUser(r->user_id);
			QString name=r->user_id;
			replyWidget=new TweetWidget(r,tweets,imgcache);
			replyWidget->setReplyMode();
			QObject::connect(replyWidget,SIGNAL(cancel()),this,SLOT(cancelReply()));
			replyId=r->id;
			ui.tweetbox->insertWidget(0,replyWidget);
			if (user)
				name=user->screen_name;
		}
	}
	delete t;
}

void TweetPanel::doSend(TweetWidget *w) {
	Tweet *t = w->getTweet();
	BirdBox *tweets = w->getAccount();
	if (!tweets)
		tweets = this->tweets;
	ui.tweetbox->removeWidget(w);
	delete w;
	if (t->direct_message) {
		emit twitterDirectMessage(tweets,t->message,t->in_reply_to_screen_name,t->image_url);
	} else
		emit twitter(tweets,t->message,t->in_reply_to_status_id,t->image_url);
	delete t;
}

void TweetPanel::retweet(Tweet *t,bool newStyle) {
	BirdBox *tweets = qobject_cast<BirdBox*>(sender());
	if (newStyle)
		return;
	if (t->type<2) {
		QString name=t->screen_name;

		cancelReply();
		ui.message->setPlainText("RT @"+name+": "+t->message);
	} else if (t->type==2) {
		FeedItem *item = (FeedItem*)t;
		ui.message->setPlainText(item->url+" "+item->message);
		shortener.shortenURL(item->url);
	}
	this->parentWidget()->show();
	ui.message->setFocus();
	if (tweets==this->rssAccount)
		return;// Retweet with the currently selected account.
	if (!prevtweets)
		prevtweets=this->tweets;
	setAccount(tweets);
}

void TweetPanel::directMessage(Tweet *t) {
	BirdBox *tweets = qobject_cast<BirdBox*>(sender());
	QString name=t->screen_name;

	cancelReply();
	ui.message->setPlainText("D "+name+" ");
	this->parentWidget()->show();
	ui.message->setFocus();
	if (!prevtweets)
		prevtweets=this->tweets;
	setAccount(tweets);
}

void TweetPanel::reply(Tweet *t) {
	BirdBox *tweets = qobject_cast<BirdBox*>(sender());
	QString name=t->screen_name;
	if (replyWidget) {
		ui.message->insertPlainText("@"+name+" ");
	} else {
		cancelReply();
		if (!replyWidget) {
			QSettings opts;
			replyWidget=new TweetWidget(t,tweets,imgcache);
			replyWidget->setReplyMode();
			QObject::connect(replyWidget,SIGNAL(cancel()),this,SLOT(cancelReply()));
			replyId=t->id;
			ui.tweetbox->insertWidget(0,replyWidget);
			ui.fixreplies->show();
			ui.fixreplies->setChecked(opts.value("fixreplies",QVariant(false)).toBool());
		}
		ui.message->setPlainText("@"+name+" ");
		ui.message->moveCursor(QTextCursor::End);
	}
	ui.message->setFocus();
	this->parentWidget()->show();
	if (!prevtweets)
		prevtweets=this->tweets;
	setAccount(tweets);
}

void TweetPanel::handlePastedUrl(QString ) {

}

void TweetPanel::handleImageUpload() {
	QString file = QFileDialog::getOpenFileName(this,tr("Open piccy"), "", tr("Beautiful pictures")+" (*.jpg *.png *.gif);;"+tr("All files")+" (*.*)");
	if (file!="") {
		setUploadImage(file);
	}
}

void TweetPanel::setUploadImage(QString filename) {
	QPixmap pixmap=QPixmap(filename).scaledToHeight(ui.message->height());
	image.setPixmap(pixmap);
	uploadImageFilename=filename;
	ui.tweetlayout->itemAt(1)->widget()->show();
}

void TweetPanel::cancelUploadImage() {
	ui.tweetlayout->itemAt(1)->widget()->hide();
	uploadImageFilename="";
	image.setPixmap(0);
}

void TweetPanel::handleFailedTweet(QString message,QString replyId,QString image) {
	BirdBox *tweets = qobject_cast<BirdBox*>(sender());
	Tweet *t = new Tweet();
	QString user = tweets->getAuthenticatingUserScreenName();
	TweetUser *u = tweets->getUserByScreenName(user);
	t->message=message;
	t->direct_message=false;
	t->in_reply_to_status_id=replyId;
	t->screen_name=user;
	t->image_url=image;
	if (u) {
		t->user_id=u->id;
	}
	TweetWidget *w=new TweetWidget(t,tweets,imgcache);
	w->setResendMode();
	QObject::connect(w,SIGNAL(timerCancel(TweetWidget*)),this,SLOT(cancelSend(TweetWidget*)));
	QObject::connect(w,SIGNAL(timerComplete(TweetWidget*)),this,SLOT(doSend(TweetWidget*)));
	ui.tweetbox->insertWidget(0,w);

}

void TweetPanel::handleFailedDirectMessage(QString message,QString userId) {
	BirdBox *tweets = qobject_cast<BirdBox*>(sender());
	Tweet *t = new Tweet();
	TweetUser *u = tweets->getUserByScreenName(user);
	t->in_reply_to_screen_name=userId;
	t->message=message;
	t->direct_message=true;
	t->in_reply_to_status_id="";
	t->screen_name=user;
	t->image_url="";
	if (u) {
		t->user_id=u->id;
	}
	TweetWidget *w=new TweetWidget(t,tweets,imgcache);
	w->setResendMode();
	QObject::connect(w,SIGNAL(timerCancel(TweetWidget*)),this,SLOT(cancelSend(TweetWidget*)));
	QObject::connect(w,SIGNAL(timerComplete(TweetWidget*)),this,SLOT(doSend(TweetWidget*)));
	ui.tweetbox->insertWidget(0,w);
}

void TweetPanel::dragEnterEvent(QDragEnterEvent *event) {
//	for(int x=0;x<event->mimeData()->formats().size();x++)

    if (event->mimeData()->hasFormat("text/uri-list")) {
//    	QMessageBox::information(this,"","_"+event->mimeData()->data("text/uri-list")+"_");
    	QString filename = event->mimeData()->data("text/uri-list").left(event->mimeData()->data("text/uri-list").indexOf('\r'));
//    	QMessageBox::information(this,"","_"+filename+"_");

    	if (filename.endsWith(tr(".jpg"),Qt::CaseInsensitive)
    			|| filename.endsWith(".jpeg",Qt::CaseInsensitive)
				|| filename.endsWith(".gif",Qt::CaseInsensitive)
				|| filename.endsWith(".bmp",Qt::CaseInsensitive)
    			|| filename.endsWith(".png",Qt::CaseInsensitive)
    			)
    		event->acceptProposedAction();
    }

}

void TweetPanel::dropEvent(QDropEvent *event) {
	event->acceptProposedAction();
	QString filename = event->mimeData()->data("text/uri-list").left(event->mimeData()->data("text/uri-list").indexOf('\r'));
//	QMessageBox::information(this,"","_"+filename+"_");
	setUploadImage(QUrl(filename).toLocalFile());
}

void TweetPanel::handleAccountsChanged(int c) {
	bool stillRefresh=false;
	toolBar->clear();
	activeIcon.clear();
	inactiveIcon.clear();

	if (c>0) {
		QList<BirdBox*> list = ((YasstApplication*)qApp)->getAccounts();
		if (!tweets)
			setAccount(list.at(0));

		for (int x=0;x<c;x++) {
			BirdBox *b = list.at(x);
			QAction *act = toolBar->addAction(b->getAuthenticatingUserScreenName());
			TweetUser *u = b->getUserByScreenName(b->getAuthenticatingUserScreenName());
			QToolButton *w = (QToolButton*)toolBar->widgetForAction(act);
			w->setCheckable(true);
			w->setMaximumSize(24,24);

			QString url = "";
			if (u) {
				url = u->image_url;
			} else
				stillRefresh=true;
			act->setData(QVariant(x));
			act->setIcon(QIcon(imgcache->getPixMap(url)));
			act->setCheckable(true);
			act->setChecked(tweets==b);
			QObject::connect(act,SIGNAL(triggered()),this,SLOT(handleAccountChange()));
//			w->setDisabled(true);
		}
	}
	if (c<=1) {
		toolBar->hide();
		ui.avatar->hide();
		ui.screenname->hide();
	} else {
		toolBar->show();
		ui.avatar->show();
		ui.screenname->show();
	}
	if (stillRefresh)
		QTimer::singleShot(2000,this,SLOT(refreshToolbar()));
}

void TweetPanel::handleAccountChange() {
    QAction *act = qobject_cast<QAction *>(sender());
    int index = act->data().toInt();
	QList<BirdBox*> list = ((YasstApplication*)qApp)->getAccounts();


	emit accountChanged(list.at(index));
	QList<QAction*> accts = toolBar->actions();
	for(int x=0;x<accts.count();x++) {
		QAction *anAct = accts.at(x);
		anAct->setChecked((anAct==act));
	}
}

void TweetPanel::addAccount(BirdBox *b) {
	QObject::connect(b,SIGNAL(retweet(Tweet *,bool)),this,SLOT(retweet(Tweet *,bool)));
	QObject::connect(b,SIGNAL(reply(Tweet *)),this,SLOT(reply(Tweet *)));
	QObject::connect(b,SIGNAL(directMessage(Tweet *)),this,SLOT(directMessage(Tweet *)));
	QObject::connect(b,SIGNAL(failedDirectMessage(QString,QString)),this,SLOT(handleFailedDirectMessage(QString,QString)));
	QObject::connect(b,SIGNAL(failedTweet(QString,QString,QString)),this,SLOT(handleFailedTweet(QString,QString,QString)));
	QObject::connect(b,SIGNAL(authChanged()),this,SLOT(accountAuthChanged()));
}

void TweetPanel::removeAccount(BirdBox *b) {
	QObject::disconnect(b,SIGNAL(retweet(Tweet *,bool)),this,SLOT(retweet(Tweet *,bool)));
	QObject::disconnect(b,SIGNAL(reply(Tweet *)),this,SLOT(reply(Tweet *)));
	QObject::disconnect(b,SIGNAL(directMessage(Tweet *)),this,SLOT(directMessage(Tweet *)));
	QObject::disconnect(b,SIGNAL(failedDirectMessage(QString,QString)),this,SLOT(handleFailedDirectMessage(QString,QString)));
	QObject::disconnect(b,SIGNAL(failedTweet(QString,QString,QString)),this,SLOT(handleFailedTweet(QString,QString,QString)));
	QObject::disconnect(b,SIGNAL(authChanged()),this,SLOT(accountAuthChanged()));
	if (b==tweets)
		tweets=0;
}

void TweetPanel::setAccount(BirdBox *b) {
	tweets=b;
	TweetUser *u = b->getUserByScreenName(b->getAuthenticatingUserScreenName());

	QString url;
	if (u) {
		url=u->image_url;
	}
	ui.avatar->setPixmap(imgcache->getPixMap(url));
	ui.screenname->setText(b->getAuthenticatingUserScreenName());
//	QMessageBox::information(this,"",b->getAuthenticatingUserScreenName());
//	emit accountChanged(b);
}

void TweetPanel::accountAuthChanged() {
	BirdBox *b = qobject_cast<BirdBox*>(sender());

	if (b==this->tweets) {
		TweetUser *u = b->getUserByScreenName(b->getAuthenticatingUserScreenName());
		QString url;
		if (u) {
			url=u->image_url;
		}
		ui.avatar->setPixmap(imgcache->getPixMap(url));
		ui.screenname->setText(b->getAuthenticatingUserScreenName());
	}
}

void TweetPanel::updateImage(QString url) {
	QList<QAction*> acts = toolBar->actions();
	QList<BirdBox*> list = ((YasstApplication*)qApp)->getAccounts();
	QString me = "";

	if (tweets)
		me = tweets->getAuthenticatingUserScreenName();

	for(int x=0;x<acts.length()&&x<list.length();x++) {
		QAction *act = acts.at(x);

		TweetUser *u = list.at(x)->getUserByScreenName(act->text());
		if (u && u->image_url==url) {
			act->setIcon(QIcon(imgcache->getPixMap(u->image_url)));
			toolBar->widgetForAction(act)->setMaximumSize(24,24);
//				QMessageBox::information(this,u->screen_name,me);
			if (u->screen_name.compare(me,Qt::CaseInsensitive)==0) {
				ui.avatar->setPixmap(imgcache->getPixMap(url));
			}
		}
	}
}

void TweetPanel::refreshToolbar() {
	bool stillRefresh=false;
	QList<QAction*> acts = toolBar->actions();
	QList<BirdBox*> list = ((YasstApplication*)qApp)->getAccounts();

	for(int x=0;x<acts.length()&&x<list.length();x++) {
		QAction *act = acts.at(x);

		TweetUser *u = list.at(x)->getUserByScreenName(act->text());
		if (u) {
			act->setIcon(QIcon(imgcache->getPixMap(u->image_url)));
			toolBar->widgetForAction(act)->setMaximumSize(24,24);
//				QMessageBox::information(this,u->screen_name,me);
		} else
			stillRefresh=true;
	}
	if (stillRefresh)
		QTimer::singleShot(2000,this,SLOT(refreshToolbar()));
}

void TweetPanel::handleShowTranslateToMenu() {
	QMap<QString,QString> lang = translator->getLanguages();
	translateToMenu.clear();
	QList<QString> lastLang = TweetWidget::lastLang;

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

void TweetPanel::handleTranslateTo() {
	QSettings opts;
	QAction *act = qobject_cast<QAction*>(sender());
	QString to = act->data().toString();
	QString from = opts.value("baseLanguage","en").toString();
	QList<QString> lastLang = TweetWidget::lastLang;

	if (from!="" && !lastLang.contains(to))
		TweetWidget::lastLang.insert(0,to);
	if (TweetWidget::lastLang.size()>5)
		TweetWidget::lastLang.removeLast();

	QString text = ui.message->textCursor().selectedText();
	if (text=="")
		text = ui.message->toPlainText();
	translator->translate(from,to,text);
}

void TweetPanel::handleTranslateResult(int,QString,QString,QString resp) {
	if (!ui.message->textCursor().hasSelection())
		ui.message->selectAll();
	ui.message->textCursor().insertText(resp);
}

void TweetPanel::setRssAccount(BirdBox *b) {
	if (b) {
		if (rssAccount)
			removeAccount(rssAccount);
		rssAccount = b;
		addAccount(b);
	}
}
