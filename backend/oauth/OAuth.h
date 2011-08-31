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
/* $Id: OAuth.h,v 1.1 2010/06/19 20:29:41 ian Exp $

 File       : OAuth.h
 Create date: 19:46:02 27 Apr 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef OAUTH_H_
#define OAUTH_H_

//Qt
#include <QMap>
#include <QCryptographicHash>
//Mine
#include "../HTTPHandler.h"

class OAuth: public HTTPHandler {
	Q_OBJECT
	enum Phase { None,GetRequestToken,AwaitingPIN,GetAccessToken,Authorized,Failed };
public:
	OAuth();
	virtual ~OAuth();

	void setSecret(QString);
	void setConsumerKey(QString);
	void authorize();
	void setPIN(QString);
	QString getAuthToken();
	QString getAuthTokenSecret();
	QString getAuthorizationHeader(QString,QString,QString, QString realm="");
	void reset();
	void setTokenAndSecret(QString,QString);
	void setUser(QString u) {user=u;};
	QString getUser() {return user;};
protected:
	QString generateSigBaseString(QString method, QString Url, QMap<QString,QString> vars, QString data="");
	QString HMAC_SHA1(QString key, QString value);
	QString nonce();
	void begin(QString url);

	OAuth::Phase phase;
	QString secret;
	QString consumerKey;
	QString token;
	QString tokenSecret;
	QString user;

protected slots:
	virtual void handleHttpResponse(QString,QUrl,bool,int);
signals:
	void PINRequest(OAuth *,QString);
	void authenticatedUsername(QString);
};

#endif /* OAUTH_H_ */
