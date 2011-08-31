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
 * TweetTextHighlighter.cpp
 *
 *  Created on: 22-Feb-2009
 *      Author: ian
 */

#include "TweetTextHighlighter.h"
#include <QTextCharFormat>
#include <QUrl>

TweetTextHighlighter::TweetTextHighlighter(QTextDocument  *parent) : QSyntaxHighlighter(parent) {
	dmcount=0;
}

TweetTextHighlighter::~TweetTextHighlighter() {
}

void TweetTextHighlighter::highlightBlock(const QString &text) {
	int size = previousBlockState();
	int prev = size;
	TweetTextHighlighter::urlData *url = 0;
	QRegExp expression("https*://[^ ]+[^ .]");
	if (size<0) {
		size=text.size();
		prev=0;
	} else
		size+=text.size();

	if (size>140+dmcount) {
		QTextCharFormat sizeFormatter;
		sizeFormatter.setBackground(QColor(255,150,150));
		sizeFormatter.setForeground(QBrush(QColor(0,0,0)));

		setFormat(0,140+dmcount-prev,sizeFormatter);
	}
	setCurrentBlockState(size+1);

	QTextCharFormat urlFormatter;
	urlFormatter.setBackground(Qt::yellow);
	QTextCharFormat shortUrlFormatter;
	shortUrlFormatter.setBackground(Qt::green);
	int index = text.indexOf(expression);
	while (index >=0 ) {
		int length = expression.matchedLength();
		QString urlText = expression.cap(0);
		QString urlHost=QUrl(urlText).host();
		bool ignore=false;

		if (urlHost=="bit.ly"||urlHost=="twitpic.com"||urlText.length()<25)
			ignore=true;

		if (!ignore) {
			if (!url) {
				url = new TweetTextHighlighter::urlData();
			}
			url->urls.append(urlText);
			setFormat(index,length,urlFormatter);
		} else
			setFormat(index,length,shortUrlFormatter);
		index=text.indexOf(expression,index+length);
	}
//	delete currentBlockUserData();
//	if (url)
	setCurrentBlockUserData(url);
}

void TweetTextHighlighter::setDirectMessagePrefix(int i) {
	dmcount=i;
}
