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
#include "PasswordWidget.h"
#include <QGraphicsBlurEffect>

PasswordWidget::PasswordWidget(QString user,QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	ui.instructions->setText(tr("Enter the password for ")+user);
}

PasswordWidget::~PasswordWidget()
{

}

QString PasswordWidget::getPassword() {
	return ui.password->text();
}

bool PasswordWidget::getRememberMe() {
	return ui.remember->isChecked();
}

/*
void PasswordWidget::showEvent(QShowEvent *e ) {
	QWidget *w = parentWidget();
	if (w) {
		w->setGraphicsEffect(new QGraphicsBlurEffect());
	}
	QDialog::showEvent(e);
}

void PasswordWidget::closeEvent(QCloseEvent *e ) {
	QWidget *w = parentWidget();
	if (w) {
		w->setGraphicsEffect(0);
	}
	QDialog::closeEvent(e);
}
*/
