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
/* $Id: HTTPHandler.h,v 1.9 2010/09/14 13:49:05 ian Exp $
File:   HTTPHandler.h
(c) 2009 Ian Clark

This is the base class for HTTP ops in Yasst.

This used to use the new QNetworking classes but I had trouble with them and twitter
in 4.4.3, so I've converted to the QHttp class which is working OK.

This is now the base class for all HTTP ops in Yasst. Subclass either of the
'handleHttpResponse' methods for your own use.
*/

#ifndef HTTPHANDLER_H_
#define HTTPHANDLER_H_
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHash>
#include <QMap>
#include <QMutex>
#include <QBuffer>
#include <QUrl>
#include <QSslError>

class HTTPHandler : public QObject{
	Q_OBJECT
public:
	HTTPHandler();
	virtual ~HTTPHandler();
	static QString htmlEncode(QString);
	static QString htmlDecode(QString);
	static QHash<QString,QString> paramDecode(QString data);
	static QString userAgent;
	int type;
signals:
	void authError(HTTPHandler*);
	void otherError(HTTPHandler*);
protected slots:
	virtual void httpFinished(int,bool);
	virtual void handleHttpResponse(QBuffer*,QUrl,bool,int);
	virtual void handleHttpResponse(QString,QUrl,bool,int) {};
	virtual void handleDone(bool);
	virtual void handleFinished  ( QNetworkReply * reply );
	virtual void handleAuthenticationRequired(QNetworkReply *, QAuthenticator *);
	virtual void handleSslErrors (QNetworkReply * , const QList<QSslError> &  );
protected:
	virtual int doHttpBit(QString URL,QString user="",QString pass="", QString data="", QString mimetype= "", QString method="");
	int doHttpBit(QUrl url,QString user="",QString pass="", QString data="", QString mimetype= "", QString method="");
	int doHttpBit(QUrl url, QString user, QString pass, QByteArray data, QString mimetype= "", QString method="");
	int doHttpBit(QUrl url, QString user, QString pass, QByteArray data, QString mimetype, QString method, int);
	QString getMimeType(QString filename);
	QString getResponseHeader(int id, QString name);

	QMap<QString,QString> headers;
	QMap<QString,QString> multipartFormData;
	QMap<QString,QString> multipartFormFiles;
	QHash<QNetworkReply*,QString> users;
	QHash<QNetworkReply*,int> id;
	QHash<int,QBuffer*> myReq;
	QHash<int,bool> mySendFlag;
	QHash<int,QUrl> myUrl;
	QHash<QNetworkReply*,QByteArray> myData;
	QHash<QNetworkReply*,QString> myMethod;
	QHash<QNetworkReply*,QString> myMime;
	QHash<QNetworkReply*,QString> passwords;
	QHash<int,QNetworkReply*> replys;
	static int requestCount;
	QNetworkAccessManager network;
};

#endif /* HTTPHANDLER_H_ */
