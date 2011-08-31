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
/* $Id: PopoutWidget.h,v 1.1 2009/09/26 18:54:09 ian Exp $

 File       : PopoutWidget.h
 Create date: 00:26:53 25 Sep 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef POPOUTWIDGET_H_
#define POPOUTWIDGET_H_
#include <QMainWindow>

class PopoutWidget : public QMainWindow {
	Q_OBJECT
public:
	PopoutWidget(QWidget *p=0);
	virtual ~PopoutWidget();
protected:
	void closeEvent ( QCloseEvent *  )  { emit amClosing();};
signals:
	void amClosing();
};

#endif /* POPOUTWIDGET_H_ */
