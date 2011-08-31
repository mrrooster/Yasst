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
/* $Id: TweetFetcher.cpp,v 1.16 2010/09/14 13:49:05 ian Exp $
File:   tweetwidget.h
(c) 2009 Ian Clark

*/


#include "TweetFetcher.h"
#include "backend/BirdBox.h"
#include <QString>
#include <QList>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include "yasstFns.h"
#include <QDebug>
#include <QApplication>
#include <QNetworkProxy>
#include <QStringList>

//QFile *TweetFetcher::file = 0;


TweetFetcher::TweetFetcher() : oauth(0) {
//	manager = new QNetworkAccessManager(this);
//	QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(handleResponse(QNetworkReply *)));
	periodic=true;
	weight=100;
	timerActive=false;
	box=0;
	apiCount=0;
	inProgressCount=0;
	firstFetch=true;
	firstFetchCount=200;
	lastId="";
	statusPage=1;
	minTimeoutSecs=5;
	deleted=false;
	singleUsersAreFriends=true;
	trustUserFollowingStatus=false;
	cursor="";
	type=0;
	drainTimer.setSingleShot(false);
	refreshTimer.setSingleShot(false);
	refreshTimer.setInterval(30000);
	QObject::connect(&drainTimer,SIGNAL(timeout()),this,SLOT(handleDrainTimerFired()));
	QObject::connect(&refreshTimer,SIGNAL(timeout()),this,SLOT(doTimeout()));
}

TweetFetcher::~TweetFetcher() {
//	QObject::disconnect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(handleResponse(QNetworkReply *)));
//	delete manager;
}

void TweetFetcher::populate() {
//	manager.get(QNetworkRequest(QUrl("http://twitter.com/statuses/public_timeline.xml")));
	doHttpBit("http://twitter.com/statuses/public_timeline.xml","","","");
	emit startFetching();
	inProgressCount++;
}

/*
void TweetFetcher::handleResponse(QNetworkReply *r) {
	QString *data = new QString();
	QDomDocument *doc = new QDomDocument();
	QNetworkReply::NetworkError err = r->error();

	if (err==QNetworkReply::NoError) {
		data->append(r->readAll());
		doc->setContent(*data);
		handleXml(doc);
		firstFetch=false;
		delete doc;
	} else if (err==QNetworkReply::AuthenticationRequiredError) {
		emit authError();
	}
	r->deleteLater();
}
*/

void TweetFetcher::getRate() {
//	doHttpBit("http://api.twitter.com/1/account/rate_limit_status.xml",user,pass,"");
}

//void TweetFetcher::doHttpBit(QString resource,QString user,QString pass, QString data) {
//	QBuffer *buff=new QBuffer();
//	QHttp *http=new QHttp();
//	QString type="POST";
//	int req;
//
////	QMessageBox::information (0,"assdf",resource);
//
//	QObject::connect(http,SIGNAL(requestFinished(int,bool)),this,SLOT(httpFinished(int,bool)));
//	if (data=="")
//		type="GET";
//	QHttpRequestHeader header(type,resource);
//	if (data!="")
//		header.setContentType("application/x-www-form-urlencoded; charset=UTF-8");
////	http.setUser(user,pass);
//	header.setContentLength(data.size());
//	header.setValue("Host","twitter.com");
//	if (user!="") {
//		QString auth=user+":"+pass;
//		header.setValue("authorization","Basic "+auth.toLatin1().toBase64());
//	}
//	buff->open(QBuffer::ReadWrite);
//	buff->reset();
//	http->setHost("twitter.com",QHttp::ConnectionModeHttp);
//
//	reqLock.lock();
//	req=http->request(header,data.toUtf8(),buff);
//	myReq.insert(req,buff);
//	myHttp.insert(req,http);
//	mySendFlag.insert(req,(data!=""));
//	reqLock.unlock();
//}

void TweetFetcher::handleHttpResponse(QString resp,QUrl url,bool isSend,int req) {
//	qDebug() << "Http response [" << req << ":" << url <<"] :\n" <<resp;
//	QMessageBox::information(0,"",resp);
	QDomDocument *doc = new QDomDocument();
	int type=0; // 0-norm,1-send,2-favadd,3-favdestroy

	doc->setContent(resp);

	qDebug() << "[" << req << "]HTTP Response ------------------";
	qDebug() << "[" << req << "]URL: " << url;
	qDebug() << "[" << req << "]Response: "<< doc;
	qDebug() << "[" << req << "]HTTP Response end --------------";


	if (isSend) {
		type=1;
		if (url.path().startsWith("/favorites/create"))
			type=2;
		else if (url.path().startsWith("/favorites/destroy"))
			type=3;
	}
	handleXml(doc,type,req);
	delete doc;
	statusUsersAreFriends=false;
	if (!periodic)
		deleted=true;
	firstFetch=false;

}


void TweetFetcher::handleXml(QDomDocument *doc,int type,int req) {
	int count;
	QDomNode node = doc->firstChild().nextSibling();
	if (node.nodeName()=="statuses"||node.nodeName()=="status") {
		bool lastIdWasEmpty = (lastId=="");
		count = handleStatusesXml(doc,type);
		if (statusPage>1 && count==200 && !lastIdWasEmpty) {
			populate();
		} else {
			emit endFetching();
			if (inProgressCount)
				inProgressCount--;
			statusPage=1;
			if (type==0)//1==send
				emit updateFinished(count,this);
		}
	} else if (node.nodeName()=="users") {
		handleUsersXml(doc,!trustUserFollowingStatus);
	} else if (node.nodeName()=="users_list") {
		handleUsersXml(doc,!trustUserFollowingStatus);
	} else if (node.nodeName()=="user") {
		handleUserXml(node,singleUsersAreFriends);
	} else if (node.nodeName()=="hash") {
		handleHashXml(node);
	} else if (node.nodeName()=="errors") {
		//handleErrorXml(node);
//		QMessageBox::information(0,"Error",doc->toString());
		//<errors><error code="12">bla bla</error></errors>
	} else {
//		QMessageBox::information (0,"assdf",doc->toString());
		emit endFetching();
		if (inProgressCount)
			inProgressCount--;
		if (type>0)//1==send
			emit updateFinished(0,this);
		emit otherError(this);
		isError.insert(req,true);
	}
}

void TweetFetcher::handleUsersXml(QDomDocument *doc,bool are_friends) {
	QDomNodeList  nodes = doc->elementsByTagName("user");
	unsigned int x;

	for(x=0;x<nodes.length();x++) {
		QDomNode node = nodes.item(x);
		handleUserXml(node, are_friends);
	}
	nodes = doc->elementsByTagName("next_cursor");
	if (nodes.length()) {
		QDomNode node = nodes.item(0);
		if (!node.firstChild().isNull()) {
			cursor=node.firstChild().nodeValue();
			if (cursor=="0")
				cursor="";
		}
	} else
		cursor="";
	emit gotUsers(x);
}

int TweetFetcher::handleStatusesXml(QDomDocument *doc,int status) {
	QDomNodeList  nodes = doc->elementsByTagName("status");
	unsigned int x;
	int count=0;
	QList<Tweet*> tweets;

//	QDebug dbg(TweetFetcher::file);
//	dbg<<"Status XML:" << doc->toString() <<"\n";

	for(x=0;x<nodes.length();x++) {
		QDomNode node = nodes.item(x);
		QDomNodeList children = node.childNodes();
		Tweet *tweet = new Tweet();

		for (unsigned int y=0;y<children.length();y++){
			QDomNode child = children.item(y);
			if (child.nodeName()=="text") {
				tweet->message = child.firstChild().nodeValue();
			} else if (child.nodeName()=="id") {
				tweet->id = child.firstChild().nodeValue();
				if (tweet->id>lastId)
					lastId=tweet->id;
			} else if (child.nodeName()=="source") {
				tweet->source = child.firstChild().nodeValue();
			} else if (child.nodeName()=="in_reply_to_status_id") {
				tweet->in_reply_to_status_id = child.firstChild().nodeValue();
			} else if (child.nodeName()=="in_reply_to_user_id") {
				tweet->in_reply_to_user_id = child.firstChild().nodeValue();
			} else if (child.nodeName()=="in_reply_to_screen_name") {
				tweet->in_reply_to_screen_name = child.firstChild().nodeValue();
			} else if (child.nodeName()=="user") {
				handleUserXml(child,(trustUserFollowingStatus)?false:statusUsersAreFriends,tweet);
			} else if (child.nodeName()=="favorited") {
				switch(status) {
				case 2:
					tweet->favorite=true;
					break;
				case 3:
					tweet->favorite=false;
					break;
				default:
					tweet->favorite=(child.firstChild().nodeValue()=="true");
					break;
				}
			} else if (child.nodeName()=="created_at") {
				//Thu Feb 19 00:20:20 +0000 2009
				QString dateStr = child.firstChild().nodeValue().trimmed();
				tweet->created_at = parseTwitterDate(dateStr);
//				dbg<<"Tweeet date: "<<dateStr<<"\n";
//				QString hour = dateStr.mid(20,5);
//				dateStr=dateStr.left(20)+dateStr.right(4);
//				dbg<<"tweet date after offset removal: "<<dateStr<<"\n";
				//QMessageBox::information (0,"assdf",hour);
				//				tweet->created_at=QDateTime::fromString(dateStr,"ddd MMM dd HH:mm:ss +0000 yyyy");
//				tweet->created_at=QDateTime::fromString(dateStr,"ddd MMM dd HH:mm:ss yyyy");
//				tweet->created_at.setTimeSpec(Qt::UTC);
			} else if (child.nodeName()=="retweeted_status") {
				tweet->retweet=true;
				QDomNodeList retweetNodes = child.childNodes();
				for(unsigned int z=0;z<retweetNodes.length();z++) {
					QDomNode retweetNode = retweetNodes.at(z);
					if (retweetNode.nodeName()=="text") {
						tweet->retweet_message = retweetNode.firstChild().nodeValue();
					} else if (retweetNode.nodeName()=="source") {
						tweet->retweet_source = retweetNode.firstChild().nodeValue();
					} else if (retweetNode.nodeName()=="in_reply_to_status_id") {
						tweet->retweet_in_reply_to_status_id = retweetNode.firstChild().nodeValue();
					} else if (retweetNode.nodeName()=="in_reply_to_user_id") {
						tweet->retweet_in_reply_to_user_id = retweetNode.firstChild().nodeValue();
					} else if (retweetNode.nodeName()=="in_reply_to_screen_name") {
						tweet->retweet_in_reply_to_screen_name = retweetNode.firstChild().nodeValue();
					} else if (retweetNode.nodeName()=="user") {
						TweetUser *rtuser = handleUserXml(retweetNode,false,0);
						if (rtuser->id!="") {
							tweet->retweet_screen_name=rtuser->screen_name;
						}
					}
				}
			}
		}
		//QMessageBox::information (0,"assdf",tweet->created_at.toString());

		if (tweet->id!="") {
			count++;
			//emit haveTweet(tweet);
			tweets.append(tweet);
		}
	}
	emit haveTweets(tweets);
//	file.close();
//	QApplication::exit(0);
	return count;
}

void TweetFetcher::handleHashXml(QDomNode &child) {
	QDomNodeList hashchildren = child.childNodes();
	long count=0,secondsToReset=0;
	bool haveRate=false;

	for (unsigned int z=0;z<hashchildren.length();z++){
		QDomNode hashchild = hashchildren.item(z);

		if (hashchild.nodeName()=="remaining-hits") {
			count=hashchild.firstChild().nodeValue().toLong();
			haveRate=true;
		} else if (hashchild.nodeName()=="reset-time-in-seconds") {
			secondsToReset=hashchild.firstChild().nodeValue().toLong();
			secondsToReset-=QDateTime::currentDateTime().toTime_t();
			haveRate=true;
		} else if (hashchild.nodeName()=="error") {
			QString error = hashchild.firstChild().nodeValue();
			if (error.startsWith("Rate limit exceeded.")) {
				if (inProgressCount)
					inProgressCount--;
				emit limitExceeded();
				emit updateFinished(count,this);
			} else if (error=="Could not authenticate you.") {
				emit authError(this);
			} else {
				emit twitterError(error);
			}
		}
	}
	if (haveRate)
		emit rateUpdate(count,(secondsToReset<60?60:secondsToReset));
}

TweetUser* TweetFetcher::handleUserXml(QDomNode &child,bool is_friend,Tweet *tweet) {
	QDomNodeList userchildren = child.childNodes();
	TweetUser *user = new TweetUser();
	bool newUser=true;
	user->is_friend=is_friend;
	for (unsigned int z=0;z<userchildren.length();z++){
		QDomNode userchild = userchildren.item(z);

		if (userchild.nodeName()=="id") {
			user->id=userchild.firstChild().nodeValue();
			if (tweet)
				tweet->user_id=user->id;
			if (box && box->getUser(user->id)) {
				QString id=user->id;
				delete user;
				newUser=false;
				user=box->getUser(id);
				if (!user->am_unfollowing) {
					if (is_friend||(user->screen_name==this->user))
						user->is_friend=true;
				}
				// Only set if we know they're a friend, don't clear otherwise.
			}
		}
		if (userchild.nodeName()=="name") {
			user->name=userchild.firstChild().nodeValue();
		}
		if (userchild.nodeName()=="screen_name") {
			user->screen_name=userchild.firstChild().nodeValue();
			if (tweet)
				tweet->screen_name=user->screen_name;
			if (box && box->getUserByScreenName(user->screen_name)) {
				QString id=user->screen_name;
				if (newUser)
					delete user;
				newUser=false;
				user=box->getUserByScreenName(id);
				if (!user->am_unfollowing) {
					if (is_friend||(user->screen_name==this->user))
						user->is_friend=true;
				}
				// Only set if we know they're a friend, don't clear otherwise.
			}
		}
		if (userchild.nodeName()=="description") {
			user->description=userchild.firstChild().nodeValue();
		}
		if (userchild.nodeName()=="location") {
			user->location=userchild.firstChild().nodeValue();
		}
		if (userchild.nodeName()=="profile_image_url") {
			user->image_url=userchild.firstChild().nodeValue();
		}
		if (userchild.nodeName()=="url") {
			user->url=userchild.firstChild().nodeValue();
		}
		if (userchild.nodeName()=="followers_count") {
			user->followers_count=userchild.firstChild().nodeValue().toLong();
		}
		if (userchild.nodeName()=="friends_count") {
			user->friends_count=userchild.firstChild().nodeValue().toLong();
		}
		if (userchild.nodeName()=="following") {
			if (userchild.firstChild().nodeValue()=="false") {
				if (trustUserFollowingStatus)
					user->is_friend=false;
			} else if (trustUserFollowingStatus && (userchild.firstChild().nodeValue()=="true")) {
				user->is_friend=true;
			}
		}
	}
	if (user->id!="") {
//		QMessageBox::information (0,"assdf",user->name);
		emit haveUser(user);
		if (!newUser)
			emit haveUpdatedUser(user);
		if (user->screen_name==this->user) {
			user->is_friend=true;
			emit haveAuthenticatingUser(user);
		}
	}
	return user;
}

void TweetFetcher::setBirdBox(BirdBox *b) {
	box=b;
}

void TweetFetcher::activateTimeout(int t,int maxJitter) {
	timeoutSecs=(t<minTimeoutSecs)?minTimeoutSecs:t;
	//unsigned long jitter = ((((unsigned long)this)>>8)&0xf)+((((unsigned long)this)>>12)&0xf);
	if (maxJitter>t)
		maxJitter=t;
	unsigned long jitter = maxJitter?yasst_rand()%(maxJitter*1000):0;
	if (!timerActive) {
		timerActive=true;
		nextUpdate=QDateTime::currentDateTime().addSecs(timeoutSecs+(jitter/1000));
		refreshTimer.setInterval((timeoutSecs+(jitter/1000))*1000);
		refreshTimer.start();
		//QTimer::singleShot((timeoutSecs*1000)+jitter,this,SLOT(doTimeout()));
	}
}

void TweetFetcher::doTimeout() {
	int secs = (timeoutSecs<minTimeoutSecs)?minTimeoutSecs:timeoutSecs;
	if (timerActive) {
		nextUpdate=QDateTime::currentDateTime().addSecs(secs);
		refreshTimer.setInterval(secs*1000);
//		QTimer::singleShot(timeoutSecs*1000,this,SLOT(doTimeout()));
	}
	if (deleted) {
		emit fetcherExpired(this);
	} else
		populate();
}

void TweetFetcher::handleDone(bool error) {
	HTTPHandler::handleDone(error);
	if (error)
		emit endFetching();
}

QString TweetFetcher::getLastId() {
	return lastId;
}

void TweetFetcher::setLastId(QString id) {
	lastId=id;
}

void TweetFetcher::setInitialFetchCount(int c) {
	firstFetchCount=c;
}

void TweetFetcher::setUser(QString u) {
	user=u;
}

void TweetFetcher::setPass(QString p) {
	pass=p;
}

QString TweetFetcher::nodeName(QDomNode &node) {
	QString name = node.nodeName();
	if (name.contains(":"))
		return name.mid(name.indexOf(":")+1);
	return name;
}

BirdBox* TweetFetcher::getBirdBox() {
	return box;
}

void TweetFetcher::setOAuth(OAuth *o) {
	oauth=o;
}

int TweetFetcher::doHttpBit(QUrl URL,QString, QString, QString, QString, QString) {
	return HTTPHandler::doHttpBit(URL);
}

int TweetFetcher::doHttpBit(QString URL,QString user,QString pass, QString data, QString type, QString method) {
	QUrl u(URL);
	u.setScheme("http");

	if (method=="") {
		if (data.size()==0)
			method="GET";
		else
			method="POST";
	}

	if (QNetworkProxy::applicationProxy().type()!=QNetworkProxy::NoProxy && u.scheme().toLower()=="https") {
		u.setScheme("http");
//		qDebug()<<"Proxy in use, setting no https for URL: "<<URL;
	}


	if (pendingRequests.isEmpty()) {
		if (user=="")
			return HTTPHandler::doHttpBit(u,"","",data,type,method);

		QString oauth;
		if (box)
			oauth = box->getOAuth()->getAuthorizationHeader(u.toString(),method,data,"http://api.twitter.com/");

		if (oauth!="") {
			headers.insert("Authorization",oauth);
			user="";
			pass="";
			//QMessageBox::information(0,URL,oauth);
			//QApplication::exit();
			return HTTPHandler::doHttpBit(u,"","",data,type,method);
		}
	}
	int id = HTTPHandler::requestCount++;
	TweetFetcher::delayedRequest *req = new TweetFetcher::delayedRequest();

	req->url=u;
	req->data=data;
	req->mimetype=type;
	req->id=id;
	req->user=user;
	req->method=method;

	pendingRequests.append(req);
	drainTimer.start(2000);
	return id;
}

void TweetFetcher::handleDrainTimerFired() {
	if (pendingRequests.isEmpty()) {
		drainTimer.stop();
		return;
	}

	QString oauth;
	UserTweetFetcher::delayedRequest *req = pendingRequests.takeFirst();

	if (box)
		oauth = box->getOAuth()->getAuthorizationHeader(req->url.toString(),req->method,req->data);

	if (oauth!="") {
		drainTimer.setInterval(1000);
		if (req->user!="")
			headers.insert("Authorization",oauth);
		HTTPHandler::doHttpBit(req->url,"","",req->data.toUtf8(),req->mimetype,req->method,req->id);
		delete req;
	} else
		pendingRequests.insert(0,req);
}

QDateTime TweetFetcher::parseTwitterDate(QString twitterdate) {
	//Thu Feb 19 00:20:20 +0000 2009
	QStringList parts = twitterdate.split(" ");
	QDateTime ret;

	ret.setTimeSpec(Qt::UTC);
	if (parts.size()==6) {
		QStringList months;
		QDate date;
		months << "jan" << "feb" << "mar" << "apr" << "may" << "jun" << "jul" << "aug" << "sep" << "oct" << "nov" << "dec";
		QString month = parts.at(1).toLower();
		date.setDate(parts.at(5).toInt(),months.indexOf(month)+1,parts.at(2).toInt());
		ret.setDate(date);
		ret.setTime(QTime::fromString(parts.at(3),"HH:mm:ss"));
	}
	return ret;
}

void TweetFetcher::addUserToList(QString listId,QString userId) {
//	QMessageBox::information(0,userId,"http://api.twitter.com/1/"+user+"/"+listId+"/members.xml",user,pass,"user_id="+userId );
	doHttpBit("http://api.twitter.com/1/"+user+"/"+listId+"/members.xml",user,pass,"id="+userId);
}

void TweetFetcher::removeUserFromList(QString listId,QString userId) {
//	QMessageBox::information(0,userId,"http://api.twitter.com/1/"+user+"/"+listId+"/members.xml",user,pass,"user_id="+userId );
	doHttpBit("http://api.twitter.com/1/"+user+"/"+listId+"/members.xml?id="+userId,user,pass,"","","DELETE");
//	doHttpBit("http://api.twitter.com/1/"+user+"/"+listId+"/members.xml",user,pass,"id="+userId+"&_method=DELETE");
}

void TweetFetcher::removeListBySlug(QString listSlug) {
//	doHttpBit("http://api.twitter.com/1/"+user+"/lists/"+listSlug+".xml",user,pass,"_method=DELETE");
	doHttpBit("http://api.twitter.com/1/"+user+"/lists/"+listSlug+".xml",user,pass,"","","DELETE");
}

void TweetFetcher::updateListDetails(QString listId,QString name,QString ,bool privateList) {
	doHttpBit("http://api.twitter.com/1/"+user+"/lists/"+listId+".xml",user,pass,"name="+QUrl::toPercentEncoding(name)+"&mode="+(privateList?"private":"public"));

}
