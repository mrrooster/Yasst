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
/* $Id: ImageCache.h,v 1.4 2010/06/19 20:29:41 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef IMAGECACHE_H_
#define IMAGECACHE_H_

#include <QObject>
#include <QHash>
#include <QString>
#include <QImage>
#include <QNetworkReply>
#include "HTTPHandler.h"
#include <QList>
#include <QThread>
#include <QDateTime>
#include <QPixmap>
#include <QMutableHashIterator>
#include <QMutex>
#include <QTimer>

class ImageCache : public HTTPHandler {
	Q_OBJECT
public:
	ImageCache();
	virtual ~ImageCache();
	struct cacheEntry
	{
		//QImage image;
		QPixmap pixmap;
		QDateTime create;
		QDateTime lastaccess;
		int updateCount;
		bool canReplace;
	};
	void cacheImage(QString url);
	void doCacheImage();
	QImage get(QString url);
	QPixmap getPixMap(QString url);
	bool contains(QString url);
	void updateStats();
	QMutableHashIterator<QString,ImageCache::cacheEntry*>* getIterator();
	void releaseIterator();
	void setMaxSize(int w,int h);
	QList<QString> pendingUrls;
	unsigned long pruneCount;
	QImage defaultImage;
	QPixmap *defaultPixmap;
	QDateTime lruDate;
	QDateTime mruDate;
	QDateTime newestDate;
	QDateTime oldestDate;
	QString lruUrl;
	QString mruUrl;
	QString newestUrl;
	QString oldestUrl;
	unsigned long count;
	unsigned long totalBytes;
	void setOldAgeSecs(int s);
	void setExpirySecs(int s);

signals:
	void update(QString);
protected:
	ImageCache::cacheEntry * getEntry(QString url);
	virtual void handleHttpResponse(QBuffer *resp,QUrl url,bool isSend,int);
private:
	QHash<QString,ImageCache::cacheEntry*> images;
//	QThread *thread;
	QTimer drainTimer;
	QThread *prunerThread;
	QMutex cacheLock;
	QMutex pendingLock;
	QMutableHashIterator<QString,ImageCache::cacheEntry*> cacheIterator;
	int width;
	int height;
	int oldageSecs;
	int expireSecs;
private slots:
	void drainTimerFired();
};

#endif /* IMAGECACHE_H_ */
