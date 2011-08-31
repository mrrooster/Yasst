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
/* $Id: FeedFetcher.h,v 1.3 2010/07/25 14:51:22 ian Exp $

 File       : FeedFetcher.h
 Create date: 22:29:03 3 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef FEEDFETCHER_H_
#define FEEDFETCHER_H_

#include "TweetFetcher.h"
#include <QDomNode>

class FeedFetcher: public TweetFetcher {
	Q_OBJECT
public:
	FeedFetcher();
	virtual ~FeedFetcher();
	void populate();
	void setUrl(QString);
	QString getUrl();
protected:
	void handleXml(QDomDocument *doc,int,int);
	void handleRssXml(QDomNode );
	void handleRdfXml(QDomNode );
	void handleAtomXml(QDomNode );
	QString url;
private:
	QDateTime parseRssDate(QString);
	QDateTime parseRdfDate(QString);
signals:
	void feedTitle(QString);

};

#endif /* FEEDFETCHER_H_ */
