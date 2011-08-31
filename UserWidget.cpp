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
#include "UserWidget.h"
#include "ya_st.h"

UserWidget::UserWidget(QWidget *parent)
    : AbstractTopWidget(parent)
{
	type=1;
	ui.setupUi(this);
}

UserWidget::UserWidget(Tweet *,TweetUser *u,BirdBox *,ImageCache *,QWidget *parent) : AbstractTopWidget(parent) {
	ui.setupUi(this);

//	userImageCache=new ImageCache();
	type=1;
	user=u;
//	tweet=t;

	ui.screen_name->setText(tr("<b>")+user->screen_name+tr("</b>"));
	ui.description->setText(user->description);
	QString web = "";
	if (user->url!="")
		web = "<style>"+Ya_st::labelCSS+"</style><a href=\""+user->url+"\">Profile URL...</a><br/>";
	ui.website->setText(web+"<a href=\"http://twitter.com/"+user->screen_name+"\">Twitter page...</a>");
	ui.friendscount->setText(QString::number(user->friends_count));
	ui.followerscount->setText(QString::number(user->followers_count));
	ui.location->setText(user->location);
	ui.image->setPixmap(userImageCache.getPixMap(user->image_url));
	QObject::connect(&userImageCache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)));
}

UserWidget::~UserWidget()
{
//	QObject::disconnect(&userImageCache,SIGNAL(update(QString)),this,SLOT(updateImage(QString)));
}

void UserWidget::updateImage(QString url) {
	if (user && user->image_url==url)
		ui.image->setPixmap(userImageCache.getPixMap(user->image_url));
}
