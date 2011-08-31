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
/* $Id: ImagePrunerThread.h,v 1.1 2009/09/02 20:44:09 ian Exp $

 File       : ImagePrunerThread.h
 Create date: 18:00:48 17-Apr-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef IMAGEPRUNERTHREAD_H_
#define IMAGEPRUNERTHREAD_H_

#include <QThread>
#include "ImageCache.h"

class ImagePrunerThread : public QThread {
	Q_OBJECT
public:
	ImagePrunerThread() {cache=0;};
	ImagePrunerThread(ImageCache *);
	virtual ~ImagePrunerThread();

	void setOldAgeSecs(int s);
	void setExpirySecs(int s);
public slots:
	void prune();
protected :
	void run();
private:
	ImageCache *cache;
	QTimer *timer;
	int oldageSecs;
	int expireSecs;
};

#endif /* IMAGEPRUNERTHREAD_H_ */
