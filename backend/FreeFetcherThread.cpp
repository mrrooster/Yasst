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
/* $Id: FreeFetcherThread.cpp,v 1.2 2010/03/21 18:08:05 ian Exp $
File:   FreeFetcherThread.h
(c) 2009 Ian Clark

*/

#include "FreeFetcherThread.h"
#include <QTimer>
#include "fetchers/TweetFetcher.h"
#include <QMutexLocker>

FreeFetcherThread::FreeFetcherThread() {
}

FreeFetcherThread::~FreeFetcherThread() {
}

void FreeFetcherThread::run() {
	QTimer::singleShot(2000, this, SLOT(update()));
	//update();
	QThread::exec();
}

void FreeFetcherThread::update() {
	QMutexLocker freeFetcherLocker(&(b->freeFetcherLock));
	int jitter = b->freeFetchers.count()*6;
	for (int x=0;x<b->freeFetchers.count();x++) {
		b->freeFetchers.at(x)->activateTimeout(120,(jitter<30)?30:jitter);
	}
	QTimer::singleShot(60000, this, SLOT(update()));

}
