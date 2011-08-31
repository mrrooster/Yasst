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
/* $Id: ImagePrunerThread.cpp,v 1.2 2010/06/19 20:29:41 ian Exp $

 File       : ImagePrunerThread.cpp
 Create date: 18:00:48 17-Apr-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "ImagePrunerThread.h"
#include <QTimer>
#include <QMutableHashIterator>

ImagePrunerThread::ImagePrunerThread(ImageCache *i) {
	cache=i;
	timer=0;
	oldageSecs=60*10;
	expireSecs=60*60*2;
}

ImagePrunerThread::~ImagePrunerThread() {
	if (timer)
		timer->deleteLater();
}

void ImagePrunerThread::run() {
	timer=new QTimer();
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(prune()));
	timer->start(1000*(30));
	QThread::exec();
}

void ImagePrunerThread::prune() {
	QDateTime cutoff = QDateTime::currentDateTime().addSecs(-oldageSecs);
	QDateTime oldage = QDateTime::currentDateTime().addSecs(-expireSecs);
	QMutableHashIterator <QString,ImageCache::cacheEntry*> *i=cache->getIterator();
	unsigned long count=0;
	while (i->hasNext()) {
		i->next();
		ImageCache::cacheEntry *e=i->value();
		if (e->lastaccess<cutoff) {
			i->remove();
			delete e;
			count++;
		} else if (e->create<oldage) {
			e->canReplace=true;
		}
	}
	cache->releaseIterator();
//	cache->updateStats();
	cache->pruneCount+=count;
}

void ImagePrunerThread::setOldAgeSecs(int s) {
	oldageSecs=s;
}

void ImagePrunerThread::setExpirySecs(int s) {
	expireSecs=s;
}

