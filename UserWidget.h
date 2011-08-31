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
#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QtGui/QWidget>
#include "ui_UserWidget.h"
#include "AbstractTopWidget.h"
#include "backend/ImageCache.h"
#include "backend/BirdBox.h"
#include "backend/TweetUser.h"
#include "backend/Tweet.h"

class UserWidget : public AbstractTopWidget
{
    Q_OBJECT

public:
    UserWidget(QWidget *parent = 0);
    UserWidget(Tweet *t,TweetUser *u,BirdBox *b,ImageCache *c = 0,QWidget *parent = 0);
    ~UserWidget();

//    Tweet* getTweet() {return tweet;};
private:
	ImageCache userImageCache;
	TweetUser *user;
//	Tweet *tweet;
    Ui::UserWidgetClass ui;
private slots:
	void updateImage(QString);
};

#endif // USERWIDGET_H
