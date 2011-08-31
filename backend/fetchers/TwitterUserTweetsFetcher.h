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
/* $Id: TwitterUserTweetsFetcher.h,v 1.2 2009/12/21 02:50:49 ian Exp $

 File       : TwitterUserTweetsFetcher.h
 Create date: 18:58:10 16-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef TWITTERUSERTWEETSFETCHER_H_
#define TWITTERUSERTWEETSFETCHER_H_

#include "UserTweetFetcher.h"

class TwitterUserTweetsFetcher: public UserTweetFetcher {
	Q_OBJECT
public:
	TwitterUserTweetsFetcher();
	virtual ~TwitterUserTweetsFetcher();
	void populate();
	void setFetchScreenName(QString id);
	QString fetchScreenName() {return id;};

private:
	QString id;

};
#endif /* TWITTERUSERTWEETSFETCHER_H_ */
