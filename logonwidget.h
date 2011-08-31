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
/* $Id: logonwidget.h,v 1.5 2010/03/04 23:48:16 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
#ifndef LOGONWIDGET_H
#define LOGONWIDGET_H

#include <QtGui/QWidget>
#include "ui_logonwidget.h"

class LogonWidget : public QWidget
{
    Q_OBJECT

public:
    LogonWidget(QWidget *parent = 0);
    ~LogonWidget();
    void setFailedLogon(QString user);
public slots:
	void logonClicked(void);
signals:
	void logon(QString,QString,bool);
	void logonSkipped();

private:
    Ui::LogonWidgetClass ui;
};

#endif // LOGONWIDGET_H
