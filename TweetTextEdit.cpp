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
/* $Id: TweetTextEdit.cpp,v 1.7 2010/04/25 23:20:27 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "TweetTextEdit.h"

TweetTextEdit::TweetTextEdit(QWidget *p) : QPlainTextEdit(p){
}

TweetTextEdit::~TweetTextEdit() {
}

void TweetTextEdit::keyReleaseEvent ( QKeyEvent * event ) {
	if (event->key()==Qt::Key_Return||event->key()==Qt::Key_Enter) {
		emit returnPressed();
		event->accept();
	} else if (event->key()==Qt::Key_Escape) {
		emit escapePressed();
		event->accept();
	} else
		QPlainTextEdit::keyReleaseEvent(event);
}

void TweetTextEdit::keyPressEvent ( QKeyEvent * event ) {
	if (event->key()==Qt::Key_Return||event->key()==Qt::Key_Enter||event->key()==Qt::Key_Escape) {
		event->accept();
		return;
	}
	QPlainTextEdit::keyPressEvent(event);
}

void TweetTextEdit::insertFromMimeData ( QMimeData *data) {
	if (data->hasText()) {
		QString text=data->text();
		if (text.startsWith("http://",Qt::CaseInsensitive)||text.startsWith("https://",Qt::CaseInsensitive)) {
			emit urlPasted(text);
		}
	}
}
