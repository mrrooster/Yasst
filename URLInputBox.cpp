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
#include "URLInputBox.h"

URLInputBox::URLInputBox(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(ui.ok,SIGNAL(clicked()),this,SLOT(accept()));
	QObject::connect(ui.cancel,SIGNAL(clicked()),this,SLOT(reject()));

	setWindowTitle(tr("Enter a URL"));
}

URLInputBox::~URLInputBox()
{

}

QString URLInputBox::getURL() {
	return ui.url->text();
}
