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
/* $Id: Translator.cpp,v 1.1 2010/07/11 10:05:45 ian Exp $

 File       : Translator.cpp
 Create date: 22:46:40 4 Jul 2010
 Project    : yasst

 (c) 2010 Ian Clark

 */

#include "Translator.h"
#include "json/JSONParser.h"
#include <QDebug>

QMap<QString,QString> Translator::lang;

Translator::Translator() : HTTPHandler() {
}

Translator::~Translator() {
}

int Translator::translate(QString from,QString to,QString text) {
//	qDebug() << "Translating: " << text;
//	qDebug() << "From: "<< from << " To: " << to;
	headers.insert("Referer","http://yasst.org/");
	QUrl u("http://ajax.googleapis.com/ajax/services/language/translate");
	u.addQueryItem("v","1.0");
	u.addQueryItem("q",text);
	u.addQueryItem("langpair",from+"|"+to);
//	qDebug() << u;
	return doHttpBit(u);
}

void Translator::handleHttpResponse(QString resp,QUrl,bool,int id) {
//	qDebug() << "translate resp: " << resp;
	JSONComplexObject *bob = JSONParser::parse(resp);
	if (!bob) {
		emit otherError(this);
		return;
	}
//	QMessageBox::information (0,"assdf",bob->toString());
	JSONComplexObject *results = (JSONComplexObject*)bob->values.value("responseData");
	if (results) {
		JSONString *text = (JSONString*)results->values.value("translatedText");
		if (text) {
			emit haveTranslation(id,"","",text->get());
		}
		delete results;
	}
}

QMap<QString,QString> Translator::getLanguages() {
	if (lang.size()==0) {

		lang.insert("af","Afrikaans");
		lang.insert("sq","Albanian");
		lang.insert("ar","Arabic");
		lang.insert("hy","Armenian ALPHA");
		lang.insert("az","Azerbaijani ALPHA");
		lang.insert("eu","Basque ALPHA");
		lang.insert("be","Belarusian");
		lang.insert("bg","Bulgarian");
		lang.insert("ca","Catalan");
		lang.insert("zh-CN","Chinese (Simplified)");
		lang.insert("zh-TW","Chinese (Traditional)");
		lang.insert("hr","Croatian");
		lang.insert("cs","Czech");
		lang.insert("da","Danish");
		lang.insert("nl","Dutch");
		lang.insert("en","English");
		lang.insert("et","Estonian");
		lang.insert("tl","Filipino");
		lang.insert("fi","Finnish");
		lang.insert("fr","French");
		lang.insert("gl","Galician");
		lang.insert("ka","Georgian ALPHA");
		lang.insert("de","German");
		lang.insert("el","Greek");
		lang.insert("ht","Haitian Creole ALPHA");
		lang.insert("iw","Hebrew");
		lang.insert("hi","Hindi");
		lang.insert("hu","Hungarian");
		lang.insert("is","Icelandic");
		lang.insert("id","Indonesian");
		lang.insert("ga","Irish");
		lang.insert("it","Italian");
		lang.insert("ja","Japanese");
		lang.insert("ko","Korean");
		lang.insert("lv","Latvian");
		lang.insert("lt","Lithuanian");
		lang.insert("mk","Macedonian");
		lang.insert("ms","Malay");
		lang.insert("mt","Maltese");
		lang.insert("no","Norwegian");
		lang.insert("fa","Persian");
		lang.insert("pl","Polish");
		lang.insert("pt","Portuguese");
		lang.insert("ro","Romanian");
		lang.insert("ru","Russian");
		lang.insert("sr","Serbian");
		lang.insert("sk","Slovak");
		lang.insert("sl","Slovenian");
		lang.insert("es","Spanish");
		lang.insert("sw","Swahili");
		lang.insert("sv","Swedish");
		lang.insert("th","Thai");
		lang.insert("tr","Turkish");
		lang.insert("uk","Ukrainian");
		lang.insert("ur","Urdu ALPHA");
		lang.insert("vi","Vietnamese");
		lang.insert("cy","Welsh");
		lang.insert("yi","Yiddish");
	}

	return lang;
}
