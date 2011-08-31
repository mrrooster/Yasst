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
/* $Id: OAuth.cpp,v 1.4 2010/08/01 18:15:36 ian Exp $

 File       : OAuth.cpp
 Create date: 19:46:02 27 Apr 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

// Qt
#include <QMapIterator>
#include <QMessageBox>
#include <QApplication>
#include <QUuid>
#include <QDateTime>
#include <QDesktopServices>
#include <QDebug>
#include "OAuth.h"

OAuth::OAuth() : HTTPHandler() {
	phase=OAuth::None;
	this->setConsumerKey("1U7luU32YCT81vQc5aWAA");
	this->setSecret("4Qn51IfsNR27FQEjJXDz1ewYK68siF3n6VlnaGoJv8");
	//test
	//this->setConsumerKey("GDdmIQH6jhtmLUypg82g");
	//this->setSecret("MCD8BKwGdgPHvAuvgvz4EQpqDAtx89grbuNMRd7Eh98");
	token="";
	tokenSecret="";
}

OAuth::~OAuth() {
}

void OAuth::setSecret(QString s) {
	secret=s;
}

void OAuth::setConsumerKey(QString s) {
	consumerKey=s;
}

void OAuth::reset() {
	token=tokenSecret="";
	phase=OAuth::None;
}

void OAuth::authorize() {
//	qDebug("==== In OAuth::authorize");
	if (phase==OAuth::None)
		begin("http://twitter.com/oauth/request_token");
}

void OAuth::begin(QString url) {
//	qDebug("==== In OAuth::begin.");
	QMap<QString,QString> vars;

/*
	QMessageBox::information(0,"",HMAC_SHA1("MCD8BKwGdgPHvAuvgvz4EQpqDAtx89grbuNMRd7Eh98&", "POST&https%3A%2F%2Fapi.twitter.com%2Foauth%2Frequest_token&oauth_callback%3Dhttp%253A%252F%252Flocalhost%253A3005%252Fthe_dance%252Fprocess_callback%253Fservice_provider_id%253D11%26oauth_consumer_key%3DGDdmIQH6jhtmLUypg82g%26oauth_nonce%3DQP70eNmVz8jvdPevU3oJD2AfF7R7odC2XJcn4XlZJqk%26oauth_signature_method%3DHMAC-SHA1%26oauth_timestamp%3D1272323042%26oauth_version%3D1.0"));
	QMessageBox::information(0,"",HMAC_SHA1("cFmFnaVm0V9ylkJloer85P116VIcZSfM64E3TDFs&", "GET&http%3A%2F%2Fapi.twitter.com%2Foauth%2Frequest_token&oauth_callback%3Dhttp%253A%252F%252Fexample.com%252Ffoo%26oauth_consumer_key%3DY8V8g65nR6bt1CGXpHdR4Q%26oauth_nonce%3DriU6Q6qAByV7RL4wLhtbsoQWMHQ%253D%26oauth_signature_method%3DHMAC-SHA1%26oauth_timestamp%3D1273441057%26oauth_version%3D1.0"));
	QApplication::exit();
*/
//	qDebug("========== Begin OAuth flow ===========");
//	qDebug()<<"Consumer key   : "<<consumerKey;
//	qDebug()<<"Consumer secret: "<<secret;
//	qDebug("State now: Get Request Token");

	vars.insert("oauth_callback","oob");
	vars.insert("oauth_consumer_key",consumerKey);
	vars.insert("oauth_nonce",nonce());
	vars.insert("oauth_signature_method","HMAC-SHA1");
	vars.insert("oauth_timestamp",QString::number(QDateTime::currentDateTime().toTime_t()));
	vars.insert("oauth_version","1.0");
	vars.insert("oauth_signature",HMAC_SHA1(QUrl::toPercentEncoding(secret)+"&",generateSigBaseString("GET",url,vars)));

//	qDebug() << "Variables : " << vars;
	phase=OAuth::GetRequestToken;

	QString out,post;
	QMapIterator<QString,QString> it(vars);
	while(it.hasNext()) {
		it.next();
		QString key = QUrl::toPercentEncoding(it.key());
		QString value = QUrl::toPercentEncoding(it.value());
//		QString value = it.value();
		out+=((out!="")?", ":"")+key+"=\""+value+"\"";
		post+=((post!="")?"&":"")+key+"="+value+"";
	}
//	qDebug()<<"Header  : "<<"OAuth "<<out;
 	headers.insert("Authorization","OAuth "+out);
// 	for(int x=50;x<out.size();x+=50)
// 		out.insert(x++,"\n");
// 	QMessageBox::information(0,"",out);
	doHttpBit(url,"","","");
}

void OAuth::setPIN(QString pin) {
//	qDebug("==== In OAuth::setPIN");

	if (phase==OAuth::AwaitingPIN) {
		QMap<QString,QString> vars;
		QString url = "http://twitter.com/oauth/access_token";
		vars.insert("oauth_consumer_key",consumerKey);
		vars.insert("oauth_nonce",nonce());
		vars.insert("oauth_signature_method","HMAC-SHA1");
		vars.insert("oauth_timestamp",QString::number(QDateTime::currentDateTime().toTime_t()));
		vars.insert("oauth_version","1.0");
		vars.insert("oauth_verifier",pin);
		vars.insert("oauth_token",token);
		vars.insert("oauth_signature",HMAC_SHA1(QUrl::toPercentEncoding(secret)+"&"+QUrl::toPercentEncoding(tokenSecret),generateSigBaseString("GET",url,vars)));

		phase=OAuth::GetAccessToken;
	//	qDebug("======= Phase: Get Access Token\n");
	//	qDebug() << "Vars: " << vars;

		QString out,post;
		QMapIterator<QString,QString> it(vars);
		while(it.hasNext()) {
			it.next();
			QString key = QUrl::toPercentEncoding(it.key());
			QString value = QUrl::toPercentEncoding(it.value());
			out+=((out!="")?", ":"")+key+"=\""+value+"\"";
			post+=((post!="")?"&":"")+key+"="+value+"";
		}
	//	qDebug()<<"Header  : "<<"OAuth "<<out;
	 	headers.insert("Authorization","OAuth "+out);
		doHttpBit(url,"","","");
	}
//	qDebug("==== exit setPIN");
}

QString OAuth::getAuthorizationHeader(QString url,QString method,QString data,QString realm) {
//	qDebug("==== In OAuth::getAuthorizationHeader");
	if (phase!=OAuth::Authorized) {
//		qDebug("In wrong phase, returning, may authorize()");
		if (phase==OAuth::None)
			authorize();
		return "";
	}

	QMap<QString,QString> vars;

//	vars.insert("realm","http://api.twitter.com/");
	vars.insert("oauth_consumer_key",consumerKey);
	vars.insert("oauth_nonce",nonce());
	vars.insert("oauth_signature_method","HMAC-SHA1");
	vars.insert("oauth_timestamp",QString::number(QDateTime::currentDateTime().toTime_t()));
	vars.insert("oauth_version","1.0");
	vars.insert("oauth_token",token);
	vars.insert("oauth_signature",HMAC_SHA1(QUrl::toPercentEncoding(secret)+"&"+QUrl::toPercentEncoding(tokenSecret),generateSigBaseString(method,url,vars,data)));

//	qDebug() << "Vars: " << vars;
	QString out,post;
	QMapIterator<QString,QString> it(vars);
	while(it.hasNext()) {
		it.next();
		QString key = it.key();
		QString value = QUrl::toPercentEncoding(it.value());
		out+=((out!="")?", ":"")+key+"=\""+value+"\"";
		post+=((post!="")?"&":"")+key+"="+value+"";
	}
	if (realm!="") {
		out = "realm=\""+realm+"\""+((out!="")?", ":"")+out;
	}

//	qDebug()<<"Header  : "<<"OAuth "<<out;
	return "OAuth "+out;
}

QString OAuth::nonce() {
//	return "453235szdfsff3";
//	k=QCryptographicHash::hash(QString::number(QDateTime::currentDateTime().toTime_t()).toUtf8(),QCryptographicHash::Sha1);
	QString nonceSense = QUuid::createUuid().toString()+QString::number(QDateTime::currentDateTime().toTime_t());
	return QString(QCryptographicHash::hash(nonceSense.toUtf8(),QCryptographicHash::Sha1).toBase64());
}

QString OAuth::generateSigBaseString(QString method, QString url, QMap<QString,QString> vars, QString data) {
//	qDebug("==== In OAuth::generateSigBaseString");
//	qDebug()<<"Method   : "<<method;
//	qDebug()<<"URL      : "<<url;
//	qDebug() << "vars     :" << vars;
//	qDebug()<<"Data     : "<<data;
	QUrl u(url);
//	QMessageBox::information(0,"",url);
	url = u.toString(QUrl::RemoveQuery);
//	QMessageBox::information(0,"",url);
	QString base = method+"&"+QUrl::toPercentEncoding(url)+"&";
	QHash<QString,QString> post = HTTPHandler::paramDecode(data);
	QHashIterator<QString,QString> postit(post);
	if (post.count()>0)
		data="";
	while(postit.hasNext()) {
		postit.next();
		vars.insert(postit.key(),postit.value());
	}
	QHashIterator<QString,QString> urlpostit(HTTPHandler::paramDecode(u.toString(QUrl::StripTrailingSlash|QUrl::RemoveFragment|QUrl::RemovePath|QUrl::RemoveAuthority|QUrl::RemovePort|QUrl::RemoveUserInfo|QUrl::RemoveScheme).mid(1)));
	while(urlpostit.hasNext()) {
		urlpostit.next();
		vars.insert(urlpostit.key(),urlpostit.value());
	}

	QMapIterator<QString,QString> it(vars);
	QString out;

//	qDebug() << "vars2     :" << vars;
//	qDebug() << "Data2     :" << data;
	while(it.hasNext()) {
		it.next();
		QString key = QUrl::toPercentEncoding(it.key());
		QString value = QUrl::toPercentEncoding(it.value());
//		QString value = it.value();
		out+=((out=="")?"":"&")+key+"="+value;
	}
//	if (data!="") {
//		out+=((out=="")?"":"&")+data;
//	}
//	QMessageBox::information(0,"",base+QUrl::toPercentEncoding(out));
//	qDebug()<<"Sig: "<<base+QUrl::toPercentEncoding(out);
	return base+QUrl::toPercentEncoding(out);
}

QString OAuth::HMAC_SHA1(QString key, QString value) {
//	QApplication::clipboard()->setText(key+"  "+value);
//	qDebug("==== In OAuth::HMAC_SHA1");
//	qDebug()<<"Key   : "<<key;
//	qDebug()<<"Value : "<<value;
	QByteArray k;
	QByteArray l;
	int blockSize=64;

	k.clear();
	k.append(key);
	while (k.size()<blockSize)
		k.append((char)0);

	if (k.size()>blockSize)
		k=QCryptographicHash::hash(k,QCryptographicHash::Sha1);

	QByteArray ipad(k);
	QByteArray opad(k);

	for(int x=0;x<blockSize;x++) {
		ipad[x]=ipad[x]^0x36;
		opad[x]=opad[x]^0x5c;
	}

	l.clear();
	l.append(ipad,blockSize);
	l.append(value);

	QByteArray i = QCryptographicHash::hash(l,QCryptographicHash::Sha1);

	l.clear();
	l.append(opad,blockSize);
	l.append(i);

	i = QCryptographicHash::hash(l,QCryptographicHash::Sha1);

//	qDebug()<<"Result: "<<i.toBase64();
	return i.toBase64();
}

void OAuth::handleHttpResponse(QString resp,QUrl ,bool ,int ) {
//	qDebug("==== In OAuth::handleHttpResponse");
//	qDebug()<<"URL      : "<<url;
//	qDebug()<<"Response : "<<resp;
//	qDebug()<<"State    : "<<((phase==OAuth::GetRequestToken)?"Get request token":((phase==OAuth::GetAccessToken)?"Get access token":"Unknown"));
	QHash<QString,QString> vars = HTTPHandler::paramDecode(resp);
	if (phase==OAuth::GetRequestToken) {
		token=vars.value("oauth_token");
		tokenSecret=vars.value("oauth_token_secret");
	//	qDebug()<<"Got auth_token: '"<<token<<"', and secret: '"<<tokenSecret<<"'";
		phase=OAuth::AwaitingPIN;
	//	qDebug("State is now: Request PIN");
		emit PINRequest(this,"https://api.twitter.com/oauth/authorize?oauth_token="+token);
	} else if (phase==OAuth::GetAccessToken) {
		token=vars.value("oauth_token");
		tokenSecret=vars.value("oauth_token_secret");
	//	qDebug()<<"Got auth_token: '"<<token<<"', and secret: '"<<tokenSecret<<"'";
		if (token!="" && tokenSecret != "") {
			phase=OAuth::Authorized;
			emit authenticatedUsername(vars.value("screen_name"));
		//	qDebug("State is now: Authorized.");
		} else
			phase=OAuth::Failed;

		//	qDebug("Auth failed!");
//		QMessageBox::information(0,"","Authorized_"+token);
	} else {
//		QMessageBox::information(0,url.toString(),QString::number(resp.length())+":"+resp+":");
//		QApplication::exit();
	}
}

QString OAuth::getAuthToken() {
	if (phase==OAuth::Authorized)
		return token;
	return "";
}

QString OAuth::getAuthTokenSecret() {
	if (phase==OAuth::Authorized)
		return tokenSecret;
	return "";
}

void OAuth::setTokenAndSecret(QString t,QString s) {
	if (phase!=OAuth::None || t=="" || s=="")
		return;
//	qDebug("==== In OAuth::setTokenAndSecret");
//	qDebug()<<"New token   : "<<t;
//	qDebug()<<"Token secret: "<<s;
//	qDebug()<<"State is now: Authorized.";

	token=t;
	tokenSecret=s;
	phase=OAuth::Authorized;
}
