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
/* $Id: TwitrpixImageHandler.h,v 1.4 2010/07/11 10:05:45 ian Exp $

 File       : TwitrpixImageHandler.h
 Create date: 20:12:02 31-May-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef TWITRPIXIMAGEHANDLER_H_
#define TWITRPIXIMAGEHANDLER_H_

#include "backend/fetchers/TweetFetcher.h"

class TwitrpixImageHandler: public TweetFetcher {
	Q_OBJECT
public:
	TwitrpixImageHandler();
	virtual ~TwitrpixImageHandler();

	void tweet(QString ,QString ="",QString="");
/* - provided by TweetFetcher, here for ref. (They make more sense here, but having them in the base class stops the annoying warnings on the console.)
signals:
	void imageUploadStarted();
	void imageUploadEnded();
*/
protected:
	void handleXml(QDomDocument *doc,int,int);
	virtual bool okToPost();
	QString source;
	QString seperator;
protected slots:
	virtual void httpFinished(int,bool);
private:
	QString msg;
	QHash<int,QString> sentTweet;
	QHash<int,QString> sentImage;
};

#endif /* TWITRPIXIMAGEHANDLER_H_ */
