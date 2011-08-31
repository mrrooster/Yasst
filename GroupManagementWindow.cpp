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
#include "GroupManagementWindow.h"
#include "backend/GroupHolder.h"
#include "backend/fetchers/UserGroupFetcher.h"

GroupManagementWindow::GroupManagementWindow(ImageCache *i,QWidget *parent)
    : QDialog(parent),myGroup(0)
{
	ui.setupUi(this);
	tweets=0;
	images=i;
	QObject::connect(ui.groups,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(handleItemClicked(QListWidgetItem*)));
	QObject::connect(ui.groupmembers,SIGNAL(itemSelectionChanged()),this,SLOT(handleSelectionChanged()));
	QObject::connect(ui.publiclist,SIGNAL(stateChanged(int)),this,SLOT(handlePubPrivToggle(int)));
	QObject::connect(ui.groupname,SIGNAL(textEdited ( const QString &)),this,SLOT(handleTextChanged ( const QString &)));
	QObject::connect(ui.addusername,SIGNAL(textEdited ( const QString &)),this,SLOT(handleAddUserNameChanged ( const QString &)));
	QObject::connect(ui.addgroup,SIGNAL(clicked()),this,SLOT(handleNewGroup()));
	QObject::connect(ui.delgroup,SIGNAL(clicked()),this,SLOT(handleDeleteGroup()));
	QObject::connect(ui.savechanges,SIGNAL(clicked()),this,SLOT(handleSaveChanges()));
	QObject::connect(ui.done,SIGNAL(clicked()),this,SLOT(handleDone()));
	QObject::connect(ui.adduser,SIGNAL(clicked()),this,SLOT(handleAddUser()));
	QObject::connect(i,SIGNAL(update(QString)),this,SLOT(handleImageUpdate(QString)));
	ui.savechanges->setVisible(false);
}

GroupManagementWindow::~GroupManagementWindow()
{

}

void GroupManagementWindow::showGroupManagement(BirdBox *b) {
	QObject::connect(b,SIGNAL(groupUpdated(Group*)),this,SLOT(handleGroupUpdated(Group*)));
	if (tweets)
		QObject::disconnect(tweets,SIGNAL(groupUpdated(Group*)),this,SLOT(handleGroupUpdated(Group*)));
	tweets=b;
	setup();
	this->show();
    this->raise();
    this->activateWindow();
}

void GroupManagementWindow::setup() {
	QList<Group*> groups = tweets->getGroups();

	ui.groups->clear();

	for(int x=0;x<groups.count();x++) {
		Group *g = groups.at(x);
		if (g->twitterList && g->user==g->ownerScreenName) {
			QListWidgetItem *i=new GroupListWidgetItem(g->name);
			i->setIcon(QIcon(":/buttons/twitter.png"));
			QVariant data;
			GroupHolder holder(g);
			data.setValue(holder);
			i->setData(Qt::UserRole,data);
			ui.groups->addItem(i);
			if (g==myGroup)
				ui.groups->setCurrentItem(i);
		}
	}

	ui.title->setText(tweets->getAuthenticatingUserScreenName()+tr("'s lists"));
}

void GroupManagementWindow::handleGroupUpdated(Group *) {
	/*
	 * BirdBox fires group updated when a group is added, need to defer update
	 * as birdbox currently has lock on groups, so 'getGroups' call in setup deadlocks.
	 */
	if (this->isVisible())
		QTimer::singleShot(0,this,SLOT(setup()));
}

void GroupManagementWindow::handleItemClicked(QListWidgetItem *item) {
	setupGroup(item->data(Qt::UserRole).value<GroupHolder>().group);
}

void GroupManagementWindow::handleImageUpdate(QString url) {
	int count = ui.groupmembers->count();
	for (int x=0;x<count;x++) {
		QListWidgetItem *i=ui.groupmembers->item(x);
		TweetUser *user = tweets->getUserByScreenName(i->text());
		if (user && user->image_url==url) {
			i->setIcon(QIcon(images->getPixMap(url)));
		}
	}
}

void GroupManagementWindow::handleSelectionChanged() {
	ui.savechanges->setVisible(true);
}

void GroupManagementWindow::handleNewGroup() {
	setupGroup(new Group(tweets));
	ui.groups->clearSelection();
	ui.groupname->setText(tr("New list"));
	ui.groupname->setFocus(Qt::OtherFocusReason);
	ui.groupname->selectAll();
}

void GroupManagementWindow::handleDeleteGroup() {
	Group *g=myGroup;
	myGroup=0;
	if (g) {
		if (!g->twitterList)
			delete g;
		else {
			g->convertToLocal();
		}
	}
	ui.groupmembers->clear();
	ui.groupname->setText("");
	ui.savechanges->setVisible(false);
	setup();
}

void GroupManagementWindow::handleSaveChanges() {
	if (!myGroup)
		return;
	QList<QListWidgetItem*> selected = ui.groupmembers->selectedItems();
	QList<QString> newmembers;

	for(int x=0;x<selected.count();x++) {
		QString name=selected[x]->text();
		//u=tweets->getUserByScreenName(name);
		newmembers.append(name);
	}
	myGroup->updateGroupMembers(newmembers);
	myGroup->updateName(ui.groupname->text());
	myGroup->updateIsPrivate(!(ui.publiclist->isChecked()));
	if (!myGroup->twitterList) {
		if (tweets->hasCredentials()) {
			UserGroupFetcher *f = new UserGroupFetcher();
			tweets->handleGroup(myGroup);
			tweets->addFetcher(f);
			f->promoteGroupToList(myGroup);
			setup();
		}
	}
	ui.savechanges->setVisible(false);
}

void GroupManagementWindow::setupGroup(Group *g) {
	QList<TweetUser *> users = tweets->getUsers();
	QList<QString> groupmembers(g->groupmembers);
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
	ui.groupname->setText(g->name);
	ui.publiclist->setChecked(!(g->privateList));
	if (myGroup && !myGroup->twitterList)
		delete myGroup;
	myGroup=g;
	ui.savechanges->setVisible(false);
}

void GroupManagementWindow::handleTextChanged ( const QString & ) {
	if (!myGroup)
		return;
	ui.savechanges->setVisible(true);
}

void GroupManagementWindow::handleAddUserNameChanged ( const QString &text ) {
	ui.adduser->setEnabled((text.length()!=0));
}

void GroupManagementWindow::handlePubPrivToggle(int) {
	if (!myGroup)
		return;
	ui.savechanges->setVisible(true);
}

void GroupManagementWindow::handleDone() {
	if (myGroup && !myGroup->twitterList) {
		handleDeleteGroup();
	}
	close();
}

void GroupManagementWindow::handleAddUser() {
	if (!myGroup)
		return;

	QString name = ui.addusername->text();
	QListWidgetItem *i=new GroupListWidgetItem(name);
	i->setIcon(QIcon(images->getPixMap("")));
	ui.groupmembers->addItem(i);
	i->setSelected(true);
}
