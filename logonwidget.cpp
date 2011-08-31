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
/* $Id: logonwidget.cpp,v 1.11 2010/06/19 20:29:41 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/

#include "logonwidget.h"
#include <QSettings>

LogonWidget::LogonWidget(QWidget *parent)
    : QWidget(parent)
{
	QSettings opts;

	ui.setupUi(this);
	QObject::connect(ui.logon,SIGNAL(clicked()),this,SLOT(logonClicked()));
	QObject::connect(ui.skip,SIGNAL(clicked()),this,SIGNAL(logonSkipped()));
	QObject::connect(ui.username,SIGNAL(returnPressed()),this,SLOT(logonClicked()));
	setObjectName(tr("logonwidget"));

	ui.username->setFocus();

}

LogonWidget::~LogonWidget()
{
	QObject::disconnect(ui.logon,SIGNAL(clicked()),this,SLOT(logonClicked()));
	QObject::disconnect(ui.skip,SIGNAL(clicked()),this,SIGNAL(logonSkipped()));

}

void LogonWidget::setFailedLogon(QString user) {
	ui.username->setText(user);
}

void LogonWidget::logonClicked() {
	QSettings opts;

//	opts.setValue("rememberpassword",ui.savedetails->isChecked());
	emit logon(ui.username->text(),"",true);
}
