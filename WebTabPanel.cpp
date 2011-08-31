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
/* $Id: WebTabPanel.cpp,v 1.5 2009/07/18 16:46:14 ian Exp $

 File       : WebTabPanel.cpp
 Create date: 22:51:21 19-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "WebTabPanel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QWebFrame>
#include <QMessageBox>
#include <QTimer>

WebTabPanel::WebTabPanel(BirdBox *t,ImageCache *c,QWidget *parent) : TabPanel(parent),statusUser(0) {
	type=1;
	tweets=t;
	cache=c;
	layout=new QVBoxLayout();
	widget()->setLayout(layout);
	state=0;
	web=new QWebView(this);
	QObject::connect(web,SIGNAL(loadFinished(bool)),this,SLOT(handleLoadFinished(bool)));
	QObject::connect(web,SIGNAL(loadStarted()),this,SLOT(handleLoadStarted()));
	QObject::connect(web,SIGNAL(urlChanged(const QUrl&)),this,SLOT(handleUrlChanged(const QUrl&)));
	QObject::connect(web,SIGNAL(linkClicked(const QUrl&)),this,SLOT(handleLinkClicked(const QUrl&)));
	QObject::connect(this,SIGNAL(followUserByScreenName(QString)),t,SIGNAL(followUserByScreenName(QString)));
	QObject::connect(t,SIGNAL(haveUpdatedUser(TweetUser*)),this,SLOT(handleUpdatedUser(TweetUser*)));
	QObject::connect(c,SIGNAL(update(QString)),this,SLOT(updateImage(QString)));
	QObject::connect(&timer,SIGNAL(timeout()),this,SLOT(handleTimeout()));
	statusArea = new QWidget(this);
	statusArea->setLayout(new QHBoxLayout());
	statusIcon = new QLabel(this);
	statusIcon->setMaximumSize(32,32);
	statusIcon->setScaledContents(true);
	message = new QLabel(this);
	message->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
	statusArea->layout()->setAlignment(Qt::AlignLeft);
	statusArea->layout()->addWidget(statusIcon);
	statusArea->layout()->addWidget(message);
	statusArea->layout()->setMargin(0);
	statusArea->layout()->setSpacing(1);
	infoArea = new QWidget(this);
	loadingText = new QLabel(this);
	loadingText->setText("Please wait, connecting to twitter...");
	progress = new QProgressBar(this);
	progress->setRange(0,0);
	progress->setMaximumHeight(15);
	progress->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
	loadingText->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
	infoArea->setLayout(new QHBoxLayout());
	infoArea->layout()->addWidget(loadingText);
	infoArea->layout()->addWidget(progress);
	infoArea->layout()->setMargin(0);
	infoArea->layout()->setSpacing(1);
	layout->setAlignment(Qt::AlignTop);
	layout->addWidget(statusArea);
	layout->addWidget(infoArea);
	layout->addWidget(web);
	//web->hide();
	web->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,false);
	web->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	web->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	setWidgetResizable(true);
	resetPage();
	handleTimeout();
}

WebTabPanel::~WebTabPanel() {
	QObject::disconnect(web,SIGNAL(loadFinished(bool)),this,SLOT(handleLoadFinished(bool)));
	QObject::disconnect(web,SIGNAL(loadStarted()),this,SLOT(handleLoadStarted()));
	QObject::disconnect(web,SIGNAL(urlChanged(QUrl&)),this,SLOT(handleUrlChanged(QUrl&)));
	QObject::disconnect(this,SIGNAL(followUserByScreenName(QString)),tweets,SIGNAL(followUserByScreenName(QString)));
	QObject::disconnect(tweets,SIGNAL(haveUpdatedUser(TweetUser*)),this,SLOT(handleUpdatedUser(TweetUser*)));
	QObject::disconnect(cache,SIGNAL(update(QString)),this,SLOT(handleUpdateImage(QString)));
}

void WebTabPanel::resetPage() {
	web->setEnabled(false);
	web->load(QUrl("http://twitter.com/invitations/find_on_twitter"));
}

void WebTabPanel::handleUrlChanged(const QUrl &u) {
	QString url = u.toString();
//	QMessageBox::information (0,"assdf",url);


	if (!(url==""
		||url=="http://twitter.com/login"
		||url=="http://twitter.com/invitations/find_on_twitter"

		||url.startsWith(tr("http://twitter.com")))
		) {
		QTimer::singleShot(0,this,SLOT(resetPage()));
		//web->page()->mainFrame()->evaluateJavaScript("alert('boo')");
	}
}

void WebTabPanel::handleLoadStarted() {
	if (state>-2) {
		infoArea->show();
		progress->show();
		loadingText->show();
		if (state>1)
			loadingText->setText("Please wait...");
	}
	web->setEnabled(false);
}

void WebTabPanel::handleLoadFinished(bool) {
	QWebFrame *frame = web->page()->mainFrame();
	QString url=frame->url().toString();
	//QMessageBox::information (0,"assdf",tr("Handle load finished, state: ")+QString::number(state)+tr(", url: ")+frame->url().toString());
	infoArea->hide();
	progress->hide();
	loadingText->hide();
	if (url=="http://twitter.com/login") {
		if (!tweets->getAuthenticatingFetcher()) {
			state=-1;
			return;
		}
		QString user = tweets->getAuthenticatingFetcher()->getUsername();
		QString pass = tweets->getAuthenticatingFetcher()->getPassword();

		// Homepage login
		//frame->evaluateJavaScript(tr("var f=document.getElementById('signin');document.getElementById('username').value=\"")+user+tr("\";document.getElementById('password').value=\"")+pass+tr("\";f.submit();"));
		// redirect page login
		loadingText->setText("Please wait, signing in to twitter...");
		frame->evaluateJavaScript(tr("var f=document.forms[1];f['username_or_email'].value=\"")+user+tr("\";f['session[password]'].value=\"")+pass+tr("\";f.submit();"));
		state=1;//loggin in

	} else if (url=="http://twitter.com/invitations/find_on_twitter") {
		// Now at being passed to twitter state
		state=2;//Searching
		web->show();
		frame->evaluateJavaScript(tr("function $(s){return document.getElementById(s);}function __yasstlinks() {var i=['home_link','profile_link','settings_link','help_link','sign_out_link'];for (var b in i) {b=$(i[b]);b.style.display='none';}i=$('footer').firstChild.nextSibling.nextSibling.nextSibling.firstChild.nextSibling;while(i.nextSibling){i.parentNode.removeChild(i.nextSibling);}}__yasstlinks();"));
		frame->setScrollPosition(QPoint(0,0));
		//QTimer::singleShot(1000,this,SLOT(resetScroll()));
		web->setEnabled(true);
		handleTimeout();
	} else if (url.startsWith(tr("http://twitter.com/"))) {
		state=3;//Search results or user info.
		frame->evaluateJavaScript(tr("function $(s){return document.getElementById(s);}function __yasstlinks() {var i=['home_link','profile_link','settings_link','help_link','sign_out_link'];for (var b in i) {b=$(i[b]);b.style.display='none';}i=$('footer').firstChild.nextSibling.nextSibling.nextSibling.firstChild.nextSibling;while(i.nextSibling){i.parentNode.removeChild(i.nextSibling);}}__yasstlinks();"));
		frame->evaluateJavaScript(tr("function __yasst() {i=$('timeline');i.innerHTML='<style>.follow-actions input{display:none;} a.__yasst-followsingle {text-decoration:none; visibility:visible !important; -webkit-appearance: none; -webkit-border-bottom-left-radius: 5px; -webkit-border-bottom-right-radius: 5px; -webkit-border-top-left-radius: 5px; -webkit-border-top-right-radius: 5px; -webkit-box-align: center; -webkit-box-sizing: border-box; -webkit-rtl-ordering: logical; -webkit-user-select: text; background-color: rgb(230, 230, 230); border-bottom-color: rgb(204, 204, 204); border-bottom-style: solid; border-bottom-width: 1px; border-left-color: rgb(204, 204, 204); border-left-style: solid; border-left-width: 1px; border-right-color: rgb(204, 204, 204); border-right-style: solid; border-right-width: 1px; border-top-color: rgb(204, 204, 204); border-top-style: solid; border-top-width: 1px; color: black; cursor: pointer; display: inline-block; font-family: \\'Lucida Grande\\'; font-size: 10px; font-style: normal; font-variant: normal; font-weight: normal; height: 23px; letter-spacing: normal; line-height: normal; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; margin-top: 0px; padding-bottom: 4px; padding-left: 8px; padding-right: 8px; padding-top: 4px; text-align: center; text-indent: 0px; text-shadow: none; text-transform: none; vertical-align: top; white-space: pre; width: 48px; word-spacing: 0px;} a:hover.__yasst-follow,a:hover.__yasst-followsingle{background-color: rgb(204, 204, 204);} a.__yasst-follow{visibility: visible !important; -webkit-appearance: none; -webkit-border-bottom-left-radius: 5px; -webkit-border-bottom-right-radius: 5px; -webkit-border-top-left-radius: 5px; -webkit-border-top-right-radius: 5px; -webkit-box-align: center; -webkit-box-sizing: border-box; -webkit-rtl-ordering: logical; -webkit-user-select: text; background-color: rgb(230, 230, 230); border-bottom-color: rgb(204, 204, 204); border-bottom-style: solid; border-bottom-width: 1px; border-left-color: rgb(204, 204, 204); border-left-style: solid; border-left-width: 1px; border-right-color: rgb(204, 204, 204); border-right-style: solid; border-right-width: 1px; border-top-color: rgb(204, 204, 204); border-top-style: solid; border-top-width: 1px; color: black; cursor: pointer; display: none; font-family: \\'Lucida Grande\\'; font-size: 10px; font-style: normal; font-variant: normal; font-weight: normal; height: auto; letter-spacing: normal; line-height: normal; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; margin-top: 0px; padding-bottom: 4px; padding-left: 8px; padding-right: 8px; padding-top: 4px; text-align: center; text-indent: 0px; text-shadow: none; text-transform: none; vertical-align: top; white-space: pre; width: 80px; word-spacing: 0px;}</style>'+$('timeline').innerHTML;var e=document.getElementById('follow_control');if (e!=null) {var u=e.parentNode.parentNode.className;u=u.substr(u.lastIndexOf(' ')+1);var l=document.createElement('div');l.innerHTML='<a class=\\\"__yasst-followsingle\\\" href=\\\"twitter:/follow/'+u+'\\\">Follow</a>';e.parentNode.parentNode.insertBefore(l,e.parentNode);} else {		e=i.getElementsByClassName('follow-actions');for(b=0;b<e.length;b++) {var u=e[b].parentNode.parentNode.className;u=u.substr(u.lastIndexOf(' ')+6);var l=document.createElement('div');var c=e[b].childNodes;for(var x=0;x<c.length;x++) {if (c[x].nodeName=='FORM') {var d=c[x].action.substr(c[x].action.lastIndexOf('/')+1);var f=c[x].firstChild.childNodes;l.innerHTML='<a class=\\\"__yasst-follow\\\" href=\\\"twitter:/follow/'+u+'\\\">Follow</a>';for(var y=0;y<f.length;y++) {if(f[y].nodeName=='INPUT'&&f[y].type=='submit') {f[y].parentNode.insertBefore(l,f[y]);}}}}}}}__yasst();"));
		web->setEnabled(true);
		//frame->setScrollPosition(QPoint(0,0));
		//QTimer::singleShot(1000,this,SLOT(resetScroll()));
	}
}

void WebTabPanel::handleLinkClicked(const QUrl&url) {
	QString urlString = url.toString();
	if (urlString.startsWith("twitter:/follow/")) {
		expectedId=urlString.mid(tr("twitter:/follow/").length(),-1);
		//QMessageBox::information (0,"assdf",expectedId);
		emit followUserByScreenName(expectedId);
	} else if (urlString.startsWith(tr("http://twitter.com/"))) {
		web->load(url);
	}
}

void WebTabPanel::handleUpdatedUser(TweetUser* u) {
	//QMessageBox::information (0,"assdf",expectedId+tr(":")+u->id);
	if (u->is_friend && u->screen_name==expectedId) {
		state=-2;
		web->reload();
		//QTimer::singleShot(1000,web,SLOT(reload()));
		expectedId="";
		displayStatusMessage(tr("You are now following ")+u->screen_name,u);
	}
}

void WebTabPanel::displayStatusMessage(QString msg,TweetUser* user) {
	message->setText(msg);
	if (user) {
		statusIcon->setPixmap(cache->getPixMap(user->image_url));
	}
	statusArea->show();
	statusIcon->show();
	message->show();
	statusUser=user;
	timer.start(7000);
}

void WebTabPanel::handleUpdateImage(QString url) {
	if (statusUser && statusUser->image_url==url)
		statusIcon->setPixmap(cache->getPixMap(statusUser->image_url));
}

void WebTabPanel::handleTimeout() {
	statusArea->hide();
	statusIcon->hide();
	message->hide();
	statusUser=0;
	timer.stop();
}
