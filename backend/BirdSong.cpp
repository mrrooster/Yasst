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
/* $Id: BirdSong.cpp,v 1.3 2010/07/20 20:08:15 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/

#include "BirdSong.h"
#include "fetchers/TweetFetcher.h"
#include <QTimer>
#include <QMutexLocker>

BirdSong::BirdSong() {
}

BirdSong::~BirdSong() {
}

void BirdSong::run() {
	QObject::connect(b,SIGNAL(rateUpdate(long,long)),this,SLOT(handleAPILimit(long,long)));
	QObject::connect(b,SIGNAL(limitExceeded()),this,SLOT(handleLimitExceeded()));
	QObject::connect(&ticktock,SIGNAL(timeout()),this,SLOT(update()));
	QTimer::singleShot(0, this, SLOT(update()));
	//update();
	ticktock.setSingleShot(false);
	// Disabled for now, using response headers.
	//ticktock.start(60000);
	QThread::exec();
}

void BirdSong::update() {
	b->getRate();
	//QTimer::singleShot(60000, this, SLOT(update()));
}

void BirdSong::handleAPILimit(long count,long secs) {
	QMutexLocker fetchersLocker(&(b->fetcherLock));
	int x,totalWeight=0;

	b->apiRefresh=QDateTime::currentDateTime().addSecs(secs);
	b->apiCount=count;

	if (count<=0) {
		for (x=0;x<b->fetchers.count();x++)
			b->fetchers.at(x)->activateTimeout(secs+2,0);
	} else {
		for (x=0;x<b->fetchers.count();x++)
			totalWeight+=b->fetchers.at(x)->weight;
		secs=secs/count;
		totalWeight=totalWeight*secs;
		for (x=0;x<b->fetchers.count();x++) {
			TweetFetcher *f=b->fetchers.at(x);
			f->activateTimeout((totalWeight/f->weight)+2,0);
		}
	}
}

void BirdSong::handleLimitExceeded() {
}
