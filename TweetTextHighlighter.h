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
 * TweetTextHighlighter.h
 *
 *  Created on: 22-Feb-2009
 *      Author: ian
 */

#ifndef TWEETTEXTHIGHLIGHTER_H_
#define TWEETTEXTHIGHLIGHTER_H_
#include <QSyntaxHighlighter>
#include <QList>

class TweetTextHighlighter : public QSyntaxHighlighter {
public:
	TweetTextHighlighter(QTextDocument *);
	virtual ~TweetTextHighlighter();

	void highlightBlock(const QString &text);
	void setDirectMessagePrefix(int i);
	int dmcount;
	struct urlInfo {
		QString url;
		int position;
		int length;
	};

	struct urlData : public QTextBlockUserData
	{
		QList<QString> urls;
	};
};

#endif /* TWEETTEXTHIGHLIGHTER_H_ */
