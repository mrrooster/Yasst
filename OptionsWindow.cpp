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
/* $Id: OptionsWindow.cpp,v 1.32 2010/09/14 13:49:05 ian Exp $
File:   OptionsWindow.cpp
(c) 2009 Ian Clark

*/
#include "OptionsWindow.h"
#include "ya_st.h"
#include "consts.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include "ya_st.h"
#include "backend/BirdBox.h"
#include "YasstApplication.h"
#include "PasswordWidget.h"
#include "backend/fetchers/UserTweetFetcher.h"
#include "backend/fetchers/UserReplyFetcher.h"
#include "backend/fetchers/UserDirectMessageFetcher.h"
#include "backend/fetchers/UserFavoriteMessageFetcher.h"
#include "backend/fetchers/FriendFetcher.h"
#include "backend/fetchers/UserGroupFollowingFetcher.h"
#include "backend/Translator.h"
#include "tweetwidget.h"
/*
 Proxy dropdown:

 0 - None
 1 - SOCKS5
 2 - HTTP
 */

OptionsWindow::OptionsWindow(ImageCache *i,QWidget *parent)
    : QDialog(parent),initialTheme(-1)
{
	images = i;

	ui.setupUi(this);

	mediaObject = new Phonon::MediaObject(this);
	audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	path = Phonon::createPath(mediaObject, audioOutput);

	numValidator=new QIntValidator(1,99999,this);
	ui.minTweets->setValidator(numValidator);
	ui.tweetPruneAge->setValidator(numValidator);
	ui.tabWidget->setCurrentIndex(0);

	QObject::connect(mediaObject,SIGNAL(finished()),this,SLOT(soundEnded()));
	QObject::connect(ui.save,SIGNAL(clicked()),this,SLOT(setOpts()));
	QObject::connect(ui.browse,SIGNAL(clicked()),this,SLOT(setFilePath()));
	QObject::connect(ui.mainbrowse,SIGNAL(clicked()),this,SLOT(setUserMainStylesheetPath()));
	QObject::connect(ui.textbrowse,SIGNAL(clicked()),this,SLOT(setUserTextStylesheetPath()));
	QObject::connect(ui.reloadstylesheet,SIGNAL(clicked()),this,SLOT(handleReloadUserStylesheet()));
	QObject::connect(ui.clear,SIGNAL(clicked()),this,SLOT(clearFilePath()));
	QObject::connect(ui.test,SIGNAL(clicked()),this,SLOT(testSound()));
	QObject::connect(ui.theme,SIGNAL(currentRowChanged(int)),this,SLOT(handleThemeChange(int)));
	QObject::connect(ui.twitteraccounts,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(handleAccountClicked(QListWidgetItem*)));
	QObject::connect(ui.twitterUser,SIGNAL(textChanged(QString)),this,SLOT(twitterUserChanged(QString)));
	QObject::connect(ui.clearoauth,SIGNAL(clicked()),this,SLOT(handleClearOAuth()));
	QObject::connect(ui.clearlang,SIGNAL(clicked()),this,SLOT(handleClearLang()));
	QObject::connect(ui.resetsettings,SIGNAL(clicked()),this,SLOT(handleResetSettings()));
	QObject::connect(ui.removeallsettings,SIGNAL(clicked()),this,SLOT(handleRemoveAllSettings()));
	QObject::connect(ui.minrefreshtime,SIGNAL(sliderMoved(int)),this,SLOT(handleMinRefreshSliderMove(int)));

	mainWindow=(Ya_st*)parent;

	setObjectName("optionswindow");

	// Populate languages
	QMap<QString,QString> lang = Translator::getLanguages();
	QList<QString> keys = lang.keys();

	for(int x=0;x<keys.size();x++) {
		ui.lang->addItem(lang.value(keys.at(x)),keys.at(x));
	}

//	QMessageBox::information (0,"assdf",tr("<style>")+Ya_st::labelCSS+tr("</style><p>You can set a default filter that will apply to all tweet columns, for example you could filter out all occurances of a particular hash tag, or all messages sent from a particular client.</p><p>To show tweets with <b class=\"hi\">certain words</b> type <b class='type'>certain words</b>. To hide other salacious terms use -salacious.</p><p>For users use @Fred, and -@Barny.</p><p>For clients use @@yasst, and -@@Spambot.</p>"));
}

OptionsWindow::~OptionsWindow()
{
	QObject::disconnect(mediaObject,SIGNAL(finished()),this,SLOT(soundEnded()));
	QObject::disconnect(ui.save,SIGNAL(clicked()),this,SLOT(setOpts()));
	QObject::disconnect(ui.browse,SIGNAL(clicked()),this,SLOT(setFilePath()));
	QObject::disconnect(ui.clear,SIGNAL(clicked()),this,SLOT(clearFilePath()));
	QObject::disconnect(ui.test,SIGNAL(clicked()),this,SLOT(testSound()));
	delete numValidator;
	delete audioOutput;
	delete mediaObject;
}

void OptionsWindow::showOpts() {
	QSettings opts;

	ui.senddelay->setChecked(opts.value("delaySending",QVariant(false)).toBool());
	ui.returnsends->setChecked(opts.value("returnSends",QVariant(true)).toBool());
	ui.dansannoyingscrollything->setChecked(opts.value("dansannoyingscrollything",QVariant(false)).toBool());
	ui.activatewindow->setChecked(opts.value("activatewindow",QVariant(false)).toBool());
	ui.trayicon->setChecked(opts.value("trayicon",QVariant(false)).toBool());
	ui.fixreplies->setChecked(opts.value("fixreplies",QVariant(false)).toBool());
	ui.proxyType->setCurrentIndex(opts.value("proxyType",QVariant(0)).toInt());
	ui.proxyHost->setText(opts.value("proxyHost","").toString());
	ui.proxyPort->setText(opts.value("proxyPort","").toString());
	ui.proxyUser->setText(opts.value("proxyUser","").toString());
	ui.proxyPass->setText(opts.value("proxyPass","").toString());
	ui.alertsound->setText(opts.value("sounds/newTweet","").toString());
	ui.newtweetalert->setChecked(opts.value("playnewtweetalert",QVariant(true)).toBool());
	ui.globalFilter->setText(opts.value("globalfiltertext","").toString());

	ui.bitlyUser->setText(opts.value("accounts/bitly/user","").toString());
	ui.bitlyAPI->setText(opts.value("accounts/bitly/APIkey","").toString());

	ui.tweetsSlider->setSliderPosition(opts.value("tweetFetcherWeight",75).toInt());
	ui.mentionsSlider->setSliderPosition(opts.value("mentionsFetcherWeight",50).toInt());
	ui.directMessagesSlider->setSliderPosition(opts.value("directMessagesFetcherWeight",50).toInt());
	ui.favoriteSlider->setSliderPosition(opts.value("favoriteFetcherWeight",50).toInt());
	ui.singleUserSlider->setSliderPosition(opts.value("singleUserFetcherWeight",5).toInt());
	ui.listSlider->setSliderPosition(opts.value("listTweetsFetcherWeight",50).toInt());
	ui.tweettextsizeSlider->setSliderPosition(opts.value("fonts/tweettextsize",DEFAULT_FONT_SIZE).toInt());
	ui.minrefreshtime->setSliderPosition(opts.value("minreRefreshSeconds",15).toInt());
	ui.pointsize->setText(opts.value("fonts/tweettextsize",DEFAULT_FONT_SIZE).toString());

	ui.tweetPruneAge->setText(opts.value("maxTweetAgeInListMins",30).toString());
	ui.minTweets->setText(opts.value("minTweetsPerList",200).toString());
	ui.sendingDelay->setText(opts.value("sendingDelay",5).toString());
	ui.startupcount->setText(opts.value("startupTweetFetchCount",50).toString());

	initialTheme=opts.value("currentTheme",2).toInt();
	ui.theme->setCurrentRow(initialTheme);
	ui.userMainStyleFilename->setText(opts.value("userMainStylesheet","").toString());
	ui.userTextStyleFilename->setText(opts.value("userTextStylesheet","").toString());

	ui.retweets->setChecked(opts.value("displayretweetsfromoriguser",QVariant(false)).toBool());
	ui.upgradecheck->setChecked(opts.value("checkforupdates",QVariant(true)).toBool());

	ui.autoread->setChecked(opts.value("automarkread",QVariant(true)).toBool());
	ui.readtimeSlider->setSliderPosition(opts.value("automarkreadtime",5000).toInt());

	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
	removedAccounts.clear();
	ui.twitteraccounts->clear();
	for (int x=0;x<accts.count();x++) {
		BirdBox *b = accts.at(x);
		QListWidgetItem *item = new QListWidgetItem(b->getUser(),ui.twitteraccounts);
		item->setData(Qt::UserRole+1,b->getUser());
		TweetUser *u = b->getUserByScreenName(b->getAuthenticatingUserScreenName());
		QString url=(u?u->image_url:"");
		item->setIcon(QIcon(images->getPixMap(url)));
	}
	if (accts.count()) {
		ui.twitteraccounts->setCurrentRow(0);
		handleAccountClicked(ui.twitteraccounts->currentItem());
	} else {
		ui.twitterusernamelabel->hide();
		ui.twitterUser->hide();
		ui.clearlabel->hide();
		ui.clearoauth->hide();
	}

	QMap<QString,QString> lang = Translator::getLanguages();
	ui.lang->setCurrentIndex(ui.lang->findText(lang.value(opts.value("baseLanguage","en").toString())));
	ui.translatebutton->setChecked(opts.value("tweettranslatebutton",QVariant(false)).toBool());

	ui.notifications->setChecked(opts.value("notifications",QVariant(true)).toBool());
	ui.notificationswhenvisible->setChecked(opts.value("notificationswhennotvisible",QVariant(false)).toBool());
	ui.notificationsonunmutedonly->setChecked(opts.value("notificationsonunmutedonly",QVariant(true)).toBool());
	ui.shorturls->setChecked(opts.value("shorturls",QVariant(false)).toBool());

	this->show();
    this->raise();
    this->activateWindow();

	ui.filterHelp->setText(tr("<style>")+Ya_st::labelCSS+tr("</style><p>You can set a default filter that will apply to all tweet columns, for example you could filter out all occurances of a particular hash tag, or all messages sent from a particular client.</p><p>To show tweets with <b class=\"hi\">certain words</b> type <i>certain words</i>. To hide other <b class=\"hi\">salacious</b> terms use <i>-salacious</i>.</p><p>For users use <i>@Fred</i>, and <i>-@Barny</i>.</p><p>For clients use <i>@@yasst</i>, and <i>-@@Spambot</i>.</p>"));
	handleMinRefreshSliderMove(ui.minrefreshtime->value());
}

void OptionsWindow::setOpts() {
	QSettings opts;

	opts.setValue("delaySending",QVariant(ui.senddelay->isChecked()));
	opts.setValue("returnSends",QVariant(ui.returnsends->isChecked()));
	opts.setValue("dansannoyingscrollything",QVariant(ui.dansannoyingscrollything->isChecked()));
	opts.setValue("activatewindow",QVariant(ui.activatewindow->isChecked()));
	opts.setValue("trayicon",QVariant(ui.trayicon->isChecked()));
	opts.setValue("globalfiltertext",ui.globalFilter->text());
	opts.setValue("fixreplies",QVariant(ui.fixreplies->isChecked()));

	opts.setValue("proxyType",QVariant(ui.proxyType->currentIndex()));
	opts.setValue("proxyHost",ui.proxyHost->text());
	opts.setValue("proxyPort",ui.proxyPort->text());
	opts.setValue("proxyUser",ui.proxyUser->text());
	opts.setValue("proxyPass",ui.proxyPass->text());
	opts.setValue("sounds/newTweet",ui.alertsound->text());
	opts.setValue("playnewtweetalert",QVariant(ui.newtweetalert->isChecked()));
	opts.setValue("accounts/bitly/user",ui.bitlyUser->text());
	opts.setValue("accounts/bitly/APIkey",ui.bitlyAPI->text());
	opts.setValue("accounts/twitter/user",ui.twitterUser->text());
//	opts.setValue("accounts/twitter/pass",ui.twitterPass->text().toLatin1().toBase64());
	opts.setValue("tweetFetcherWeight",ui.tweetsSlider->sliderPosition());
	opts.setValue("mentionsFetcherWeight",ui.mentionsSlider->sliderPosition());
	opts.setValue("directMessagesFetcherWeight",ui.directMessagesSlider->sliderPosition());
	opts.setValue("favoriteFetcherWeight",ui.favoriteSlider->sliderPosition());
	opts.setValue("singleUserFetcherWeight",ui.singleUserSlider->sliderPosition());
	opts.setValue("listTweetsFetcherWeight",ui.listSlider->sliderPosition());
	opts.setValue("minreRefreshSeconds",ui.minrefreshtime->sliderPosition());
	opts.setValue("fonts/tweettextsize",ui.tweettextsizeSlider->sliderPosition());
	opts.setValue("maxTweetAgeInListMins",ui.tweetPruneAge->text().toInt());
	opts.setValue("minTweetsPerList",ui.minTweets->text().toInt());
	opts.setValue("sendingDelay",ui.sendingDelay->text().toInt());
	opts.setValue("startupTweetFetchCount",ui.startupcount->text().toInt());
	opts.setValue("currentTheme",ui.theme->currentRow());
	opts.setValue("userMainStylesheet",ui.userMainStyleFilename->text());
	opts.setValue("userTextStylesheet",ui.userTextStyleFilename->text());
	opts.setValue("displayretweetsfromoriguser",ui.retweets->isChecked());
	opts.setValue("checkforupdates",ui.upgradecheck->isChecked());
	opts.setValue("automarkread",ui.autoread->isChecked());
	opts.setValue("automarkreadtime",ui.readtimeSlider->sliderPosition());
	opts.setValue("baseLanguage",ui.lang->itemData(ui.lang->currentIndex()).toString());
	opts.setValue("tweettranslatebutton",QVariant(ui.translatebutton->isChecked()));
	opts.setValue("notifications",QVariant(ui.notifications->isChecked()));
	opts.setValue("notificationswhennotvisible",QVariant(ui.notificationswhenvisible->isChecked()));
	opts.setValue("notificationsonunmutedonly",QVariant(ui.notificationsonunmutedonly->isChecked()));
	opts.setValue("shorturls",QVariant(ui.shorturls->isChecked()));

	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
	for(int x=0;x<removedAccounts.count();x++) {
		for (int y=0;y<accts.count();y++) {
			BirdBox *b = accts.at(y);
			if (b->getUser()==removedAccounts.at(x)) {
				((YasstApplication*)qApp)->removeAccount(b);
				b->deleteAccount();
				accts.removeOne(b);
				break;
			}
		}
	}
	removedAccounts.clear();
	for (int x=0;x<accts.count();x++) {
		BirdBox *b=accts.at(x);
		for (int y=0;y<ui.twitteraccounts->count();y++) {
			QListWidgetItem *i = ui.twitteraccounts->item(y);
			if (b->getUser()==i->data(Qt::UserRole+1).toString()) {
				b->setUser(i->text());
				if (b->getOAuth()->getAuthToken()=="") {
					if (ui.twitteraccounts->count()>1)
						QMessageBox::information(0,"Yasst","About to open twitter to authorize '"+b->getUser()+"'.");
					b->getOAuth()->authorize();
				}
				break;
			}
		}
		int to = ui.minrefreshtime->sliderPosition();
		for(int y=0;y<b->fetchersCount();y++) {
			TweetFetcher *f = b->fetchersAt(y);
			if (f && f->type!=5 && f->type!=14 && f->minTimeoutSecs<to)
				f->minTimeoutSecs=to;
		}
	}
	for (int x=0;x<ui.twitteraccounts->count();x++) {
		QListWidgetItem *i = ui.twitteraccounts->item(x);
		if (""==i->data(Qt::UserRole+1).toString()) {
			BirdBox *b = new BirdBox();
			QObject::connect(b,SIGNAL(fetcherAdded(TweetFetcher*)),this,SLOT(setupFetcherWeights(TweetFetcher*)));
			QObject::connect(b,SIGNAL(PINRequest(OAuth *,QString)),mainWindow,SLOT(handlePINRequest(OAuth *,QString)));
			b->setUser(i->text());
			if (b->getOAuth()->getAuthToken()=="") {
				if (ui.twitteraccounts->count()>1)
					QMessageBox::information(0,"Yasst","About to open twitter to authorize '"+b->getUser()+"'.");
				b->getOAuth()->authorize();
			}
			TweetFetcher *f = new UserGroupFetcher();
			b->addFetcher(f,true);
			f = new UserGroupFollowingFetcher();
			b->addFetcher(f,true);

			f = new FriendFetcher();
			b->addFetcher(f);
			f->populate();
			mainWindow->addNewAccount(b);
		}
	}
	this->hide();
	emit optionsSaved();
	setupFetcherWeights();
}

void OptionsWindow::setupFetcherWeights(TweetFetcher* newf) {
	QSettings opts;
	QList<BirdBox*> list = ((YasstApplication*)qApp)->getAccounts();
	for (int x=0;x<list.count();x++) {
		BirdBox *b = list.at(x);
		for (int x=0;x<b->fetchersCount();x++) {
			TweetFetcher *f = b->fetchersAt(x);
			switch (f->type) {
				case 1 :
					f->weight=opts.value("tweetFetcherWeight",75).toInt();
					break;
				case 2 :
					f->weight=opts.value("mentionsFetcherWeight",50).toInt();
					break;
				case 3 :
					f->weight=opts.value("directMessagesFetcherWeight",50).toInt();
					break;
				case 4 :
					f->weight=opts.value("favoriteFetcherWeight",50).toInt();
					break;
				case 13 :
					f->weight=opts.value("listTweetsFetcherWeight",50).toInt();
					break;
			}
		}
	}
	int to = opts.value("minreRefreshSeconds",15).toInt();
	if (newf && newf->type!=5 && newf->type!=14 && newf->minTimeoutSecs<to)
		newf->minTimeoutSecs=to;
}

void OptionsWindow::setFilePath() {
	QString file = QFileDialog::getOpenFileName(this,tr("Open sound file"), "", tr("Annoying noises")+" (*.wav *.mp3)");
	if (file!="")
		ui.alertsound->setText(file);
}

void OptionsWindow::setUserMainStylesheetPath() {
	QString file = QFileDialog::getOpenFileName(this,tr("Open user stylesheet"), "", tr("CSS Files")+" (*.css)");
	if (file!="")
		ui.userMainStyleFilename->setText(file);
}

void OptionsWindow::setUserTextStylesheetPath() {
	QString file = QFileDialog::getOpenFileName(this,tr("Open user stylesheet"), "", tr("CSS Files")+" (*.css)");
	if (file!="")
		ui.userTextStyleFilename->setText(file);
}

void OptionsWindow::clearFilePath() {
	ui.alertsound->setText("");
}

void OptionsWindow::testSound() {
	QString alert = ui.alertsound->text();
	if (playing) {
		playing=false;
		mediaObject->stop();
		ui.test->setText(tr("Play"));
	} else {
		if (alert!="") {
			mediaObject->setCurrentSource(Phonon::MediaSource(alert));
		} else
			mediaObject->setCurrentSource(Phonon::MediaSource(new QFile(":/sounds/new_tweets.mp3")));
		mediaObject->stop();
		mediaObject->play();
		playing=true;
		ui.test->setText(tr("Stop"));
	}
}

void OptionsWindow::soundEnded() {
	playing=false;
	ui.test->setText(tr("Play"));
}

void OptionsWindow::handleThemeChange(int i) {
	if (this->isVisible())
		mainWindow->setStyle(i);
}

void OptionsWindow::closeEvent(QCloseEvent *) {
	QSettings opts;
	int theme =opts.value("currentTheme",2).toInt();

	if (initialTheme!=theme)
		handleThemeChange(theme);
}

void OptionsWindow::handleAccountClicked(QListWidgetItem *i) {
	ui.twitterUser->setText(i->text());
	//ui.twitterPass->setText(i->data(Qt::UserRole).toString());
	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();

	for (int x=0;x<accts.count();x++) {
		BirdBox *b=accts.at(x);
		if (b->getUser()==i->data(Qt::UserRole+1).toString()) {
			if (b->getOAuth()->getAuthToken()=="")
				ui.clearoauth->hide();
			else
				ui.clearoauth->show();
			break;
		}
	}

}

void OptionsWindow::twitterUserChanged(QString text) {
	QListWidgetItem *i = ui.twitteraccounts->currentItem();
	if (i)
		i->setText(text);
}

void OptionsWindow::twitterPassChanged(QString text) {
	QListWidgetItem *i = ui.twitteraccounts->currentItem();
	if (i)
		i->setData(Qt::UserRole,text);
}

void OptionsWindow::handleAddNewUser() {
	QListWidgetItem *item = new QListWidgetItem("New user",ui.twitteraccounts);
	ui.twitteraccounts->setCurrentItem(item);
	item->setIcon(QIcon(images->getPixMap("")));
	ui.twitterUser->setText("New user");
	ui.twitterUser->show();
	ui.twitterUser->setFocus();
	ui.twitterUser->selectAll();
	ui.clearoauth->hide();
	ui.twitterusernamelabel->show();
	ui.clearlabel->show();
//	ui.twitterPass->clear();
}

void OptionsWindow::handleDeleteUser() {
	QListWidgetItem *i = ui.twitteraccounts->currentItem();
	if (i) {
		ui.twitteraccounts->takeItem(ui.twitteraccounts->row(i));
		removedAccounts.append(i->data(Qt::UserRole+1).toString());
		delete i;
		if (ui.twitteraccounts->count()==0) {
			ui.twitterusernamelabel->hide();
			ui.twitterUser->hide();
			ui.clearlabel->hide();
			ui.clearoauth->hide();
		}
	}
}

void OptionsWindow::handleClearOAuth() {
	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();
	QListWidgetItem *i = ui.twitteraccounts->currentItem();

	for (int x=0;x<accts.count();x++) {
		BirdBox *b=accts.at(x);
		if (b->getUser()==i->data(Qt::UserRole+1).toString()) {
			b->getOAuth()->reset();
			ui.clearoauth->hide();
			break;
		}
	}
}

void OptionsWindow::handleClearLang() {
	TweetWidget::lastLang.clear();
}

void OptionsWindow::handleResetSettings() {
	QSettings opts;
	opts.clear();
	close();
}

void OptionsWindow::handleRemoveAllSettings() {
	if (QMessageBox::question(this,"Remove all settings","<h1>Warning</h1><p>Are you sure? If you continue all your settings, including your accounts will be reset and yasst will exit.</p>",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes) {
		QSettings opts;
		opts.clear();
		QApplication::exit(0);
	}
}

void OptionsWindow::handleReloadUserStylesheet() {
	if (this->isVisible())
		mainWindow->setStyle(ui.userMainStyleFilename->text(),ui.userTextStyleFilename->text());
}

void OptionsWindow::handleMinRefreshSliderMove(int x) {
	ui.minrefreshlabel->setText(QString::number(x)+tr(" seconds"));
}
