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
 * TabPanel.cpp
 *
 *  Created on: 16-Apr-2009
 *      Author: ian
 */

#include "TabPanel.h"
#include <QString>
#include <QDateTime>
#include <QLayout>

TabPanel::TabPanel(QWidget *parent) : QScrollArea(parent) {
	id = QString::number((long long)this)+tr(":")+QString::number(QDateTime::currentDateTime().toTime_t());
	type=-1;
	QWidget *c = new QWidget(this);
//	QScrollArea *area = new QScrollArea();
//	tabs->addTab(area,tr("Tweets"));

	setWidget(c);
	setWidgetResizable(true);
	c->setObjectName(tr("tabpanel"));
}

TabPanel::~TabPanel() {
}
