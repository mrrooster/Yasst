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
/* $Id: TrendlingLocationsFetcher.h,v 1.1 2010/09/26 22:27:28 ian Exp $

 File       : TrendlingLocationsFetcher.h
 Create date: 19:59:23 26 Sep 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#ifndef TRENDLINGLOCATIONSFETCHER_H_
#define TRENDLINGLOCATIONSFETCHER_H_

#include "TweetFetcher.h"

class TrendlingLocationsFetcher: public TweetFetcher {
	Q_OBJECT
public:
	TrendlingLocationsFetcher();
	virtual ~TrendlingLocationsFetcher();
};

#endif /* TRENDLINGLOCATIONSFETCHER_H_ */
