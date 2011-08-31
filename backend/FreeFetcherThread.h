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
 * FreeFetcherThread.h
 *
 *  Created on: 22-Mar-2009
 *      Author: ian
 */

#ifndef FREEFETCHERTHREAD_H_
#define FREEFETCHERTHREAD_H_

#include <QThread>
#include "backend/BirdBox.h"

class FreeFetcherThread : public QThread {
	Q_OBJECT
public:
	FreeFetcherThread();
	FreeFetcherThread(BirdBox *b) {this->b=b;};
	virtual ~FreeFetcherThread();
public slots:
	void update();
protected :
	void run();
private:
	BirdBox *b;
};

#endif /* FREEFETCHERTHREAD_H_ */
