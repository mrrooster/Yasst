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
#include "NotificationWidget.h"
#include "ya_st.h"
#include <QListWidgetItem>
#include <QDesktopWidget>
#include <QMutexLocker>

#include <QDebug>

//#ifndef Q_WS_MAC
QList<NotificationWidget*> NotificationWidget::notifications;
QMutex NotificationWidget::notificationsLock;
//#endif

NotificationWidget::NotificationWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::ToolTip)
{
//#ifndef Q_WS_MAC
	ui.setupUi(this);

	timeout.setSingleShot(true);

	opacity=1.0;

	setObjectName("notification");
	setStyleSheet(Ya_st::mainCSS);

	QObject::connect(&timeout,SIGNAL(timeout()),this,SLOT(handleExpiredTimer()));
//#endif
}

NotificationWidget::~NotificationWidget()
{
//#ifndef Q_WS_MAC
	QMutexLocker notificationsLocker(&NotificationWidget::notificationsLock);
	NotificationWidget::notifications.removeOne(this);
//#endif
}

void NotificationWidget::addMessage(QString msg, QPixmap pic) {
//#ifndef Q_WS_MAC
	QHBoxLayout *l = new QHBoxLayout();
	QLabel *w = new QLabel();
	w->setMaximumSize(16,16);
	w->setPixmap(pic);
	w->setScaledContents(true);
	l->addWidget(w);
	w=new QLabel();
	w->setText(msg);
	l->addWidget(w);
	((QBoxLayout*)layout())->addLayout(l);
//#endif
//#ifdef Q_WS_MAC
//	icons.append(pic);
//	text.append(msg);
//#endif
}

void NotificationWidget::handleExpiredTimer() {
//#ifndef Q_WS_MAC
	if (opacity==1.0) {
		setWindowOpacity(1.0);
		setAttribute(Qt::WA_TranslucentBackground, true);
	}
	if (opacity>0) {
		opacity-=0.05;
		setWindowOpacity(opacity);
		timeout.start(50);
	} else {
		close();
		this->deleteLater();
	}
//#endif
}

void NotificationWidget::showNotification() {
//#ifndef Q_WS_MAC
	QMutexLocker notificationsLocker(&NotificationWidget::notificationsLock);
	QDesktopWidget *d = QApplication::desktop();

	QRect r = d->availableGeometry(this);

	show();
	move(r.right()-width(),r.bottom()-height());
	for(int x=0;x<NotificationWidget::notifications.count();x++) {
		NotificationWidget *w = NotificationWidget::notifications.at(x);
		w->move(w->x(),w->y()-height());
	}
	NotificationWidget::notifications.append(this);
	timeout.start(10000);
//#endif
//#ifdef Q_WS_MAC
//	while (icons.count()>0) {
//		QPixmap icon = icons.takeAt(0);
//		QString msg = text.takeAt(0);
//
//		tray.showMessage(tr("Yasst"),msg);
//	}
//	tray.showMessage("Hello","hello");
//#endif
}
