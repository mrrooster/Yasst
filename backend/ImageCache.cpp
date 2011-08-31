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
/* $Id: ImageCache.cpp,v 1.6 2010/06/19 20:29:41 ian Exp $
File:   ImageCache.h
(c) 2009 Ian Clark

*/


#include "ImageCache.h"
//#include "ImageCacheThread.h"
#include "ImagePrunerThread.h"
#include <QMessageBox>
#include <QHashIterator>
#include <QMutexLocker>

ImageCache::ImageCache() : pruneCount(0),defaultPixmap(0),count(0),totalBytes(0),prunerThread(0),cacheLock(QMutex::Recursive),pendingLock(),cacheIterator(images) {
	//defaultImage = QImage(":/misc/media/missingavatar.png");
	QObject::connect(&drainTimer,SIGNAL(timeout()),this,SLOT(drainTimerFired()));
	width=0;
	height=0;
	oldageSecs=60*10;
	expireSecs=60*60*2;
}

ImageCache::~ImageCache() {
/*
	if (thread)
		thread->deleteLater();
*/
	if (prunerThread)
		prunerThread->deleteLater();
	if (defaultPixmap)
		delete defaultPixmap;
	QObject::disconnect(&drainTimer,SIGNAL(timeout()),this,SLOT(drainTimerFired()));
}

void ImageCache::cacheImage(QString url) {
	QMutexLocker pendingLocker(&pendingLock);
	url=QUrl(url).toString();
	if (images.contains(url))
		return;

	if (!pendingUrls.size())
		drainTimer.start(50);
	pendingUrls.append(url);
/*
	if (!thread) {
		thread=(QThread*)new ImageCacheThread(this);
		thread->start();
	}
*/
}

void ImageCache::doCacheImage() {
	QMutexLocker pendingLocker(&pendingLock);
	if (pendingUrls.count())
		doHttpBit(pendingUrls.takeFirst());
}

QImage ImageCache::get(QString url) {
	ImageCache::cacheEntry *e=getEntry(url);

	if (e)
		return defaultImage;
	return defaultImage;
}

QPixmap ImageCache::getPixMap(QString url) {
	if (!defaultPixmap) {
		defaultPixmap = new QPixmap(":/misc/media/missingavatar.png");
		//defaultPixmap->fromImage(defaultImage);
	}
	if (url=="")
		return *defaultPixmap;
	ImageCache::cacheEntry *e=getEntry(QUrl(url).toString());

	if (e)
		return e->pixmap;

	return *defaultPixmap;
}

bool ImageCache::contains(QString url) {
	return (getEntry(QUrl(url).toString())!=0);
}

ImageCache::cacheEntry * ImageCache::getEntry(QString url) {
	if (images.contains(url)) {
		ImageCache::cacheEntry *e = images.value(url);

		e->lastaccess=QDateTime::currentDateTime();
		mruDate=e->lastaccess;
		mruUrl=url;
		if (e->canReplace) {
			cacheImage(url);
		}

		return e;
	} else {
		if (!pendingUrls.contains(url))
			cacheImage(url);
	}
	return 0;
	//QImage(":/misc/media/missingavatar.png");
}

void ImageCache::handleHttpResponse(QBuffer *resp, QUrl url,bool,int) {
	QMutexLocker cacheLocker(&cacheLock);
	ImageCache::cacheEntry *e;

	if (resp->size()==0 || resp->data().startsWith("<!DOCTYPE"))
		return;

	if (images.contains(url.toString())) {
		e = images.value(url.toString());
		if (e->create.addSecs(60)>QDateTime::currentDateTime())
			return;
	} else {
		e=new ImageCache::cacheEntry();
		e->updateCount=0;
		images.insert(url.toString(),e);
	}
	e->canReplace=false;
	e->create=QDateTime::currentDateTime();
	e->lastaccess=QDateTime::currentDateTime();
	//e->image.loadFromData(resp->buffer());
	//e->pixmap=QPixmap::fromImage(e->image));
	e->pixmap.loadFromData(resp->buffer());
	if (width>0||height>0) {
		if (e->pixmap.height()>e->pixmap.width()) {
			if (height>0)
				e->pixmap=e->pixmap.scaledToHeight(height,Qt::SmoothTransformation);
		} else {
			if (width>0)
				e->pixmap=e->pixmap.scaledToWidth(width,Qt::SmoothTransformation);
		}
	}
//	e->pixmap=e->pixmap.scaledToWidth(40,Qt::SmoothTransformation);
	e->updateCount++;


	updateStats();

	emit update(url.toString());
	if (!prunerThread) {
		prunerThread = (QThread*)new ImagePrunerThread(this);
		((ImagePrunerThread*)prunerThread)->setOldAgeSecs(oldageSecs);
		((ImagePrunerThread*)prunerThread)->setExpirySecs(expireSecs);
		prunerThread->start();
	}
}

void ImageCache::updateStats() {
	QMutableHashIterator <QString,ImageCache::cacheEntry*> *i=getIterator();

	QDateTime mru;
	QDateTime lru=QDateTime::currentDateTime();
	QDateTime newest;
	QDateTime oldest=QDateTime::currentDateTime();
	QString mruUrl,lruUrl,newestUrl,oldestUrl;
	unsigned long size=0;

	while(i->hasNext()) {
		i->next();
		ImageCache::cacheEntry *e=i->value();
		if (mru<e->lastaccess) {
			mru=e->lastaccess;
			mruUrl=i->key();
		}
		if (lru>e->lastaccess) {
			lru=e->lastaccess;
			lruUrl=i->key();
		}
		if (newest<e->create) {
			newest=e->create;
			newestUrl=i->key();
		}
		if (oldest>e->create) {
			oldest=e->create;
			oldestUrl=i->key();
		}
		QSize s=e->pixmap.size();
		size += (s.width()*s.height()*e->pixmap.depth()/8)+sizeof(e->pixmap);
	}
	releaseIterator();
	this->mruDate=mru;
	this->lruDate=lru;
	this->oldestDate=oldest;
	this->newestDate=newest;
	this->mruUrl=mruUrl;
	this->lruUrl=lruUrl;
	this->oldestUrl=oldestUrl;
	this->newestUrl=newestUrl;
	this->count=images.count();
	this->totalBytes=size;
}

QMutableHashIterator<QString,ImageCache::cacheEntry*>* ImageCache::getIterator() {
	cacheLock.lock();
	cacheIterator.toFront();
	return &cacheIterator;
}

void ImageCache::releaseIterator() {
	cacheLock.unlock();
}

void ImageCache::drainTimerFired() {
	if (pendingUrls.count()) {
		doCacheImage();
	} else
		drainTimer.stop();
}

void ImageCache::setMaxSize(int w,int h) {
	if (w>=0)
		width=w;
	if (h>=0)
		height=h;
}

void ImageCache::setOldAgeSecs(int s) {
	oldageSecs=s;
	if (prunerThread)
		((ImagePrunerThread*)prunerThread)->setOldAgeSecs(s);
}

void ImageCache::setExpirySecs(int s) {
	expireSecs=s;
	if (prunerThread)
		((ImagePrunerThread*)prunerThread)->setExpirySecs(s);
}

