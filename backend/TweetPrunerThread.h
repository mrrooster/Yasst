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
 * TweetPrunerThread.h
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#ifndef TWEETPRUNERTHREAD_H_
#define TWEETPRUNERTHREAD_H_
#include <QThread>
#include <QTimer>
#include "BirdBox.h"

class TweetPrunerThread : public QThread {
	Q_OBJECT
public:
	TweetPrunerThread();
	TweetPrunerThread(BirdBox *b) {this->tweets=b;timer=0;};
	virtual ~TweetPrunerThread();
	void run();
protected:
	BirdBox *tweets;
	QTimer *timer;
private slots:
	void prune();
};

#endif /* TWEETPRUNERTHREAD_H_ */
