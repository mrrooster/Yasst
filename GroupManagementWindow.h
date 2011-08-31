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
#ifndef GROUPMANAGEMENTWINDOW_H
#define GROUPMANAGEMENTWINDOW_H

#include <QtGui/QDialog>
#include "ui_GroupManagementWindow.h"
#include "backend/BirdBox.h"
#include "backend/ImageCache.h"
#include "GroupListWidgetItem.h"

class GroupManagementWindow : public QDialog
{
    Q_OBJECT

public:
    GroupManagementWindow(ImageCache *i,QWidget *parent = 0);
    ~GroupManagementWindow();

    void showGroupManagement(BirdBox *b);

private:
    Ui::GroupManagementWindowClass ui;
    BirdBox *tweets;
    ImageCache *images;
    Group *myGroup;

    void setupGroup(Group *);
private slots:
	void handleGroupUpdated(Group*);
	void handleItemClicked(QListWidgetItem*);
	void handleImageUpdate(QString);
	void handleSelectionChanged();
	void handleTextChanged ( const QString & );
	void handleAddUserNameChanged ( const QString & );
	void handlePubPrivToggle(int);
	void handleNewGroup();
	void handleDeleteGroup();
	void handleSaveChanges();
	void handleAddUser();
	void setup();
	void handleDone();
};

#endif // GROUPMANAGEMENTWINDOW_H
