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
#include <QDateTime>
#include <QCoreApplication>
#include <QLightDM/UsersModel>
#include <QPropertyAnimation>
#include <unistd.h>
#include <QGraphicsOpacityEffect>
#include "xeventmonitor.h"

#define ITEM1_X 0
#define ITEM2_X (ITEM1_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH)
#define ITEM3_X (ITEM2_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH )
#define ITEM4_X (ITEM3_X + CENTER_ENTRY_WIDTH*3 - ENTRY_WIDTH*2)
#define ITEM5_X (ITEM4_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH)

UsersView::UsersView(QWidget *parent) :
    QWidget(parent),
    usersModel(nullptr),
    currentUser(0),
    lasttime(QTime::currentTime())
{
    resize(CENTER_ENTRY_WIDTH*9 - ENTRY_WIDTH*4 , CENTER_ENTRY_HEIGHT);
    initUI();
}

UsersView::~UsersView()
{
}


void UsersView::initUI()
{
    setFocusPolicy(Qt::NoFocus);
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
    if(userName.isEmpty() || userlist.count() == 0)
        return ;
    for(int i = 0;i<userlist.count();i++)
    {
        UserEntry *entry = userlist.at(i);
        if(entry->userIndex().data(QLightDM::UsersModel::NameRole).toString() == userName)
        {
            setCurrentRow(i);
            return ;
        }
    }
}

void UsersView::onGlobalKeyRelease(const QString &key)
{
    int interval = lasttime.msecsTo(QTime::currentTime());
    if(interval < 200 && interval > -200)
        return ;
    lasttime = QTime::currentTime();
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

    entry->hide();
    userlist.append(entry);
}

void UsersView::removeUserEntry(int row)
{
    QModelIndex index = usersModel->index(row,0);
    uid_t uid = index.data(QLightDM::UsersModel::UidRole).toUInt();

    for(int i=0;i<userlist.count();i++)
    {
        UserEntry *entry = userlist.at(i);
        uid_t id = entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();
        if(id == uid){
            userlist.takeAt(i);
            currentUser = 0;
        }
    }
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

    for(int i = left; i <= right; i++){
        removeUserEntry(i);
    }
}

void UsersView::onUserChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    int left = topLeft.row();
    int right = topLeft.row();

    for(int i = left;i<=right;i++)
    {
        QModelIndex index = usersModel->index(i,0);
        uid_t uid =  index.data(QLightDM::UsersModel::UidRole).toUInt();
        for(int j = 0;j<userlist.count();j++)
        {
            UserEntry *entry = userlist.at(j);
            uid_t id = entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();
            if(id == uid)
            {
                entry->setUserName(index.data(Qt::DisplayRole).toString());
                entry->setFace(index.data(QLightDM::UsersModel::ImagePathRole).toString());
                entry->setLogin(index.data(QLightDM::UsersModel::LoggedInRole).toBool());
                break;
            }
        }
    }
}

void UsersView::centerToleft()
{
    if(currentUser >= userlist.count() - 1)
        return ;

    UserEntry *entry = userlist.at(currentUser);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(ITEM2_X,
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
    QRect rec2 = QRect(ITEM4_X,
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
    QRect rec2 = QRect(ITEM1_X,
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
    QRect rec2 = QRect(ITEM3_X,0,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);
    entry->setSelected();

    if(left >= 2){
        UserEntry *entry2 = userlist.at(currentUser - 2);
        rec1 = entry2->geometry();
        rec2 = QRect(ITEM2_X,CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,ENTRY_WIDTH,ENTRY_HEIGHT);

        moveAnimation(entry2,rec1,rec2);
    }

    if(left >= 3){
        UserEntry *entry3 = userlist.at(currentUser - 3);
        entry3->setGeometry(ITEM1_X,CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,ENTRY_WIDTH,ENTRY_HEIGHT);
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
    QRect rec2 = QRect(ITEM3_X,0,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);
    entry->setSelected();

    if(right >= 2){
        UserEntry *entry2 = userlist.at(currentUser + 2);
        rec1 = entry2->geometry();
        rec2 = QRect(ITEM4_X,
                     CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                     ENTRY_WIDTH,
                     ENTRY_HEIGHT);
        moveAnimation(entry2,rec1,rec2);
    }

    if(right >= 3){
        UserEntry *entry3 = userlist.at(currentUser + 3);
        entry3->setGeometry(ITEM5_X,
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
    QRect rec2 = QRect(ITEM5_X,
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

    userlist.at(user) -> setGeometry(ITEM3_X,0,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    userlist.at(user)->show();
    userlist.at(user)->setSelected();

    for(int i = user + 1; i < userlist.count() && i - user < 3; ++i)
    {
        int val = i - user;
        if(val == 1)
            userlist.at(i)->setGeometry(ITEM4_X,
                                        CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                                        ENTRY_WIDTH,
                                        ENTRY_HEIGHT);
        if(val == 2)
            userlist.at(i)->setGeometry(ITEM5_X,
                                        CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                                        ENTRY_WIDTH,
                                        ENTRY_HEIGHT);

        userlist.at(i)->show();
    }

    for(int i = user - 1;i >= 0 && user - i < 3; i--)
    {
        int val = user - i;
        if(val == 1)
            userlist.at(i)->setGeometry(ITEM2_X,
                                        CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT,
                                        ENTRY_WIDTH,
                                        ENTRY_HEIGHT);
        if(val == 2)
            userlist.at(i)->setGeometry(ITEM1_X,
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

