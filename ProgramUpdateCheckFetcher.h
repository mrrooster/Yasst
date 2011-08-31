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
/* $Id: ProgramUpdateCheckFetcher.h,v 1.3 2009/09/21 00:53:02 ian Exp $

 File       : ProgramUpdateCheckFetcher.h
 Create date: 20:44:04 23-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef PROGRAMUPDATECHECKFETCHER_H_
#define PROGRAMUPDATECHECKFETCHER_H_

#include "backend/fetchers/TweetFetcher.h"

class ProgramUpdateCheckFetcher: public TweetFetcher {
public:
	ProgramUpdateCheckFetcher();
	virtual ~ProgramUpdateCheckFetcher();
	void populate();
	void setVersion(QString);
	QString getVersion();
protected:
	void handleXml(QDomDocument *doc,int type,int);

	QString version;
};

#endif /* PROGRAMUPDATECHECKFETCHER_H_ */
