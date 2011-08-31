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
/* $Id: TweetTextEdit.h,v 1.7 2010/04/25 23:20:27 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#ifndef TWEETTEXTEDIT_H_
#define TWEETTEXTEDIT_H_
#include <QPlainTextEdit>

class TweetTextEdit : public QPlainTextEdit{
	Q_OBJECT
public:
	TweetTextEdit(QWidget *p=0);
	virtual ~TweetTextEdit();
	void keyReleaseEvent ( QKeyEvent * event );
	void keyPressEvent ( QKeyEvent * event );
	void insertFromMimeData ( QMimeData *);
signals:
	void returnPressed();
	void escapePressed();
	void urlPasted(QString);
};

#endif /* TWEETTEXTEDIT_H_ */
