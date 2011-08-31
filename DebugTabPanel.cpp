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
 * DebugTabPanel.cpp
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#include "DebugTabPanel.h"
#include "backend/fetchers/SearchFetcher.h"
#include "backend/fetchers/UserGroupFetcher.h"
#include "backend/fetchers/UserGroupFollowingFetcher.h"
#include "backend/fetchers/TwitterUserTweetsFetcher.h"
#include "backend/fetchers/UserGroupTweetFetcher.h"
#include "backend/fetchers/FriendFetcher.h"
#include "backend/Group.h"
#include <QString>
#include <QMutableHashIterator>
#include <QWidget>
#include "YasstApplication.h"

DebugTabPanel::DebugTabPanel(BirdBox *t,ImageCache *c,TweetTabWidget *w,QWidget *parent) : TabPanel(parent) {
	type=9999;
	tweets=t;
	tabs=w;
	cache=c;
	layout=new QVBoxLayout();
	widget()->setLayout(layout);
	label=new QLabel(this);
	label->setAlignment(Qt::AlignTop);
	layout->addWidget(label);
	timer=new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateDetails()));
	timer->start(1000);
	updateDetails();
}

DebugTabPanel::~DebugTabPanel() {
	timer->stop();
	timer->deleteLater();
}

void DebugTabPanel::updateDetails() {
	QString text= "<h1>Debug info</h1>";

	int x;

	text+="<p><table border='0' cellpadding='2'><tr><th align='left'>Yasst started:</th><td>"+((YasstApplication*)qApp)->startTime.toString()+"</td></tr>";
	text+="<tr><th align='left'>Time now:</th><td>"+QDateTime::currentDateTime().toString()+"</td></tr></table></p>";

	QList<BirdBox*> accts = ((YasstApplication*)qApp)->getAccounts();

	text+="<h2>Accounts</h2><p>There are "+QString::number(accts.length())+" accounts known.</p>";
	for(int y=0;y<accts.length();y++) {
		tweets=accts.at(y);

		text+="<h3>Account: "+tweets->getAuthenticatingUserScreenName()+"</h3>";
		text+="<h4>Tweets</h4><p>I know about "+QString::number(tweets->tweets.count())+" tweets and "+QString::number(tweets->directMessages.count())+" direct messages.<br>";
		text+=QString::number(getPrunableTweetCount())+" tweets are prunable, next prune at "+tweets->nextPrune.toString();
		text+="<br>I have removed "+QString::number(tweets->pruneCount)+" expired tweets so far.</p>";
		text+="<p>I know about "+QString::number(tweets->users.count())+" twitter users.</p>";

		text+="<h4>API</h4><p>There are "+QString::number(tweets->apiCount)+" API calls left in this hour, API count resets at: "+tweets->apiRefresh.toString()+"</p>";
		text+="<table cellpadding='2'><tr><th>#</th><th>Type</th><th>API Cost</th><th>Weight</th><th>Min update<br/>seconds</th><th>In progress count</th><th>Refresh interval (Secs)</th><th>Next update at</th><th>Authenticating</th></tr>";
		for(x=0;x<tweets->fetchers.count();x++) {
			TweetFetcher *f=tweets->fetchers.at(x);
			text+="<tr><td>Fetcher "+QString::number(x+1)+((f->deleted)?"[D]":"")+"</td>";
			text+="<td>"+fetcherType(f)+"</td>";
			text+="<td>"+QString::number(f->apiCount)+"</td>";
			text+="<td>"+QString::number(f->weight)+"</td>";
			text+="<td>"+QString::number(f->minTimeoutSecs)+"</td>";
			text+="<td>"+QString::number(f->inProgressCount)+"</td>";
			text+="<td>"+QString::number(f->timeoutSecs)+"</td>";
			text+="<td>"+f->nextUpdate.toString()+"</td>";
			text+="<td>"+((f->getUser()=="")?"No":"Yes ["+f->getUser()+"]")+"</td></tr>";
		}
		for(x=0;x<tweets->freeFetchers.count();x++) {
			TweetFetcher *f=tweets->freeFetchers.at(x);
			text+="<tr><td>Fetcher "+QString::number(x+tweets->fetchers.count()+1)+((f->deleted)?"[D]":"")+"</td>";
			text+="<td>"+fetcherType(f)+"</td>";
			text+="<td>"+QString::number(f->apiCount)+"</td>";
			text+="<td>"+QString::number(f->weight)+"</td>";
			text+="<td>"+QString::number(f->minTimeoutSecs)+"</td>";
			text+="<td>"+QString::number(f->inProgressCount)+"</td>";
			text+="<td>"+QString::number(f->timeoutSecs)+"</td>";
			text+="<td>"+f->nextUpdate.toString()+"</td>";
			text+="<td>"+((f->getUser()=="")?"No":"Yes ["+f->getUser()+"]")+"</td></tr>";
		}
		text+="</table>";
		text+="<p>First fetch count: "+QString::number(tweets->firstFetchCount)+"</p>";
	}

	text+="<h2>Images</h2><p>I have "+QString::number(cache->count)+" images cached, totaling aprox. "+QString::number((cache->totalBytes)/1024)+" KB ("+QString::number((cache->totalBytes)/1024/1024)+" MB).";
	text+="<br>There are "+QString::number(cache->pendingUrls.count())+" pending URLs";
	text+="<br>I have evicted "+QString::number(cache->pruneCount)+" old images so far.</p>";

	QString Url = cache->oldestUrl;
	if (Url.size()>30)
		Url="..."+Url.right(30);
	text+="<p>The oldest image is '"+Url+"', created: "+cache->oldestDate.toString()+"<br>";
	Url = cache->newestUrl;
	if (Url.size()>30)
		Url="..."+Url.right(30);
	text+="The newest image is '"+Url+"', created: "+cache->newestDate.toString()+"<br>";
	Url = cache->mruUrl;
	if (Url.size()>30)
		Url="..."+Url.right(30);
	text+="The most recent image used is '"+Url+"', accessed: "+cache->mruDate.toString()+"<br>";
	Url = cache->lruUrl;
	if (Url.size()>30)
		Url="..."+Url.right(30);
	text+="The oldest used image is '"+Url+"', accessed: "+cache->lruDate.toString()+"</p>";
	text+=getImageInfo();

/*
	for(x=0;x<tweets->fetchers.count();x++) {
		TweetFetcher *f=tweets->fetchers.at(x);
		text+="<li>Fetcher "+QString::number(x+1)+(f->inProgressCount?" is doing something,":"")+" next updates at "+f->nextUpdate.toString()+"</li>";
	}
	for(x=0;x<tweets->freeFetchers.count();x++) {
		TweetFetcher *f=tweets->freeFetchers.at(x);
		text+="<li>Fetcher "+QString::number(x+fetchersCount+1)+(f->inProgressCount?" is doing something,":"")+" has no API cost and will update periodically. Next updated at "+f->nextUpdate.toString()+"</li>";
	}
	text+="</ul>";
*/
	label->setText(text);
}

QString DebugTabPanel::fetcherType(TweetFetcher *f) {
	Group *g=0;
	switch(f->type) {
	case 0 : return "Public tweets";break;
	case 1 : return "Friends tweets";break;
	case 2 : return "Replys";break;
	case 3 : return "DMs";break;
	case 4 : return "Favourites";break;
	case 5 :
		g=((UserGroupFetcher*)f)->group();
		if (!g)
			return "Lists fetcher";
		return "List fetcher ["+g->name+"]";
		break;
	case 6 : return "Single tweet (web)";break;
	case 7 : return "Search ["+((SearchFetcher*)f)->getSearch()+"]";break;
	case 8 : return "Friends [State "+QString::number(((FriendFetcher*)f)->getState())+"]";break;
	case 9 : return "Feeds";break;
	case 10: return "Trending topics";break;
	case 11: return "Twitrpix handler";break;
	case 12: return "Single users tweets ["+((TwitterUserTweetsFetcher*)f)->fetchScreenName()+"]";break;
	case 13: return "List tweets ["+((UserGroupTweetFetcher*)f)->getGroup()->name+" - "+((UserGroupTweetFetcher*)f)->getGroup()->id+"]";break;
	case 14: return "Lists following";break;
	case 256: return "Program update check";break;
	}
	return "Unknown: "+QString::number(f->type);
}
int DebugTabPanel::getPrunableTweetCount() {
	QMutableHashIterator <QString,Tweet*> i(tweets->tweets);
	int count=0;

	QDateTime cutoff = QDateTime::currentDateTime().addSecs(-(60*20));
	while (i.hasNext()) {
		i.next();
		Tweet *t=i.value();
		if (t->refCount==0 && t->timestamp<cutoff) {
			count++;
		}
	}
	return count;
}

QString DebugTabPanel::getImageInfo() {
	QMutableHashIterator <QString,ImageCache::cacheEntry*> *i=cache->getIterator();
	QString out="";
	int count=0;
	while (i->hasNext()) {
		i->next();
		ImageCache::cacheEntry *e=i->value();
		if (e->updateCount>1)
			count++;
	}
	cache->releaseIterator();
	if (count)
		out+="<p style=\"color: red;\">"+QString::number(count)+tr(" images have >1 update count.</p>");

	return out;
}
