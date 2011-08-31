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
 * TabPanel.h
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#ifndef TABPANEL_H_
#define TABPANEL_H_
#include <QScrollArea>
#include <QSettings>

class TabPanel : public QScrollArea
{
    Q_OBJECT
public:
	TabPanel(QWidget *parent = 0);
	virtual ~TabPanel();
	virtual void readSettings(QSettings &settings) {id=settings.value("id",id).toString();};
	virtual void writeSettings(QSettings &settings) {settings.setValue("type",type);settings.setValue("id",id);}
	int getType(){return type;};
	QString getId(){return id;};
	virtual void setActive(bool){};
public slots:
	virtual void refresh(){};
protected:
	QString id;
	int type;
};

#endif /* TABPANEL_H_ */
