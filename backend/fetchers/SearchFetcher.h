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
 * SearchFetcher.h
 *
 *  Created on: 12-Mar-2009
 *      Author: ian
 */

#ifndef SEARCHFETCHER_H_
#define SEARCHFETCHER_H_

#include "TweetFetcher.h"

class SearchFetcher: public TweetFetcher {
	Q_OBJECT
public:
	SearchFetcher();
	virtual ~SearchFetcher();
	void populate();
	void setSearch(QString search);
	QString getSearch();
	virtual void handleHttpResponse(QString,QUrl,bool,int);
	//void handleHttpResponse(QString resp,QUrl url,bool isSend);
signals:
	void haveResults(QList<QString>);
protected:
	QDateTime parseSearchDate(QString);
private:
	QString search;
	QString sinceId;
};

#endif /* SEARCHFETCHER_H_ */
