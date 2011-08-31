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
/* $Id: Translator.h,v 1.1 2010/07/11 10:05:45 ian Exp $

 File       : Translator.h
 Create date: 22:46:40 4 Jul 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include "HTTPHandler.h"
#include <QMap>

class Translator: public HTTPHandler {
	Q_OBJECT
public:
	Translator();
	virtual ~Translator();

	int translate(QString from,QString to,QString text);

	static QMap<QString,QString> getLanguages();
protected slots:
	virtual void handleHttpResponse(QString,QUrl,bool,int);

private:
	static QMap<QString,QString> lang;

signals:
	void haveTranslation(int id,QString from,QString to,QString translation);
};

#endif /* TRANSLATOR_H_ */
