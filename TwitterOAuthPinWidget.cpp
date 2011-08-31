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
#include "TwitterOAuthPinWidget.h"
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QTimer>

TwitterOAuthPinWidget::TwitterOAuthPinWidget(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.clipboard,SIGNAL(clicked()),this,SLOT(copyURLToClipboard()));
	QObject::connect(ui.opentwitter,SIGNAL(clicked()),this,SLOT(launchURL()));
	setUser("");
	ui.pintext->hide();
	ui.pin->hide();
	ui.logon->hide();
	ui.line->hide();
	ui.label->hide();
	ui.urllabel->hide();
	ui.url->hide();
	ui.clipboard->hide();
	ui.message->show();
	this->update();
}

TwitterOAuthPinWidget::~TwitterOAuthPinWidget()
{

}

void TwitterOAuthPinWidget::setURL(QString url) {
	ui.url->setText(url);
//	QTimer::singleShot(1400,this,SLOT(launchURL()));
}

void TwitterOAuthPinWidget::setUser(QString u) {
	if (u=="")
		ui.pintext->setText("PIN from twitter");
	else {
		ui.pintext->setText("Log on to twitter as '"+u+"', then enter the PIN here");
		ui.message->setText("You now need to give Yasst permission to use your twitter account. Click on the button below to open twitter in your browser. You should logon as '"+u+"'.");
	}
}

QString TwitterOAuthPinWidget::getPIN() {
	return ui.pin->text();
}

void TwitterOAuthPinWidget::copyURLToClipboard() {
	QApplication::clipboard()->setText(ui.url->text());
}

void TwitterOAuthPinWidget::launchURL() {
	ui.pintext->show();
	ui.pin->show();
	ui.logon->show();
	ui.line->show();
	ui.label->show();
	ui.urllabel->show();
	ui.url->show();
	ui.clipboard->show();
	ui.message->hide();

	this->setMinimumHeight(this->height()+1);

	if (ui.url->text().length()>0)
		QDesktopServices::openUrl(QUrl(ui.url->text()));
}
