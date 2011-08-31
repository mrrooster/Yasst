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
/* $Id: ProgramUpdateCheckFetcher.cpp,v 1.9 2010/03/30 17:40:06 ian Exp $

 File       : ProgramUpdateCheckFetcher.cpp
 Create date: 20:44:04 23-Aug-2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#include "ProgramUpdateCheckFetcher.h"
#include <QSettings>
#include <QMessageBox>
#include <QDesktopServices>

ProgramUpdateCheckFetcher::ProgramUpdateCheckFetcher() : TweetFetcher() {
	minTimeoutSecs=86400; // a day
//	minTimeoutSecs=240; // a day
	periodic=true;
	type=256;
}

ProgramUpdateCheckFetcher::~ProgramUpdateCheckFetcher() {
}

void ProgramUpdateCheckFetcher::setVersion(QString v) {
	version=v;
//	version="18990101";
}

QString ProgramUpdateCheckFetcher::getVersion() {
	return version;
}

void ProgramUpdateCheckFetcher::populate() {
	QSettings opts;

	if (opts.value("checkforupdates",QVariant(true)).toBool()) {
		doHttpBit(tr("http://twitter.com/statuses/user_timeline/24565379.rss"),"","","");
	}
}

void ProgramUpdateCheckFetcher::handleXml(QDomDocument *doc,int,int ) {
	QDomNode node = doc->firstChild().nextSibling();
	if (node.nodeName()=="rss") {
		QDomNodeList  nodes = doc->elementsByTagName("item");
		for(unsigned int x=0;x<nodes.length();x++) {
			QDomNode node = nodes.item(x).firstChildElement("description");
			if (node.hasChildNodes()) {
				QString text = node.firstChild().nodeValue();
				QString os = "win32";
				// New windows version available, build 19000101 http://yasst.org/
#ifdef Q_WS_WIN
				QString startingText = "yasst: new windows version available, build ";
#else
#ifdef Q_WS_MAC
				QString startingText = "yasst: new osx version available, build ";
				os = "osx";
#else
				QString startingText = "yasst: new linux version available, build ";
				os = "x11";
#endif
#endif
				if (text.toLower().startsWith(startingText)) {
					QString newVersion = text.mid(startingText.length(),8);
					if (newVersion>version) {
						if (QMessageBox::question(0,tr("Yasst upgrade check"),tr("There is a new version of Yasst available for download. (Version ")+newVersion+tr(") Would you like to go to the download page?"),QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
							QDesktopServices::openUrl(QUrl("http://yasst.org/downloads.php?from="+version+"&p="+os));
						return;
					}
				}
			}
		}
	}
}
