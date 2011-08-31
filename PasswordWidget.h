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
#ifndef PASSWORDWIDGET_H
#define PASSWORDWIDGET_H

#include <QtGui/QDialog>
#include "ui_PasswordWidget.h"
#include <QString>

class PasswordWidget : public QDialog
{
    Q_OBJECT

public:
    PasswordWidget(QString user,QWidget *parent = 0);
    ~PasswordWidget();

    QString getPassword();
    bool getRememberMe();
protected:
/*
    virtual void showEvent(QShowEvent * );
    virtual void closeEvent(QCloseEvent * );
*/
private:
    Ui::PasswordWidgetClass ui;
};

#endif // PASSWORDWIDGET_H
