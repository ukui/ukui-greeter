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

#define LEFT_ARROW_X 0
#define ITEM1_X (CENTER_ENTRY_WIDTH/2)
#define ITEM2_X (ITEM1_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH)
#define ITEM3_X (ITEM2_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH )
#define ITEM4_X (ITEM3_X + CENTER_ENTRY_WIDTH*3 - ENTRY_WIDTH*2)
#define ITEM5_X (ITEM4_X + CENTER_ENTRY_WIDTH * 2 - ENTRY_WIDTH)
#define RIGHT_ARROW_X (CENTER_ENTRY_WIDTH*9.5 - ENTRY_WIDTH*4)
#define ITEM_Y CENTER_ENTRY_HEIGHT-ENTRY_HEIGHT
#define ITEM_CENTER_Y  0

UsersView::UsersView(QWidget *parent) :
    QWidget(parent),
    usersModel(nullptr),
    currentUser(0),
    lasttime(QTime::currentTime()),
    mouseClickLast(QTime::currentTime())
{
    QSize size = QApplication::primaryScreen()->size();
    scale = QString::number(size.width() / 1920.0, 'f', 1).toFloat();
    scale = scale > 0.5 ? scale : (width() >= 800 ? 0.5 : scale);
	
    if(scale > 1)
	    scale = 1;
// CENTER_ENTRY_WIDTH*5 +4* (CENTER_ENTRY_WIDTH - ENTRY_WIDTH*) +CENTER_ENTRY_WIDTH 
// 计算头像列表的宽度，宽度计算公式为5个大头像的宽加上四个间距，再加上两边左右箭头的
// 宽度
    resize(CENTER_ENTRY_WIDTH*10 - ENTRY_WIDTH*4 , CENTER_ENTRY_HEIGHT);
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

    prevArrow = new QPushButton(this);
    prevArrow->setObjectName("prevArrow");
    prevArrow->setIcon(QIcon(":/resource/prev.png"));
    prevArrow->setIconSize(QSize(64,64));
    prevArrow->setGeometry(LEFT_ARROW_X,0,CENTER_ENTRY_WIDTH/2,CENTER_ENTRY_HEIGHT);
    prevArrow->hide();
    connect(prevArrow,&QPushButton::clicked,this,[this](){
        leftKeyPressed(true);
    });

    nextArrow = new QPushButton(this);
    nextArrow->setObjectName("nextArrow");
    nextArrow->setIcon(QIcon(":/resource/next.png"));
    nextArrow->setIconSize(QSize(64,64));
    nextArrow->setGeometry(RIGHT_ARROW_X,0,CENTER_ENTRY_WIDTH/2,CENTER_ENTRY_HEIGHT);
    nextArrow->hide();
    connect(nextArrow,&QPushButton::clicked,this,[this](){
        rightKeyPressed(true);
    });

}

bool UsersView::eventFilter(QObject *obj, QEvent *event)
{

    if(event->type() == QEvent::MouseButtonRelease){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i)){
                    //添加定时器，防止点击过快。
//                    int interval = mouseClickLast.msecsTo(QTime::currentTime());
//                    if(interval < 100 && interval > -100)
//                        return false;
                    mouseClickLast = QTime::currentTime();

                    if(i == currentUser - 1){
                        leftKeyPressed(true);
                    }
                    else if(i == currentUser -2){
                        leftKeyPressed(false);
                        leftKeyPressed(true);
                    }
                    else if(i == currentUser + 1){
                        rightKeyPressed(true);
                    }
                    else if(i == currentUser + 2){
                        rightKeyPressed(false);
                        rightKeyPressed(true);
                    }
            }
        }
    }

    if(event->type() == QEvent::Enter){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i)){
//                if(i == currentUser - 2)
//                    userlist.at(i)->setGeometry(ITEM1_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
//                else if(i == currentUser - 1)
//                    userlist.at(i)->setGeometry(ITEM2_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
//                else if(i == currentUser + 1)
//                    userlist.at(i)->setGeometry(ITEM4_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
//                else if(i == currentUser + 2)
//                    userlist.at(i)->setGeometry(ITEM5_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
                userlist.at(i)->setEnterEvent(true);
            }
        }
    }

    if(event->type() == QEvent::Leave){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i)){
//                if(i == currentUser - 2)
//                    userlist.at(i)->setGeometry(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
//                else if(i == currentUser - 1)
//                    userlist.at(i)->setGeometry(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
//                else if(i == currentUser + 1)
//                    userlist.at(i)->setGeometry(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
//                else if(i == currentUser + 2)
//                    userlist.at(i)->setGeometry(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
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
        rightKeyPressed(true);
    }
    else if (key.compare("left",Qt::CaseInsensitive)==0) {
        leftKeyPressed(true);
    }
}

void UsersView::leftKeyPressed(bool isChoosed)
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

    if(isChoosed){
        QModelIndex index = usersModel->index(x, 0);
        Q_EMIT currentUserChanged(index);
        Q_EMIT userSelected(index);
    }

    if(currentUser >= 3)
        prevArrow->show();
    else
        prevArrow->hide();

    if(currentUser + 2 < userlist.count()-1)
        nextArrow->show();
    else
        nextArrow->hide();

}

void UsersView::rightKeyPressed(bool isChoosed)
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

   if(isChoosed){
       QModelIndex index = usersModel->index(x, 0);
       Q_EMIT currentUserChanged(index);
       Q_EMIT userSelected(index);
   }

   if(currentUser >= 3)
       prevArrow->show();
   else
       prevArrow->hide();

   if(currentUser + 2 < userlist.count()-1)
       nextArrow->show();
   else
       nextArrow->hide();

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
                       ITEM_Y,
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
                       ITEM_Y,
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
                       ITEM_Y,
                       ENTRY_WIDTH,
                       ENTRY_HEIGHT);

    moveAnimation(entry,rec1,rec2);

    if(left >= 2){
        UserEntry *entry2 = userlist.at(currentUser-2);
        rec1 = entry2->geometry();
        rec2 = QRect(ITEM1_X+ENTRY_WIDTH/2,ITEM_Y/2,0,0);
        moveAnimation(entry2,rec1,rec2);
    }

}

void UsersView::leftToRight()
{
    int left = currentUser;

    if(left <= 0)
        return ;

    UserEntry *entry = userlist.at(currentUser-1);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);
    entry->setSelected();

    if(left >= 2){
        UserEntry *entry2 = userlist.at(currentUser - 2);
        rec1 = entry2->geometry();
        rec2 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);

        moveAnimation(entry2,rec1,rec2);
    }

    if(left >= 3){
        UserEntry *entry3 = userlist.at(currentUser - 3);
      //  entry3->setGeometry(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
        rec1 = QRect(ITEM1_X+ENTRY_WIDTH/2,ITEM_Y/2,0,0);
        rec2 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
        entry3->show();
        moveAnimation(entry3,rec1,rec2);

    }

}


void UsersView::rightToLeft()
{
    int right = userlist.count() - currentUser - 1;

    if(right <= 0)
        return ;

    UserEntry *entry = userlist.at(currentUser + 1);
    QRect rec1 = entry->geometry();
    QRect rec2 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);
    entry->setSelected();

    if(right >= 2){
        UserEntry *entry2 = userlist.at(currentUser + 2);
        rec1 = entry2->geometry();
        rec2 = QRect(ITEM4_X,
                     ITEM_Y,
                     ENTRY_WIDTH,
                     ENTRY_HEIGHT);
        moveAnimation(entry2,rec1,rec2);
    }

    if(right >= 3){
        UserEntry *entry3 = userlist.at(currentUser + 3);
        rec1 = QRect(ITEM5_X+ENTRY_WIDTH/2,ITEM_Y/2,0,0);
        rec2 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
        entry3->show();
        moveAnimation(entry3,rec1,rec2);
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
                       ITEM_Y,
                       ENTRY_WIDTH,
                       ENTRY_HEIGHT);

    moveAnimation(entry,rec1,rec2);

    if(right >= 2){
        UserEntry *entry2 = userlist.at(currentUser + 2);
        rec1 = entry2->geometry();
        rec2 = QRect(ITEM5_X+ENTRY_WIDTH/2,ITEM_Y/2,0,0);
        moveAnimation(entry2,rec1,rec2);
    }
}

void UsersView::moveAnimation(UserEntry *entry, QRect preRect, QRect nextRect)
{
    QPropertyAnimation *pScaleAnimation = new QPropertyAnimation(entry, "geometry");
    pScaleAnimation->setDuration(200);
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

    userlist.at(user) -> setGeometry(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    userlist.at(user)->show();
    userlist.at(user)->setSelected();

    for(int i = user + 1; i < userlist.count() && i - user < 3; ++i)
    {
        int val = i - user;
        if(val == 1)
            userlist.at(i)->setGeometry(ITEM4_X,
                                        ITEM_Y,
                                        ENTRY_WIDTH,
                                        ENTRY_HEIGHT);
        if(val == 2)
            userlist.at(i)->setGeometry(ITEM5_X,
                                        ITEM_Y,
                                        ENTRY_WIDTH,
                                        ENTRY_HEIGHT);

        userlist.at(i)->show();
    }

    for(int i = user - 1;i >= 0 && user - i < 3; i--)
    {
        int val = user - i;
        if(val == 1)
            userlist.at(i)->setGeometry(ITEM2_X,
                                        ITEM_Y,
                                        ENTRY_WIDTH,
                                        ENTRY_HEIGHT);
        if(val == 2)
            userlist.at(i)->setGeometry(ITEM1_X,
                                        ITEM_Y,
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

    if(currentUser >= 3)
        prevArrow->show();
    else
        prevArrow->hide();

    if(currentUser + 2 < userlist.count()-1)
        nextArrow->show();
    else
        nextArrow->hide();

    QModelIndex index = usersModel->index(x, 0);
    Q_EMIT currentUserChanged(index);
    Q_EMIT userSelected(index);


}

