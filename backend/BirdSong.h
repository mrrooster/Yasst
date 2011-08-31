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
/* $Id: BirdSong.h,v 1.2 2010/07/20 20:08:15 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/
/*
 * BirdSong.h
 *
 *  Created on: 28-Jan-2009
 *      Author: ian
 */

#ifndef BIRDSONG_H_
#define BIRDSONG_H_

#include <QThread>
#include "BirdBox.h"

class BirdSong : public QThread {
	Q_OBJECT
public:
	BirdSong();
	BirdSong(BirdBox *b) {this->b=b;};
	virtual ~BirdSong();
public slots:
	void update();
	void handleAPILimit(long,long);
	void handleLimitExceeded();
protected :
	void run();
private:
	BirdBox *b;
	QTimer ticktock;
};

#endif /* BIRDSONG_H_ */
