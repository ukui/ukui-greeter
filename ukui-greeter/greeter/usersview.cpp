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
#include <QLightDM/UsersModel>

#include "xeventmonitor.h"

UsersView::UsersView(QWidget *parent) :
    QWidget(parent),
    usersModel(nullptr)
{
    resize(USERSVIEW_WIDTH, USERSVIEW_HEIGHT);
    initUI();
}

UsersView::~UsersView()
{
}


void UsersView::initUI()
{
    usersList = new QListWidget(this);
    usersList->setObjectName(QStringLiteral("usersListWidget"));
    usersList->setFlow(QListWidget::LeftToRight);
    usersList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    usersList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    usersList->setSelectionMode(QListWidget::NoSelection);
    usersList->setFocusPolicy(Qt::NoFocus);
    usersList->setContentsMargins(10, 0, 10, 0);
    usersList->installEventFilter(this);

    connect(XEventMonitor::instance(), SIGNAL(keyRelease(const QString &)),
            this, SLOT(onGlobalKeyRelease(const QString &)));

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
    if(!usersModel || userName.isEmpty())
        return;
    for(int i = 0; i < usersModel->rowCount(); i++)
    {
        QString name = usersModel->index(i).data(QLightDM::UsersModel::NameRole).toString();
        if(name == userName)
        {
            setCurrentRow(i);
            if (entered)
                onUserClicked(i);
            return;
        }
    }
    Q_EMIT userNotFound(userName);
}

void UsersView::resizeEvent(QResizeEvent *event)
{
    int buttonTop = scale > 0.5 ? 10 : 5;

    int num = usersList->count() <= 5 ? usersList->count() : 5;
    usersList->resize(ITEM_WIDTH * num + 2, ITEM_HEIGHT);
    usersList->move((width()-usersList->width())/2 + 20,(height()-usersList->height())/2);
    for(int i = 0; i < usersList->count(); i++) {
        QListWidgetItem *item = usersList->item(i);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item->setTextAlignment(Qt::AlignCenter);
        UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(item));
        if(usersList->currentRow()==i)
            entry->setFixedSize(CENTER_ENTRY_WIDTH, CENTER_ENTRY_HEIGHT);
        else
            entry->setFixedSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    }
    if(usersList->count()==4||usersList->count()==2)
        usersList->move((width()-usersList->width() + ITEM_WIDTH)/2 + 20, \
                        (height()-usersList->height())/2);

    QWidget::resizeEvent(event);
}

void UsersView::moveUserEntry(int from,int to)
{
    UserEntry *fromentry = static_cast<UserEntry*>(usersList->itemWidget(usersList->item(from)));
    uid_t uid = fromentry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();
    QListWidgetItem *fromitem = usersList->takeItem(from);
    delete fromitem;

    int row = 0;
    for(row = 0; row < usersModel->rowCount(); row++)
    {
        uid_t id= usersModel->index(row).data(QLightDM::UsersModel::UidRole).toUInt();
        if(uid == id)
            break;
    }

    QModelIndex index = usersModel->index(row, 0);
    QPersistentModelIndex persistentIndex(index);
    UserEntry *entry = new UserEntry(this);
    entry->setUserIndex(persistentIndex);

    connect(entry, &UserEntry::pressed, this, &UsersView::onUserPressed);
    connect(entry, &UserEntry::clicked, this, &UsersView::onUserClicked);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    usersList->insertItem(to, item);
    usersList->setItemWidget(item, entry);
}

void UsersView::onGlobalKeyRelease(const QString &key)
{
    qDebug()<<"kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk key = "<<key;
    if(key.compare("left",Qt::CaseInsensitive)==0)
    {
        if(usersList->currentRow() < usersList->count()-1)
            setCurrentRow(usersList->currentRow() + 1);
        else if(usersList->currentRow() == 0 && usersList->count()==2)
            setCurrentRow(0);
    }
    else if (key.compare("right",Qt::CaseInsensitive)==0) {
        if(usersList->currentRow() > 0)
            setCurrentRow(usersList->currentRow() - 1);
        else if(usersList->currentRow() == 0 && usersList->count()==2)
            setCurrentRow(1);
    }
}

//void UsersView::keyReleaseEvent(QKeyEvent *event)
//{
//    switch(event->key()) {
//    case Qt::Key_Down:
//    case Qt::Key_Right:
//        if(usersList->currentRow() > 0)
//            setCurrentRow(usersList->currentRow() - 1);
//        else if(usersList->currentRow() == 0 && usersList->count()==2)
//            setCurrentRow(1);
//        break;
//    case Qt::Key_Up:
//    case Qt::Key_Left:
//        if(usersList->currentRow() < usersList->count()-1)
//            setCurrentRow(usersList->currentRow() + 1);
//        else if(usersList->currentRow() == 0 && usersList->count()==2)
//            setCurrentRow(0);
//        break;
//    case Qt::Key_Return:
//        onUserClicked(usersList->currentRow());
//        break;
//    default:
//        QWidget::keyReleaseEvent(event);
//    }

//}

void UsersView::showEvent(QShowEvent *event)
{
    this->setFocus();

    QWidget::showEvent(event);
}

void UsersView::insertUserEntry(int row)
{
    QModelIndex index = usersModel->index(row, 0);
    QPersistentModelIndex persistentIndex(index);
    UserEntry *entry = new UserEntry(this);
    entry->setUserIndex(persistentIndex);

    connect(entry, &UserEntry::pressed, this, &UsersView::onUserPressed);
    connect(entry, &UserEntry::clicked, this, &UsersView::onUserClicked);
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    usersList->insertItem(row, item);
    usersList->setItemWidget(item, entry);

    if(usersList->count() <= 5) {
        usersList->resize(ITEM_WIDTH * usersList->count() + 2, ITEM_HEIGHT);
    }
}

void UsersView::onUserPressed()
{
    QString objName = sender()->objectName();
    for(int i = 0; i < usersList->count(); i++){
        QWidget *entry = usersList->itemWidget(usersList->item(i));
        if(entry->objectName() == objName){
            setCurrentRow(i);
        }
    }
    update();
}

void UsersView::onUserClicked(int row)
{
    UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(usersList->currentItem()));
    int x = 0;
    uid_t uid =  entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();
    for(x = 0; x < usersModel->rowCount(); x++)
    {
        uid_t id= usersModel->index(x).data(QLightDM::UsersModel::UidRole).toUInt();
        if(uid == id)
            break;
    }

    Q_EMIT userSelected(usersModel->index(x, 0));
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
        usersList->takeItem(i);
        setCurrentRow(usersList->currentRow());
        if(usersList->count() < 5) {
            usersList->resize(ITEM_WIDTH * usersList->count() + 2, ITEM_HEIGHT);
            usersList->move(usersList->geometry().left() + ITEM_WIDTH / 2, 0);
        }
    }
}

void UsersView::onUserChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    int begin = topLeft.row();
    int end = bottomRight.row();
    for(int i = begin; i <= end; i++) {
        UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(usersList->item(i)));
        QModelIndex index = usersModel->index(i, 0);
        entry->setUserName(index.data(Qt::DisplayRole).toString());
        entry->setFace(index.data(QLightDM::UsersModel::ImagePathRole).toString());
        entry->setLogin(index.data(QLightDM::UsersModel::LoggedInRole).toBool());
    }
}

void UsersView::moveCurrentToCenter(int row)
{

    int center = usersList->indexAt(usersList->viewport()->contentsRect().center()).row();

    if(usersList->count() == 4 || usersList->count() == 3)
        center = 1;
    else if(usersList->count() == 1 || usersList->count() == 2)
        center = 0;

    if(center < 0)
        return ;
    if(row == center)
        return ;
    else if(row > center){
        while (row > center) {
            row -= 1;
            moveUserEntry(0,usersList->count()-1);
        }
    }
    else if(row < center){
        while (row < center) {
            row += 1 ;
            moveUserEntry(usersList->count() - 1 , 0);
        }
    }
    usersList->setCurrentRow(center);
}

void UsersView::setCurrentRow(int row)
{
    if(row < 0 || row >= usersList->count())
        return;
    usersList->setCurrentRow(row);
    moveCurrentToCenter(row);

    UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(usersList->currentItem()));
    entry->setFixedSize(CENTER_ENTRY_WIDTH, CENTER_ENTRY_HEIGHT);
    entry->setSelected();
    for(int i=0;i<usersList->count();i++)
    {
         UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(usersList->item(i)));
         if(i==usersList->currentRow())
         {
             entry->setFixedSize(CENTER_ENTRY_WIDTH, CENTER_ENTRY_HEIGHT);
             entry->setSelected();
         }
         else{
             entry->setFixedSize(ENTRY_WIDTH,ENTRY_HEIGHT);
         }
    }


    int x = 0;
    uid_t uid =  entry->userIndex().data(QLightDM::UsersModel::UidRole).toUInt();
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

