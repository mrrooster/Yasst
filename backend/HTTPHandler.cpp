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
/* $Id: HTTPHandler.cpp,v 1.11 2010/09/14 13:49:05 ian Exp $
File:   HTTPHandler.h
(c) 2009 Ian Clark

*/

#include "HTTPHandler.h"
#include <QUrl>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

QString HTTPHandler::userAgent="Yasst";
int HTTPHandler::requestCount=0;
//QNetworkAccessManager HTTPHandler::network;

HTTPHandler::HTTPHandler() {
	type=0;
	QObject::connect(&network,SIGNAL(finished(QNetworkReply *)),this,SLOT(handleFinished(QNetworkReply *)));
	QObject::connect(&network,SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),this,SLOT(handleAuthenticationRequired(QNetworkReply *, QAuthenticator *)));
	QObject::connect(&network,SIGNAL(sslErrors(QNetworkReply *,const QList<QSslError>&)),this,SLOT(handleSslErrors (QNetworkReply *,const QList<QSslError> &)));
}

HTTPHandler::~HTTPHandler() {
}

int HTTPHandler::doHttpBit(QString URL,QString user,QString pass, QString data, QString mimetype, QString method) {
	return doHttpBit(QUrl(URL),user,pass,data,mimetype,method);
}

int HTTPHandler::doHttpBit(QUrl urlHolder,QString user,QString pass, QString data, QString mimetype, QString method) {
	return doHttpBit(urlHolder,user,pass,data.toUtf8(),mimetype,method);
}

int HTTPHandler::doHttpBit(QUrl urlHolder,QString user,QString pass, QByteArray data, QString mimetype, QString method) {
	int id = HTTPHandler::requestCount++;
	return doHttpBit(urlHolder,user,pass,data,mimetype,method,id);
}

int HTTPHandler::doHttpBit(QUrl urlHolder,QString user,QString pass, QByteArray data, QString mimetype, QString method, int id) {
	QNetworkReply *reply;
	QNetworkRequest req;
//	headers.insert("Accept-Encoding"," ");
	QMapIterator<QString,QString> it(headers);

	req.setUrl(urlHolder);
	while (it.hasNext()) {
		it.next();
		req.setRawHeader(it.key().toAscii(),it.value().toAscii());
	}
	req.setRawHeader(QByteArray("User-Agent"),HTTPHandler::userAgent.toAscii());

	// Multipart form?
	if (multipartFormData.size()>0||multipartFormFiles.size()>0) {
		//qDebug() << "== Http handler has multipart form data.";
		QString boundry="345AJjiof95p__httphandler_boundry_"+HTTPHandler::userAgent.toAscii()+"_"+QString::number(QDateTime::currentDateTime().toTime_t());
		QMapIterator<QString,QString> formIt(multipartFormData);
		QMapIterator<QString,QString> filesIt(multipartFormFiles);
		data.clear();

		while(formIt.hasNext()) {
			formIt.next();
			data+="--"+boundry+"\r\n";

			data+="content-disposition: form-data; name=\""+formIt.key()+"\"\r\n\r\n"+formIt.value()+"\r\n";
		}
		//qDebug() << "== Data:\n"<<data;
		while(filesIt.hasNext()) {
			filesIt.next();
			data+="--"+boundry+"\r\n";

			QFile file(filesIt.value());
			QFileInfo info(file);
			file.open(QIODevice::ReadOnly);
			data+="content-disposition: form-data; name=\""+filesIt.key()+"\"; filename=\""+info.fileName()+"\"\r\nContent-Transfer-Encoding: binary\r\nContent-type: "+getMimeType(info.fileName())+"\r\n\r\n";
			data+=file.readAll()+"\r\n";
			file.close();
		}
		data+="--"+boundry+"--\r\n";
		mimetype="multipart/form-data; boundary="+boundry;
		multipartFormData.clear();
		multipartFormFiles.clear();
	}

	if (mimetype!="")
		req.setHeader(QNetworkRequest::ContentTypeHeader,mimetype);
	if (method=="") {
		if (data.size()==0)
			method="GET";
		else
			method="POST";
	}
	if (method=="GET")
		reply = HTTPHandler::network.get(req);
	else if (method=="POST")
		reply = HTTPHandler::network.post(req,data);
	else if (method=="DELETE") {
		reply = HTTPHandler::network.deleteResource(req);
	}
	users.insert(reply,user);
	passwords.insert(reply,pass);
//	int id = HTTPHandler::requestCount++;
	mySendFlag.insert(id,(data.size()>0));
	myUrl.insert(id,urlHolder);
	myData.insert(reply,data);
	myMethod.insert(reply,method);
	myMime.insert(reply,mimetype);
	this->id.insert(reply,id);
	return id;
}

void HTTPHandler::httpFinished(int i,bool error) {
	QBuffer *buff;
//	reqLock.lock();
	bool isSend;

	if (myReq.contains(i)) {
		buff=myReq.take(i);
		isSend=mySendFlag.take(i);
		if (!error) {
			handleHttpResponse(buff,myUrl.take(i),isSend,i);
		}/* else if (http->error()==QHttp::AuthenticationRequiredError)
			emit authError(this);*/
		else {
			emit otherError(this);
		}
//		delete buff;
//		else if (!isSend)
//			emit updateFinished(0,(HTTPHandler*)this);

	//	http->deleteLater();
		QNetworkReply *r = replys.take(i);
		if (r)
			r->deleteLater();
	}
//	reqLock.unlock();
}

QString HTTPHandler::getResponseHeader(int id, QString name) {
	QNetworkReply *r = replys.value(id);

	if (r) {
		if (r->hasRawHeader(name.toAscii()))
			return QString(r->rawHeader(name.toAscii()));
	}
	return "";
}

void HTTPHandler::handleHttpResponse(QBuffer *b,QUrl u,bool s,int r) {
	QByteArray data = b->data();
	handleHttpResponse(QString::fromUtf8(data),u,s,r);
//	b->deleteLater();
	delete b;
}

QString HTTPHandler::htmlEncode(QString text) {
	long otherPos=0;
	QRegExp amp("&");
	QRegExp lt("<");
	QRegExp gt(">");

	// Must be first. :)
	while ((otherPos=amp.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+"&amp;"+text.mid(otherPos+1,-1);
	otherPos = 0;
	while ((otherPos=lt.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+"&lt;"+text.mid(otherPos+1,-1);
	otherPos = 0;
	while ((otherPos=gt.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+"&gt;"+text.mid(otherPos+1,-1);
	otherPos = 0;
	return text;
}

QString HTTPHandler::htmlDecode(QString text) {
	long otherPos=0;
	QRegExp deLt("&lt;");
	QRegExp deGt("&gt;");
	QRegExp deAmp("&amp;");

	while ((otherPos=deLt.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+"<"+text.mid(otherPos+4,-1);
	otherPos = 0;
	while ((otherPos=deGt.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+">"+text.mid(otherPos+4,-1);
	// Must be last. :)
	otherPos = 0;
	while ((otherPos=deAmp.indexIn(text,otherPos+1))>=0)
		text=text.left(otherPos)+"&"+text.mid(otherPos+5,-1);
	return text;
}

void HTTPHandler::handleDone(bool) {
}

void HTTPHandler::handleFinished  ( QNetworkReply * reply ) {
	if (!this->id.contains(reply))
		return;
	// Check for redirect
	QVariant redr = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	int id = this->id.take(reply);
	if (!redr.isNull()) {
		QUrl url = myUrl.value(id).resolved(redr.toUrl());
		doHttpBit(url,users.take(reply),passwords.take(reply),myData.take(reply),myMime.take(reply),myMethod.take(reply),id);
	}
	bool error=false;
	users.take(reply);
	passwords.take(reply);
	myData.take(reply);
	myMethod.take(reply);
	myMime.take(reply);
	replys.insert(id,reply);
//	if (reply->error()!=QNetworkReply::NoError) {
//		error=true;
//		handleDone(true);
//	} else {
//		QMessageBox::information(0,"e",QString(reply->readAll()));
//		QApplication::exit(0);

		QBuffer *b=new QBuffer();
		b->setData(reply->readAll());
		myReq.insert(id,b);
//		handleHttpResponse(b,reply->url(),mySendFlag.value(id),id);
		httpFinished(id,reply->error()!=QNetworkReply::NoError);
		handleDone(reply->error()!=QNetworkReply::NoError);
		if (reply->error()!=QNetworkReply::NoError) {
			qDebug() << "HTTP finished with error: " << reply->errorString();
			qDebug() << "Error payload: "<<QString(reply->readAll());
			qDebug() << "URL: "<<reply->url();
		}
//	}
//	httpFinished(id,error);
	reply->deleteLater();
}

void HTTPHandler::handleAuthenticationRequired(QNetworkReply *net, QAuthenticator *auth) {
//	qDebug("==== In HTTPHandler::handleAuthenticationRequired");
	if (!this->id.contains(net))
		return;
	QString user = users.value(net);
	QString pass = passwords.value(net);

//	QMessageBox::information(0,"",QString(net->readAll()));
	if (user==""&&pass=="")
		return;

	auth->setUser(user);
	auth->setPassword(pass);
}

void HTTPHandler::handleSslErrors (QNetworkReply *net,const QList<QSslError> &errors ) {
	if (!this->id.contains(net))
		return;
	for (int x=0;x<errors.count();x++) {
//		QMessageBox::information(0,"Error",errors.at(x).errorString());
//		qDebug() << "SSL Error "<<x<<": "<<errors.at(x).errorString();
	}
}

QHash<QString,QString> HTTPHandler::paramDecode(QString data) {
	QHash<QString,QString> vars;
	QStringList varlist = data.split("&");
	for(int x=0;x<varlist.length();x++) {
		QStringList bits = varlist[x].split("=");
		if (bits.size()==2) {
			vars.insert(QUrl::fromPercentEncoding(bits[0].toUtf8()),QUrl::fromPercentEncoding(bits[1].toUtf8()));
		}
	}
	return vars;
}

QString HTTPHandler::getMimeType(QString filename) {
	QString type=filename.mid(filename.lastIndexOf('.')).toLower();
	if (type==".jpg"||type==".jpeg"||type==".jpe")
		return "image/jpeg";
	if (type==".png")
		return "image/png";
	if (type==".gif")
		return "image/gif";
	if (type==".bmp")
		return "image/bmp";
	return "application/octet-stream";
}
