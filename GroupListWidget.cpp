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
/* $Id: GroupListWidget.cpp,v 1.23 2010/07/25 14:51:22 ian Exp $
File:   GroupListWidget.h
(c) 2009 Ian Clark

*/

#include <QMessageBox>
#include "GroupListWidget.h"
#include "GroupListWidgetItem.h"
#include "backend/fetchers/UserGroupFetcher.h"

GroupListWidget::GroupListWidget(QWidget *parent) : TweetListWidget(parent) {
	type=1;
	id="";
}

GroupListWidget::GroupListWidget(BirdBox *b,ImageCache *i, QWidget *parent) : TweetListWidget(b,i,parent),myGroup(0),id("") {
	setListVisible();
	QObject::connect(ui.save,SIGNAL(clicked()),this,SLOT(saveGroup()));
	QObject::connect(ui.cancel,SIGNAL(clicked()),this,SLOT(handleCancelPressed()));
	QObject::connect(ui.title,SIGNAL(doubleclicked()),this,SLOT(setListVisible()));
	QObject::connect(ui.closeButton,SIGNAL(clicked()),this,SLOT(close()));
	QObject::connect(ui.groupmembers,SIGNAL(itemClicked   (QListWidgetItem *)),this,SLOT(handleItemActivated  (QListWidgetItem *)));
	QObject::connect(b,SIGNAL(haveUser(TweetUser *)),this,SLOT(handleHaveUser(TweetUser *)));
	QObject::connect(i,SIGNAL(update(QString)),this,SLOT(handleImageUpdate(QString)));
	QObject::connect(b,SIGNAL(groupUpdated(Group*)),this,SLOT(handleGroupUpdated(Group*)));
	QString tt = ui.title->toolTip();
	if (tt!="")
		tt+="\n";
	tt+=tr("Double click to change the group members.");
	ui.title->setToolTip(tt);
//	b->registerGroup(this);
	type=1;
	ui.type->setPixmap(QPixmap(":/buttons/group.png"));
	hadSave=false;
}

GroupListWidget::~GroupListWidget() {
//	tweets->unregisterGroup(this);
	QObject::disconnect(ui.save,SIGNAL(clicked()),this,SLOT(saveGroup()));
	QObject::disconnect(ui.cancel,SIGNAL(clicked()),this,SLOT(setTweetsVisible()));
	QObject::disconnect(ui.title,SIGNAL(doubleclicked()),this,SLOT(setListVisible()));
	QObject::disconnect(ui.closeButton,SIGNAL(clicked()),this,SLOT(close()));

	if (myGroup) {
		myGroup->goodbye();
		if (!myGroup->twitterList && !myGroup->getRefCount()) {
			tweets->removeGroup(myGroup);
			myGroup->deleteLater();
		}
	}
}

void GroupListWidget::saveGroup() {
	QList<QListWidgetItem*> selected;
	bool twitterList = ui.twitterlist->isChecked();
	QString name = ui.groupname->text();

	groupmembers.clear();

	ui.title->setText(ui.groupname->text());

	selected=ui.groupmembers->selectedItems();
	for(int x=0;x<selected.count();x++) {
		QString name=selected[x]->text();
		//u=tweets->getUserByScreenName(name);
		groupmembers.append(name);
	}
	if (!myGroup && id=="") {
		myGroup=new Group(tweets);
		myGroup->hello();
		myGroup->twitterList=false;
		myGroup->setGroupMembers(groupmembers);
		myGroup->updateName(ui.groupname->text());
		tweets->handleGroup(myGroup);
		id="";
	}
	if (myGroup) {
		myGroup->updateGroupMembers(groupmembers);// NB: Causes update signal, resets checkbox
		myGroup->updateName(name);
		if (twitterList&&(!(myGroup->twitterList)||(myGroup->ownerScreenName!=myGroup->user))) {
			if (tweets->hasCredentials()) {
				myGroup->ownerScreenName=myGroup->user;
				UserGroupFetcher *f = new UserGroupFetcher();
				tweets->addFetcher(f);
				f->promoteGroupToList(myGroup);
			}
		} else if (!twitterList && myGroup->twitterList) {
			myGroup->convertToLocal();
			id="";
		}
	} else if (!twitterList)
		id="";
	repopulate();
	setTweetsVisible();
	emit tweetsPending();
	hadSave=true;
}

bool GroupListWidget::canDoUpdate(Tweet *t) {
	return groupmembers.contains(t->screen_name)&&TweetListWidget::canDoUpdate(t);
}


void GroupListWidget::populateList() {
	QList<TweetUser *> users = tweets->getUsers();
	QList<QString> groupmembers(this->groupmembers);
	ui.groupmembers->clear();

	for(int x=0;x<users.count();x++) {
		if (users[x]->is_friend) {
			QListWidgetItem *i=new GroupListWidgetItem(users[x]->screen_name);
			i->setIcon(QIcon(images->getPixMap(users[x]->image_url)));
			ui.groupmembers->addItem(i);
			if(groupmembers.contains(users[x]->screen_name)) {
				i->setSelected(true);
				groupmembers.removeOne(users[x]->screen_name);
			}
		}
	}
	while (groupmembers.count()) {
		QString name = groupmembers.takeFirst();
		QListWidgetItem *i=new GroupListWidgetItem(name);
		i->setIcon(QIcon(images->getPixMap("")));
		ui.groupmembers->addItem(i);
		i->setSelected(true);
	}
	if (myGroup)
		ui.twitterlist->setChecked(myGroup->twitterList);
	else
		ui.twitterlist->setChecked(id!="");

}

void GroupListWidget::setListVisible() {
	populateList();
	setStack(1);
}

void GroupListWidget::writeSettings(QSettings &settings) {
	TweetListWidget::writeSettings(settings);
	QList<QString> members;
//	QList<QString,TweetUser*>::iterator i = groupmembers.begin();
	QList<Tweet*> ret;
	settings.setValue("name",ui.title->text());
	if (myGroup) {
		settings.setValue("list_id",myGroup->id);
		settings.setValue("local_id",myGroup->local_id);
	} else
		settings.setValue("list_id",id);
	settings.beginWriteArray("members");
	for(int x=0;x<groupmembers.count();x++) {
		settings.setArrayIndex(x);
		settings.setValue("user_id",groupmembers.at(x));
	}
	settings.endArray();
}

void GroupListWidget::readSettings(QSettings &settings) {
	TweetListWidget::readSettings(settings);
	ui.title->setText(settings.value("name").toString());
	ui.groupname->setText(settings.value("name").toString());
	id=settings.value("list_id","").toString();
//	QMessageBox::information(0,"ff",ui.groupname->text());
	int count = settings.beginReadArray("members");
	groupmembers.clear();
	for(int x=0;x<count;x++) {
    	settings.setArrayIndex(x);
		QString val = settings.value("user_id").toString();
		groupmembers.append(val);
	}
	settings.endArray();

	if (!myGroup && id=="") {
		myGroup=new Group(tweets);
		myGroup->twitterList=false;
		myGroup->name=ui.groupname->text();
		myGroup->updateGroupMembers(groupmembers);
		tweets->handleGroup(myGroup);
	}
	repopulate();
	setTweetsVisible();
	hadSave=true;
}

void GroupListWidget::close() {
	if ((myGroup&&myGroup->twitterList)||(!myGroup&&id!=""))
		return;
	emit groupListClosed(ui.title->text(),groupmembers);
}

void GroupListWidget::setMembers(QList<QString> members) {
	groupmembers.append(members);
	if (myGroup)
		myGroup->updateGroupMembers(groupmembers);
	populateList();
}

void GroupListWidget::setName(QString name) {
	ui.groupname->setText(name);
	ui.title->setText(name);
}

void GroupListWidget::handleHaveUser(TweetUser *u) {
	if (ui.stack->currentIndex()!=1)
		return;
	if (u->is_friend) {
		int count = ui.groupmembers->count();
		for (int x=0;x<count;x++) {
			if (ui.groupmembers->item(x)->text()==u->screen_name)
				return;
		}
		QListWidgetItem *i=new GroupListWidgetItem(u->screen_name);
		i->setIcon(QIcon(images->getPixMap(u->image_url)));
		ui.groupmembers->addItem(i);
		if(groupmembers.contains(u->screen_name)) {
			i->setSelected(true);
		}
	}
}

void GroupListWidget::handleImageUpdate(QString url) {
	if (ui.stack->currentIndex()!=1)
		return;
	int count = ui.groupmembers->count();
	for (int x=0;x<count;x++) {
		QListWidgetItem *i=ui.groupmembers->item(x);
		TweetUser *user = tweets->getUserByScreenName(i->text());
		if (user && user->image_url==url) {
			i->setIcon(QIcon(images->getPixMap(url)));
		}
	}
	//populateList();
}

QString GroupListWidget::getName() {
	return ui.title->text();
}

void GroupListWidget::addUserByScreenName(QString name) {
	groupmembers.append(name);
	if (myGroup)
		myGroup->updateName(name);
	else {
		populateList();
		repopulate();
	}
}

void GroupListWidget::removeUserByScreenName(QString name) {
	if (groupmembers.removeOne(name)) {
		if (myGroup)
			myGroup->removeGroupMember(name);
		else {
			populateList();
			repopulate();
		}
	}
}

void GroupListWidget::handleGroupUpdated(Group *g) {
	if (g->id!="" && g->id==id) {
		myGroup=g;
		myGroup->hello();
		id="";
	}
	if (myGroup && myGroup==g) {
		groupmembers.clear();
		groupmembers=g->groupmembers;
		ui.title->setText(g->name);
		ui.groupname->setText(g->name);
		populateList();
		repopulate();
	}
}

void GroupListWidget::setGroup(Group *g) {
	if (myGroup==g)
		return;

	if (myGroup)
		myGroup->goodbye();

	myGroup=g;
	myGroup->hello();
	groupmembers.clear();
	groupmembers=g->groupmembers;
	ui.title->setText(g->name);
	ui.groupname->setText(g->name);
	populateList();
	repopulate();
}

void GroupListWidget::repopulate() {
	if ((myGroup && myGroup->twitterList)||id!="")
		if (myGroup && myGroup->ownerScreenName!=myGroup->user)
			ui.type->setPixmap(QPixmap(":/buttons/twitter-dark.png"));
		else
			ui.type->setPixmap(QPixmap(":/buttons/twitter.png"));
	else
		ui.type->setPixmap(QPixmap(":/buttons/group.png"));
	TweetListWidget::repopulate();
}

void GroupListWidget::handleCancelPressed() {
	if (!hadSave&&!myGroup)
		slowDelete();
	else
		TweetListWidget::setTweetsVisible();
}

void GroupListWidget::handleItemActivated  ( QListWidgetItem *  ){
	if (myGroup && ui.stack->currentIndex()==1 && myGroup->twitterList && myGroup->ownerScreenName!=myGroup->user) {
		QMessageBox box;
		box.setText(tr("This is a list your follow, to add or remove members you must create a copy or convert to a local group.\n\nDoing this will not unfollow this list."));
		box.addButton(tr("Create twitter list"),QMessageBox::AcceptRole);
		box.addButton(tr("Convert to local group"),QMessageBox::ActionRole);
		box.addButton(QMessageBox::Cancel);
		box.exec();
		QAbstractButton *b = box.clickedButton();
		if (!b || box.buttonRole(b)==QMessageBox::RejectRole) {
			populateList();
		} else if (box.buttonRole(b)==QMessageBox::AcceptRole) {
//			myGroup->convertToLocal(); // saving the group will create the list
		} else if (box.buttonRole(b)==QMessageBox::ActionRole) {
			ui.twitterlist->setChecked(false);
		}
	}
}

void GroupListWidget::handleRefreshButton() {
	if (myGroup)
		myGroup->refresh();
	else {
		TweetFetcher *f = tweets->getFetcher(1);
		if (f)
			f->populate();
	}
}

