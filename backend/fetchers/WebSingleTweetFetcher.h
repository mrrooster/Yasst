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
 * WebSingleTweetFetcher.h
 *
 *  Created on: 03-Mar-2009
 *      Author: ian
 */

#ifndef WEBSINGLETWEETFETCHER_H_
#define WEBSINGLETWEETFETCHER_H_
#include "TweetFetcher.h"

class WebSingleTweetFetcher : public TweetFetcher {
	Q_OBJECT
public:
	WebSingleTweetFetcher();
	virtual ~WebSingleTweetFetcher();

	void fetchSingleTweet(QString,QString,QString id);
	void populate();
	void setPreText(QString t) {preText=t;};
	void setPostText(QString t) {postText=t;};

protected:
	virtual void handleHttpResponse(QString,QUrl,bool,int);

private:
	QString userScreenName;
	QString userId;
	QString tweetId;
	QString preText;
	QString postText;
private slots:
	void handleError(HTTPHandler*);
};

#endif /* WEBSINGLETWEETFETCHER_H_ */
