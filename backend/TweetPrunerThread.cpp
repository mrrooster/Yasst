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
/*
 * TweetPrunerThread.cpp
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#include <QMutableHashIterator>
#include "TweetPrunerThread.h"
#include "Tweet.h"
#include <QMutexLocker>

TweetPrunerThread::TweetPrunerThread() {

}

TweetPrunerThread::~TweetPrunerThread() {
	if (timer)
		timer->deleteLater();
}

void TweetPrunerThread::run() {
	timer=new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(prune()));
	timer->start(20000);
	tweets->nextPrune=QDateTime::currentDateTime().addMSecs(20000);
	QThread::exec();
}

void TweetPrunerThread::prune() {
	tweets->nextPrune=QDateTime::currentDateTime().addMSecs(20000);
	QMutableHashIterator <QString,Tweet*> *i=tweets->getTweetsIterator();
	unsigned long count=0;

	QDateTime cutoff = QDateTime::currentDateTime().addSecs(-(60*20));
	while (i->hasNext()) {
		i->next();
		Tweet *t=i->value();
		if (t->refCount==0 && t->timestamp<cutoff) {
			i->remove();
			delete t;
			count++;
		}
	}
	tweets->releaseTweetsIterator();
	tweets->pruneCount+=count;
}
