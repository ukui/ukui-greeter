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
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
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
#include "securityuser.h"

/*记录五个节点的坐标*/
#define ARROW_WIDTH 120*scale
#define LEFT_ARROW_X 0
#define ITEM1_X (ARROW_WIDTH)
#define ITEM2_X (ITEM1_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH)
#define ITEM3_X (ITEM2_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH )
#define ITEM4_X (ITEM3_X + CENTER_ENTRY_WIDTH*3 - ENTRY_WIDTH*2)
#define ITEM5_X (ITEM4_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH)
#define RIGHT_ARROW_X (CENTER_ENTRY_WIDTH*9 - ENTRY_WIDTH*4 + 220*scale)
#define ITEM_Y CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT
#define ITEM_CENTER_Y  0

UsersView::UsersView(QWidget *parent) :
    QWidget(parent),
    usersModel(nullptr),
    currentUser(-1),
    prevArrow(nullptr),
    nextArrow(nullptr),
    secUser(SecurityUser::instance()),
    lasttime(QTime::currentTime()),
    mouseClickLast(QTime::currentTime())
{
    QSize size = QApplication::primaryScreen()->size();
    scale = QString::number(size.width() / 1920.0, 'f', 1).toFloat();

    if(scale > 1)
        scale = 1;

    setFocusPolicy(Qt::NoFocus);
    connect(XEventMonitor::instance(), SIGNAL(keyPress(const QString &)),
            this, SLOT(onGlobalKeyRelease(const QString &)));

    setAttribute(Qt::WA_TranslucentBackground);

}

UsersView::~UsersView()
{
}

void UsersView::resizeEvent(QResizeEvent *)
{
    setCurrentRow(currentUser);
}

bool UsersView::eventFilter(QObject *obj, QEvent *event)
{
    if(userlist.count() < 1)
        return false;

    for(int i = 0;i<userlist.count();i++){
        if(userlist.at(i) == obj && event->type() == QEvent::MouseButtonRelease){
            uid_t uid =  userlist.at(i)->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();


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
            break;
        }
    }

    /*鼠标移动到头像上事件*/
    if(event->type() == QEvent::Enter){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i)){
                userlist.at(i)->setEnterEvent(true);
            }
        }
    }
    /*鼠标移出头像事件*/
    if(event->type() == QEvent::Leave){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i)){
                userlist.at(i)->setEnterEvent(false);
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
    
    for(int i = 0; i < usersModel->rowCount(); i++){
        QString name = usersModel->index(i).data(QLightDM::UsersModel::NameRole).toString();
        if(secUser->isSecrityUser(name))
            insertUserEntry(i);
    }

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
            if(i != currentUser)
                setCurrentRow(i);
            return ;
        }
        else if(entry->userIndex().data(QLightDM::UsersModel::NameRole).toString() == "*login")
        {
            entry->setUserName(userName);
            return ;
       }	
    }
}

void UsersView::onGlobalKeyRelease(const QString &key)
{

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

}

void UsersView::onUserAdded(const QModelIndex &parent, int left, int right)
{
    Q_UNUSED(parent);

    for(int i = left; i <= right; i++){
        QString name = usersModel->index(i).data(QLightDM::UsersModel::NameRole).toString();
        if(secUser->isSecrityUser(name))
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

bool UsersView::getIsUser(QString username)
{
    for(int i = 0; i < usersModel->rowCount(); i++){
        QString name = usersModel->index(i).data(QLightDM::UsersModel::NameRole).toString();
    	if(name == username)
	    return true;
    }
    return false;
}

bool UsersView::getLoginStaus(QString username)
{
    for(int i = 0; i < usersModel->rowCount(); i++){
        QString name = usersModel->index(i).data(QLightDM::UsersModel::NameRole).toString();
        if(name == username){
 	    return usersModel->index(i).data(QLightDM::UsersModel::LoggedInRole).toBool();	
	}
    }
}

QSize UsersView::getSize()
{
    int w,h;

    if(userlist.count() <= 7){   //小于等于7个用户时，只用显示一行
        w = ENTRY_WIDTH*userlist.count() + C_BORDER_WIDTH*(userlist.count() - 1 );
        h = ENTRY_HEIGHT;
    }else{   //大于7个用户时，设置多行
        int line;
        if(userlist.count() % 7 == 0){
            line = userlist.count()/7;
        }else{
            line = userlist.count()/7 + 1;
        }

        w = ENTRY_WIDTH*7 + C_BORDER_WIDTH*6;
        h = ENTRY_HEIGHT*line + C_BORDER_HEIGHT*(line - 1);
    }

    QSize size(w,h);
    return size;
}

void UsersView::setCurrentRow(int user)
{

    if(userlist.count() == 0)
        return ;

    if(user < 0 || user >= userlist.count())
        user = 0;

    if(currentUser == user){
        resize(getSize());

        for(int i = 0;i<userlist.count();i++){
            int x = 0,y = 0;
            x = i % 7;
            y = i / 7;
            userlist.at(i)->setGeometry(x*(ENTRY_WIDTH + C_BORDER_WIDTH),y*(ENTRY_HEIGHT + C_BORDER_HEIGHT),ENTRY_WIDTH,ENTRY_HEIGHT);
            userlist.at(i)->show();
        }
        return ;
    }

    currentUser = user;

    resize(getSize());

    for(int i = 0;i<userlist.count();i++){
        int x = 0,y = 0;
        x = i % 7;
        y = i / 7;
        userlist.at(i)->setGeometry(x*(ENTRY_WIDTH + C_BORDER_WIDTH),y*(ENTRY_HEIGHT + C_BORDER_HEIGHT),ENTRY_WIDTH,ENTRY_HEIGHT);
        userlist.at(i)->show();
    }

    //获取当前用户对应的model下标
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

