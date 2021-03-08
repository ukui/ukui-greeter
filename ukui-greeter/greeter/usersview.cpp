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
    secUser(SecurityUser::instance()),
    lasttime(QTime::currentTime()),
    mouseClickLast(QTime::currentTime())
{
    QSize size = QApplication::primaryScreen()->size();
    scale = QString::number(size.width() / 1920.0, 'f', 1).toFloat();

    if(scale > 1)
	    scale = 1;
// CENTER_ENTRY_WIDTH*5 +4* (CENTER_ENTRY_WIDTH - ENTRY_WIDTH*) +CENTER_ENTRY_WIDTH 
// 计算头像列表的宽度，宽度计算公式为5个大头像的宽加上四个间距，再加上两边左右箭头的
// 宽度
    resize(CENTER_ENTRY_WIDTH*9 - ENTRY_WIDTH*4 + 240*scale , CENTER_ENTRY_HEIGHT);
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
    prevArrow->setIconSize(QSize(19*scale,52*scale));
    prevArrow->setGeometry(LEFT_ARROW_X,CENTER_ENTRY_WIDTH - ENTRY_WIDTH + (ENTRY_WIDTH - 52*scale)/2,19*scale,52*scale);
    prevArrow->hide();
    connect(prevArrow,&QPushButton::clicked,this,[this](){
        leftKeyPressed(true);
    });

    nextArrow = new QPushButton(this);
    nextArrow->setObjectName("nextArrow");
    nextArrow->setIconSize(QSize(19*scale,52*scale));
    nextArrow->setGeometry(RIGHT_ARROW_X,CENTER_ENTRY_WIDTH - ENTRY_WIDTH + (ENTRY_WIDTH - 52*scale)/2,19*scale,52*scale);
    nextArrow->hide();
    connect(nextArrow,&QPushButton::clicked,this,[this](){
        rightKeyPressed(true);
    });

}

void UsersView::resizeEvent(QResizeEvent *)
{
    prevArrow->setGeometry(LEFT_ARROW_X,CENTER_ENTRY_WIDTH - ENTRY_WIDTH + (ENTRY_WIDTH - 52*scale)/2,19*scale,52*scale);
    nextArrow->setGeometry(RIGHT_ARROW_X,CENTER_ENTRY_WIDTH - ENTRY_WIDTH + (ENTRY_WIDTH - 52*scale)/2,19*scale,52*scale);
    setCurrentRow(currentUser);
}

bool UsersView::eventFilter(QObject *obj, QEvent *event)
{
    if(userlist.count() <= 1)
        return false;

    if(event->type() == QEvent::MouseButtonRelease){
	/*为防止双击时点中当前头像和下一个头像，添加延时，确保只点中当前头像*/
        int interval = mouseClickLast.msecsTo(QTime::currentTime());
        if(interval < 300 && interval > -300)
            return false;
        mouseClickLast = QTime::currentTime();
        for(int i=0;i<userlist.count();i++){
            int next = userlist.at(currentUser).second.second; //当前用户的下一个用户下标
            int prev = userlist.at(currentUser).second.first; //当前用户的上一个用户下标
            int nnext = userlist.at(next).second.second;//当前用户的下下个用户下标
            int pprev = userlist.at(prev).second.first;//当前用户的上上个用户下标

            if(obj == userlist.at(i).first){
                    if(i == prev){
                        leftKeyPressed(true);
                    }
                    else if(i == next){
                        rightKeyPressed(true);
                    }
                    else if(i == nnext && i != currentUser){
			    /*这里要放在下一条语句之前，否则当只有四个用户时，左移变右移*/
                        rightKeyPressed(false);
                        rightKeyPressed(true);
                    }
		    else if(i == pprev && i != currentUser){
                        leftKeyPressed(false);
                        leftKeyPressed(true);
                    }
            }
        }
    }
    /*鼠标移动到头像上事件*/
    if(event->type() == QEvent::Enter){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i).first){
                userlist.at(i).first->setEnterEvent(true);
            }
        }
    }
    /*鼠标移出头像事件*/
    if(event->type() == QEvent::Leave){
        for(int i=0;i<userlist.count();i++){
            if(obj == userlist.at(i).first){
                userlist.at(i).first->setEnterEvent(false);
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

    setCurrentRow(0);
}

void UsersView::setCurrentUser(const QString &userName, bool entered)
{
    if(userName.isEmpty() || userlist.count() == 0)
        return ;
    for(int i = 0;i<userlist.count();i++)
    {
        UserEntry *entry = userlist.at(i).first;
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
    /*添加一定延时，避免按键太快*/	
    int interval = lasttime.msecsTo(QTime::currentTime());
    if(interval < 300 && interval > -300)
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
    if(userlist.count() <= 1)
       return;

    rightToRight();
    centerToRight();
    leftToRight();

    currentUser = userlist.at(currentUser).second.first;

    UserEntry *entry = userlist.at(currentUser).first;
    uid_t uid =  entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();


    int x = 0;
    for(x = 0; x < usersModel->rowCount(); x++)
    {
        uid_t id= usersModel->index(x).data(QLightDM::UsersModel::UidRole).toUInt();
        if(uid == id)
            break;
    }
    
    /*只有isChoosed为true时，才会触发切换用户信号，否则只是在界面上右显示，
     * 用于处理点击距离当前用户一个位置以上时，多次移动用户的问题*/
    if(isChoosed){
        QModelIndex index = usersModel->index(x, 0);
        Q_EMIT currentUserChanged(index);
        Q_EMIT userSelected(index);
    }

}

void UsersView::rightKeyPressed(bool isChoosed)
{
    if(userlist.count() <= 1)
        return ;

   leftToLeft();
   centerToleft();
   rightToLeft();
   currentUser = userlist.at(currentUser).second.second;
   UserEntry *entry = userlist.at(currentUser).first;
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
    
    int prev = 0,next = 0;
    if(userlist.count() >= 1)
    {
        //第一个用户的上一个用户指向即将添加的下一个用户。
        userlist.last().second.second = userlist.count();
        //原来最后一个用户的下一个用户指向即将添加的下一个用户。
        userlist.first().second.first = userlist.count();
        //新添加的用户放在最后，所以上一个用户是原来的最后一个用户。
        prev = userlist.count() - 1;
    }

    QPair<int,int> pairindex(prev,next);
    QPair<UserEntry*,QPair<int,int>> pair(entry,pairindex);
    userlist.append(pair);

    /*记录当前用户两边可显示的用户数量*/
    int usersCount = userlist.count();
       switch (usersCount) {
       case 0:
           return;
           break;
       case 1:
           leftCount = rightCount = 0;
           break;
       case 2:
           leftCount = 0;
           rightCount = 1;
           break;
       case 3:
           leftCount = rightCount = 1;
           break;
       case 4:
           leftCount = 1;
           rightCount = 2;
           break;
       default:
            leftCount = rightCount = 2;
           break;
       }

    if(userlist.count() > 5){
        prevArrow->show();
        nextArrow->show();
    }
}

void UsersView::removeUserEntry(int row)
{
//    QModelIndex index = usersModel->index(row,0);
//    uid_t uid = index.data(QLightDM::UsersModel::UidRole).toUInt();

//    for(int i=0;i<userlist.count();i++)
//    {
//        UserEntry *entry = userlist.at(i).first;
//        uid_t id = entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();
//        if(id == uid){
//            int prev = userlist.at(i).second.first;
//            int next = userlist.at(i).second.second;

//            QPair<int,int> swap(userlist.at(prev).second.first,next);
//            QPair<UserEntry*,QPair<int,int>> tmp(userlist.at(prev).first,swap);
//            userlist.at(prev) = tmp;
//            userlist.takeAt(i);
//            currentUser = 0;
//        }
//    }

//    if(userlist.count() <= 5){
//        prevArrow->hide();
//        nextArrow->hide();
//    }
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
            UserEntry *entry = userlist.at(j).first;
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

/*
 * 当前用户往左移，走到这里，说明至少存在两个用户，
 *当只有两个用户时，头像不能左移，而是隐藏,这里必须写明坐标，
 * 否则当切换头像过快时，会出现坐标不对的问题。
*/
void UsersView::centerToleft()
{
    UserEntry *entry = userlist.at(currentUser).first;
    if(userlist.count() >= 1){
        QRect rec1 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
        QRect rec2 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);

        moveAnimation(entry,rec1,rec2);
    }
    else{
        QRect rec1 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
        QRect rec2 = QRect(ITEM3_X+CENTER_ENTRY_WIDTH/2,
                           ITEM_CENTER_Y+CENTER_ENTRY_HEIGHT/2,
                           0,0);
        moveAnimation(entry,rec1,rec2);
    }
}

/*当前用户头像右移，当只有两个用户时，不往右移，而是隐藏*/
void UsersView::centerToRight()
{

    UserEntry *entry = userlist.at(currentUser).first;
    if(userlist.count() >= 1){
        QRect rec1 =  QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
        QRect rec2 = QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);

        moveAnimation(entry,rec1,rec2);
    }
    else{
        QRect rec1 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
        QRect rec2 = QRect(ITEM3_X+CENTER_ENTRY_WIDTH/2,
                           ITEM_CENTER_Y+CENTER_ENTRY_HEIGHT/2,
                           0,0);
        moveAnimation(entry,rec1,rec2);
    }
}
/*
 *左边的两个头像左移，需要考虑三种情况
 *leftCount == 0 : 返回
 *leftCount == 1 : 左边只有一个头像，隐藏
 *leftCount == 2: 最左边头像隐藏，左边第二个移动到最左边
*/
void UsersView::leftToLeft()
{
    if(leftCount == 0)
        return ;
    else if(leftCount == 1){
        int index = userlist.at(currentUser).second.first;
        UserEntry *entry = userlist.at(index).first;
        QRect rec1 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
        QRect rec2 = QRect(ITEM2_X+ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
        moveAnimation(entry,rec1,rec2);
        return ;
    }

    /*leftCount == 2*/
    int swap = currentUser;
    swap = userlist.at(swap).second.first;
    UserEntry *entry = userlist.at(swap).first;
    QRect rec1 =QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
    QRect rec2 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);

    swap = userlist.at(swap).second.first;
    QRect rec3 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
    QRect rec4 = QRect(ITEM1_X+ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
    moveAnimation(userlist.at(swap).first,rec3,rec4);

}

/*
* 当前头像左边的头像右移
*/
void UsersView::leftToRight()
{
    int swap = currentUser;
    for(int i = 0;i<=leftCount;i++){
         swap = userlist.at(swap).second.first;
         UserEntry *entry = userlist.at(swap).first;
         entry->show();
         QRect rec1,rec2;
         if(i == 0){
             if(leftCount == 0){
                 rec1 = QRect(ITEM3_X+CENTER_ENTRY_WIDTH/2,
                              ITEM_CENTER_Y+CENTER_ENTRY_HEIGHT/2,
                              0,0);
             }else{
                 rec1 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
             }
             entry->setSelected();
             rec2 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
         }else if(i == 1){
             if(leftCount == 1){
                 rec1 = QRect(ITEM2_X + ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
             }else if(leftCount == 2){
                 rec1 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
             }
             rec2 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
         }else{
             rec1 = QRect(ITEM1_X+ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
             rec2 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
         }

         moveAnimation(entry,rec1,rec2);
    }
}

/*右边部分头像左移*/
void UsersView::rightToLeft()
{
    int swap = currentUser;
    for(int i = 0;i<=rightCount;i++){
         swap = userlist.at(swap).second.second;
         UserEntry *entry = userlist.at(swap).first;
         entry->show();
         QRect rec1,rec2;
         if(i == 0){
                 rec1 = QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
                 rec2 = QRect(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
                 entry->setSelected();
         }else if(i == 1){
             if(rightCount == 1){
                 rec1 = QRect(ITEM4_X + ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
             }else if(rightCount == 2){
                 rec1 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
             }
             rec2 = QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
         }else{
             rec1 = QRect(ITEM5_X+ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
             rec2 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
         }

         moveAnimation(entry,rec1,rec2);
    }
}

/*右边部分头像右移*/
void UsersView::rightToRight()
{
    if(rightCount == 0)
        return ;
    else if(rightCount == 1){
        int index = userlist.at(currentUser).second.second;
        UserEntry *entry = userlist.at(index).first;
        QRect rec1 = QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
        QRect rec2 = QRect(ITEM4_X+ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
        moveAnimation(entry,rec1,rec2);
        return ;
    }

    /*leftCount == 2*/
    int swap = currentUser;
    swap = userlist.at(swap).second.second;
    UserEntry *entry = userlist.at(swap).first;
    QRect rec1 =QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
    QRect rec2 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
    moveAnimation(entry,rec1,rec2);

    swap = userlist.at(swap).second.second;
    QRect rec3 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT);
    QRect rec4 = QRect(ITEM5_X+ENTRY_WIDTH/2,ITEM_Y+ENTRY_HEIGHT/2,0,0);
    moveAnimation(userlist.at(swap).first,rec3,rec4);
}

/*移动的动画效果*/
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

    if(currentUser == user){
        for(int i=0;i<userlist.count();i++)
            userlist.at(i).first->hide();

            //当前用户保持居中
        userlist.at(user).first -> setGeometry(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
        userlist.at(user).first->show();
        userlist.at(user).first->setSelected();

        int swap = currentUser;

        //设置当前用户左边的用户显示
        for(int i = 0;i<leftCount;i++){
            swap = userlist.at(swap).second.first;

            QRect rec1 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //左边第二个用户的大小
            QRect rec2 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //左边第一个用户的大小

            if(i == 0)
                userlist.at(swap).first->setGeometry(rec1);
            else if(i == 1)
                userlist.at(swap).first->setGeometry(rec2);

            userlist.at(swap).first->show();
        }

        //设置当前用户右边的用户显示
        swap = currentUser;
        for(int i = 0;i<rightCount;i++){
            swap = userlist.at(swap).second.second;

            QRect  rec1 = QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //从右往左第二个用户
            QRect rec2 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //从右往左第一个用户

            if(i == 0)
                userlist.at(swap).first->setGeometry(rec1);
            else if(i == 1)
                userlist.at(swap).first->setGeometry(rec2);

            userlist.at(swap).first->show();
        }

        return ;
    }

    currentUser = user;

    for(int i=0;i<userlist.count();i++)
        userlist.at(i).first->hide();

    //当前用户保持居中
    userlist.at(user).first -> setGeometry(ITEM3_X,ITEM_CENTER_Y,CENTER_ENTRY_WIDTH,CENTER_ENTRY_HEIGHT);
    userlist.at(user).first->show();
    userlist.at(user).first->setSelected();

    int swap = currentUser;

    //设置当前用户左边的用户显示
    for(int i = 0;i<leftCount;i++){
        swap = userlist.at(swap).second.first;
        
        QRect rec1 = QRect(ITEM2_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //左边第二个用户的大小
        QRect rec2 = QRect(ITEM1_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //左边第一个用户的大小
        
        if(i == 0)
            userlist.at(swap).first->setGeometry(rec1);
        else if(i == 1)
            userlist.at(swap).first->setGeometry(rec2);
                           
        userlist.at(swap).first->show();
    }

    //设置当前用户右边的用户显示
    swap = currentUser;
    for(int i = 0;i<rightCount;i++){
        swap = userlist.at(swap).second.second;

        QRect  rec1 = QRect(ITEM4_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //从右往左第二个用户
        QRect rec2 = QRect(ITEM5_X,ITEM_Y,ENTRY_WIDTH,ENTRY_HEIGHT); //从右往左第一个用户

        if(i == 0)
            userlist.at(swap).first->setGeometry(rec1);
        else if(i == 1)
            userlist.at(swap).first->setGeometry(rec2);

        userlist.at(swap).first->show();
    }

    //获取当前用户对应的model下标
    UserEntry *entry = userlist.at(currentUser).first;
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

