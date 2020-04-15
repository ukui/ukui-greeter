/* usersview.h
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
#ifndef USERSVIEW_H
#define USERSVIEW_H

#include <QWidget>
#include <QTime>
#include <QPair>
#include "userentry.h"

class QListWidget;
class QPushButton;
class QAbstractListModel;
class UsersView : public QWidget
{
    Q_OBJECT

public:
    explicit UsersView(QWidget *parent = 0);
    ~UsersView();
    void initUI();
    void setModel(QAbstractListModel *);
    void setCurrentUser(const QString &userName, bool entered = false);
    void pageUp();
    void pageDown();

protected:
  //  void keyReleaseEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void onUserAdded(const QModelIndex&, int, int);
    void onUserRemoved(const QModelIndex&, int, int );
    void onUserChanged(const QModelIndex&, const QModelIndex&);
    void setCurrentRow(int row);
    void onGlobalKeyRelease(const QString &key);

signals:
    void userSelected(const QModelIndex&);
    void currentUserChanged(const QModelIndex&);
    void userNotFound(const QString &userName);

private:
    void insertUserEntry(int row);
    void removeUserEntry(int row);
    void moveUserEntry(int from,int to);
    void moveCurrentToCenter(int row);
    void leftToRight();
    void leftToLeft();
    void rightToRight();
    void rightToLeft();
    void centerToleft();
    void centerToRight();
    void moveAnimation(UserEntry *entry,QRect preRect,QRect nextRect);
    /*左方向建按下，isChoosed代表下个用户是否为选中用户，如果不是，则不触发用户改变
        信号，只进行头像的动画效果。
    */
    void leftKeyPressed(bool isChoosed);
    void rightKeyPressed(bool isChoosed);

private:
    QAbstractListModel *usersModel;
    int currentUser;
    /*qpair记录用户的上一个用户和下一用户的在list的位置*/
    QList<QPair<UserEntry *, QPair<int ,int>>> userlist;
    QTime lasttime;
    QTime mouseClickLast;
    QPushButton *prevArrow;
    QPushButton *nextArrow;
    int leftCount;
    int rightCount;

};

#endif // USERSVIEW_H
