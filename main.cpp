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
/* $Id: main.cpp,v 1.10 2010/09/14 13:49:05 ian Exp $
File:   main.cpp
(c) 2009 Ian Clark

*/
#include "ya_st.h"

#include <QtGui>
#include <QApplication>

#define YASST_VERSION "20090420"
long yasst_rand();

#include "backend/BirdBox.h"
#include "backend/ImageCache.h"
#include "yasstFns.h"
#include "YasstApplication.h"

int main(int argc, char *argv[])
{
	srand(QDateTime::currentDateTime().toTime_t()); // Yes, I know. But actual randomness isn't important, it's used as jitter.

    QCoreApplication::setApplicationName("Yasst");
    QCoreApplication::setOrganizationName("yasst.org");
    QCoreApplication::setOrganizationDomain("yasst.org");
    //BirdBox b;
    ImageCache c;

    YasstApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    Ya_st w(&c);

    QStringList args = QCoreApplication::arguments();

    QString tweet="";
    QString tweetAs="";

    for (int x=0,state=0;x<args.count();x++) {
		QString arg=args.at(x).toLower();
    	if (state==0) {
    		if (arg=="-tweet") {
    			state=1;
    		} else if (arg=="-tweetas") {
    			state=2;
    		}
    	} else if (state==1) {
    		tweet=arg;
    		state=0;
    	} else if (state==2) {
    		tweetAs=arg;
    		state=0;
    	}
    }

    if (tweet!="" && tweetAs!="") {
    	w.tweetAndQuit(tweet,tweetAs);
    } else
    	w.show();
    return a.exec();
}

