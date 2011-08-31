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
/* $Id: Group.h,v 1.5 2010/07/25 14:51:22 ian Exp $

 File       : Group.h
 Create date: 00:19:41 6 Dec 2009
 Project    : yasst

 (c) 2009 Ian Clark

 */

#ifndef GROUP_H_
#define GROUP_H_

#include <QStringList>
#include <QString>
#include <QTimer>
#include <QMutex>
#include "HTTPHandler.h"

class BirdBox;
class UserGroupTweetFetcher;

class Group : public HTTPHandler {
	Q_OBJECT
public:
	Group(BirdBox*);
	virtual ~Group();

	//void addMemberByScreenName(QString);
	QString id;
	QString local_id;
	QString name;
	QString description;
	QString full_name;
	QString slug;
	QString uri;
	QString user;
	QString pass;
	QString ownerScreenName;
	int subscriber_count;
	int member_count;
	bool twitterList;
	bool privateList;
	QStringList groupmembers;
	QStringList newMembers;
	QStringList removedMembers;
	QStringList newMembersWaiting;
	QStringList removedMembersWaiting;

	BirdBox *tweets;

	void setGroupMembers(QStringList);
	void updateGroupMembers(QStringList);
	void updateGroupMembers(QString);
	void removeGroupMember(QString);
	void updateName(QString);
	void updateIsPrivate(bool);
	void convertToLocal();
	void hello();
	void goodbye();
	int getRefCount();
	void updateMembers();
	void refresh();
protected:
	UserGroupTweetFetcher *fetcher;
	int refCount;

private:
	QTimer drainTimer;
	QMutex waitingLock;
	bool populateUsers;

	void twitterRemoveListMember(QString);
	void twitterAddListMember(QString);
	void updateTwitter();
private slots:
	void updateFetcher();
	void drainPendingLists();
	void drainWaitingLists();

signals:
	void groupUpdated(Group*);
};


#endif /* GROUP_H_ */
