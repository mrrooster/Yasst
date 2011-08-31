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
 * URLShortener.h
 *
 *  Created on: 28-Feb-2009
 *      Author: ian
 */

#ifndef URLSHORTENER_H_
#define URLSHORTENER_H_

#include "backend/HTTPHandler.h"
#include <QtXml/QDomDocument>

class URLShortener : public HTTPHandler {
	Q_OBJECT
public:
	URLShortener();
	virtual ~URLShortener();

	virtual void shortenURL(QString url);

signals:
	void shortURL(QString longUrl,QString shortUrl);

protected:
	void httpFinished(int i,bool error);
	void handleXml(QDomDocument *doc);
protected slots:
	virtual void handleHttpResponse(QString,QUrl,bool,int);
};

#endif /* URLSHORTENER_H_ */
