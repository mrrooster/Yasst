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
#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QtGui/QWidget>
#include "ui_NotificationWidget.h"
#include <QPixmap>
#include <QTimer>
#include <QList>
#include <QMutex>
//#ifdef Q_WS_MAC
//#include <QSystemTrayIcon>
//#endif

class NotificationWidget : public QWidget
{
    Q_OBJECT

public:
    NotificationWidget(QWidget *parent = 0);
    ~NotificationWidget();

    void addMessage(QString,QPixmap=QPixmap());
    void showNotification();
private:
//#ifndef Q_WS_MAC
    Ui::NotificationWidgetClass ui;
    QTimer timeout;
    qreal opacity;
    static QList<NotificationWidget*> notifications;
    static QMutex notificationsLock;
//#endif
//#ifdef Q_WS_MAC
//    QList<QPixmap> icons;
//    QList<QString> text;
//    QSystemTrayIcon tray;
//#endif
private slots:
	void handleExpiredTimer();
};

#endif // NOTIFICATIONWIDGET_H
