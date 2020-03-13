/* usersview.cpp
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#include "usersview.h"
#include <QListWidget>
#include <QDebug>
#include <QPushButton>
#include <QKeyEvent>
#include <QStandardPaths>
#include <QTime>
#include <QCoreApplication>
#include <QLightDM/UsersModel>
#include <QPropertyAnimation>

#include "xeventmonitor.h"

UsersView::UsersView(QWidget *parent) :
    QWidget(parent),
    usersModel(nullptr),
    currentUser(0)
{
    resize(CENTER_ENTRY_WIDTH*5, CENTER_ENTRY_HEIGHT);
    initUI();
}

UsersView::~UsersView()
{
}


void UsersView::initUI()
{
//    usersList = new QListWidget(this);
//    usersList->setObjectName(QStringLiteral("usersListWidget"));
//    usersList->setFlow(QListWidget::LeftToRight);
//    usersList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    usersList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    usersList->setSelectionMode(QListWidget::NoSelection);
//    usersList->setFocusPolicy(Qt::NoFocus);
//    usersList->setContentsMargins(10, 0, 10, 0);
//    usersList->installEventFilter(this);

    connect(XEventMonitor::instance(), SIGNAL(keyRelease(const QString &)),
            this, SLOT(onGlobalKeyRelease(const QString &)));

}

bool UsersView::eventFilter(QObject *obj, QEvent *event)
{
    for(int i=0;i<userlist.count();i++){
        if(obj == userlist.at(i)){
            if(event->type() == QEvent::MouseButtonDblClick){
                setCurrentRow(i);
            }
        }
    }
    return false;
}

void UsersView::setModel(QAbstractListModel *model)
{
    usersModel = model;
    connect(usersModel, &QAbstractListModel::rowsInserted, this, &UsersView::onUserAdded);
    connect(usersModel, &QAbstractListModel::rowsRemoved, this, &UsersView::onUserRemoved);
    connect(usersModel, &QAbstractListModel::dataChanged, this, &UsersView::onUserChanged);

    for(int i = 0; i < usersModel->rowCount(); i++)
        insertUserEntry(i);

    setCurrentRow(0);
}

void UsersView::setCurrentUser(const QString &userName, bool entered)
{
//    if(!usersModel || userName.isEmpty())
//        return;
//    for(int i = 0; i < usersList->count(); i++)
//    {
//        QListWidgetItem *item = usersList->item(i);
//        UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(item));
//        if(userName == entry->userIndex().data(QLightDM::UsersModel::NameRole).toString())
//        {
//            setCurrentRo#define ITEM_WIDTH (CENTER_ENTRY_WIDTH)
//            return;
//        }
//    }
}

void UsersView::onGlobalKeyRelease(const QString &key)
{
    if(key.compare("right",Qt::CaseInsensitive)==0)
    {
        rightKeyPressed();
    }
    else if (key.compare("left",Qt::CaseInsensitive)==0) {
        leftKeyPressed();
    }
}

void UsersView::leftKeyPressed()
{
    if(currentUser <= 0)
       return;

    rightToRight();
    centerToRight();
    leftToRight();
    currentUser-- ;

    UserEntry *entry = userlist.at(currentUser);
    uid_t uid =  entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();


    int x = 0;
    for(x = 0; x < usersModel->rowCount(); x++)
    {
        uid_t id= usersModel->index(x).data(QLightDM::UsersModel::UidRole).toUInt();
        if(uid == id)
            break;
    }

    QModelIndex index = usersModel->index(x, 0);
    Q_EMIT currentUserChanged(index);
    Q_EMIT userSelected(index);

}

void UsersView::rightKeyPressed()
{
    if(currentUser >= userlist.count() - 1)
        return ;

   leftToLeft();
   centerToleft();
   rightToLeft();
   currentUser++;

   UserEntry *entry = userlist.at(currentUser);
   uid_t uid =  entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();

   int x = 0;
   for(x = 0; x < usersModel->rowCount(); x++)
   {
       uid_t id= usersModel->index(x).data(QLightDM::UsersModel::UidRole).toUInt();
       if(uid == id)
           break;
   }

   QModelIndex index = usersModel->index(x, 0);
   Q_EMIT currentUserChanged(index);
   Q_EMIT userSelected(index);

}

void UsersView::showEvent(QShowEvent *event)
{

    QWidget::showEvent(event);
}

void UsersView::insertUserEntry(int row)
{
    QModelIndex index = usersModel->index(row, 0);
    QPersistentModelIndex persistentIndex(index);
    UserEntry *entry = new UserEntry(this);
    entry->setUserIndex(persistentIndex);
    entry->installEventFilter(this);

    connect(entry, &UserEntry::pressed, this, &UsersView::onUserPressed);
    connect(entry, &UserEntry::clicked, this, &UsersView::onUserClicked);

    entry->hide();
    userlist.append(entry);
}

void UsersView::onUserPressed()
{
//    QString objName = sender()->objectName();
//    for(int i = 0; i < usersList->count(); i++){
//        QWidget *entry = usersList->itemWidget(usersList->item(i));
//        if(entry->objectName() == objName){
//            setCurrentRow(i);
//        }
//    }
//    update();
}

void UsersView::onUserClicked(int row)
{
    qDebug()<<"sssssssssssssssssssssssssss row = "<<row;
}

void UsersView::onUserAdded(const QModelIndex &parent, int left, int right)
{
    Q_UNUSED(parent);

    for(int i = left; i <= right; i++){
        insertUserEntry(i);
    }
}

void UsersView::onUserRemoved(const QModelIndex &parent, int left, int right)
{
    Q_UNUSED(parent);

//    for(int i = left; i <= right; i++){
//        usersList->takeItem(i);
//        setCurrentRow(usersList->currentRow());
//        if(usersList->count() < 5) {
//            usersList->resize(ITEM_WIDTH * usersList->count() + 2, ITEM_HEIGHT);
//            usersList->move(usersList->geometry().left() + ITEM_WIDTH / 2, 0);
//        }
//    }
}

void UsersView::onUserChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{

}

void UsersView::centerToleft()
{
    if(currentUser >= userlist.count() - 1)
        return ;

    UserEntry *entry = userlist.at(currentUser);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(CENTER_ENTRY_WIDTH,
                       CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                       ENTRY_WIDTH,
                       ENTRY_HEIGHT);

    moveAnimation(entry,rec1,rec2);
}

void UsersView::centerToRight()
{
    qDebug()<<"centerToRight currentuser = "<<currentUser;
    if(currentUser <= 0)
        return ;

    UserEntry *entry = userlist.at(currentUser);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(CENTER_ENTRY_WIDTH*4-ENTRY_WIDTH,
                       CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                       ENTRY_WIDTH,
                       ENTRY_HEIGHT);

    moveAnimation(entry,rec1,rec2);
}

void UsersView::leftToLeft()
{
    int left = currentUser;

    if(left <= 0){
        return ;
    }

    UserEntry *entry = userlist.at(currentUser-1);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(0,
                       CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                       ENTRY_WIDTH,
                       ENTRY_HEIGHT);

    moveAnimation(entry,rec1,rec2);

    if(left >= 2){
        UserEntry *entry2 = userlist.at(currentUser-2);
        entry2->hide();
    }

}

void UsersView::leftToRight()
{
    int left = currentUser;

    if(left <= 0)
        return ;

    UserEntry *entry = userlist.at(currentUser-1);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(CENTER_ENTRY_WIDTH*2,0,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);
    entry->setSelected();

    if(left >= 2){
        UserEntry *entry2 = userlist.at(currentUser - 2);
        rec1 = entry2->geometry();
        rec2 = QRect(CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,ENTRY_WIDTH,ENTRY_HEIGHT);

        moveAnimation(entry2,rec1,rec2);
    }

    if(left >= 3){
        UserEntry *entry3 = userlist.at(currentUser - 3);
        entry3->setGeometry(0,CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,ENTRY_WIDTH,ENTRY_HEIGHT);
        entry3->show();
    }
}

void UsersView::rightToLeft()
{
    int right = userlist.count() - currentUser - 1;

    if(right <= 0)
        return ;

    UserEntry *entry = userlist.at(currentUser + 1);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(CENTER_ENTRY_WIDTH*2,0,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);
    entry->setSelected();

    if(right >= 2){
        UserEntry *entry2 = userlist.at(currentUser + 2);
        rec1 = entry2->geometry();
        rec2 = QRect(CENTER_ENTRY_WIDTH*4-FACE_WIDTH,
                     CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                     ENTRY_WIDTH,
                     ENTRY_HEIGHT);
        moveAnimation(entry2,rec1,rec2);
    }

    if(right >= 3){
        UserEntry *entry3 = userlist.at(currentUser + 3);
        entry3->setGeometry(CENTER_ENTRY_WIDTH*5-ENTRY_WIDTH,
                             CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                             ENTRY_WIDTH,
                             ENTRY_HEIGHT);
        entry3->show();
    }
}

void UsersView::rightToRight()
{
    int right = userlist.count() - currentUser - 1;
    if(right <= 0)
        return ;

    UserEntry *entry = userlist.at(currentUser + 1);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(CENTER_ENTRY_WIDTH*5-ENTRY_WIDTH,
                       CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                       ENTRY_WIDTH,
                       ENTRY_HEIGHT);

    moveAnimation(entry,rec1,rec2);

    if(right >= 2){
        UserEntry *entry2 = userlist.at(currentUser + 2);
        entry2->hide();
    }
}

void UsersView::moveAnimation(UserEntry *entry, QRect preRect, QRect nextRect)
{
    QPropertyAnimation *pScaleAnimation = new QPropertyAnimation(entry, "geometry");
    pScaleAnimation->setDuration(300);
    pScaleAnimation->setStartValue(preRect);
    pScaleAnimation->setEndValue(nextRect);
    pScaleAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    pScaleAnimation->start();
}

void UsersView::setCurrentRow(int user)
{
    if(userlist.count() == 0)
        return ;

    if(user < 0 || user >= userlist.count())
        user = 0;

    currentUser = user;

    for(int i=0;i<userlist.count();i++)
        userlist.at(i)->hide();

    userlist.at(user) -> setGeometry(CENTER_ENTRY_WIDTH*2,0,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    userlist.at(user)->show();
    userlist.at(user)->setSelected();

    for(int i = user + 1; i < userlist.count() && i - user < 3; ++i)
    {
        userlist.at(i)->setGeometry(CENTER_ENTRY_WIDTH*(2+i-user)+CENTER_ENTRY_WIDTH-ENTRY_WIDTH,
                                    CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                                    ENTRY_WIDTH,
                                    ENTRY_HEIGHT);

        userlist.at(i)->show();
    }

    for(int i = user - 1;i >= 0 && user - i < 3; i--)
    {
        userlist.at(i)->setGeometry(CENTER_ENTRY_WIDTH*(2+i-user),
                                    CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                                    ENTRY_WIDTH,
                                    ENTRY_HEIGHT);
        userlist.at(i)->show();
    }

    UserEntry *entry = userlist.at(currentUser);
    uid_t uid =  entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();


    int x = 0;
    for(x = 0; x < usersModel->rowCount(); x++)
    {
        uid_t id= usersModel->index(x).data(QLightDM::UsersModel::UidRole).toUInt();
        if(uid == id)
            break;
    }

    QModelIndex index = usersModel->index(x, 0);
    Q_EMIT currentUserChanged(index);
    Q_EMIT userSelected(index);

}

