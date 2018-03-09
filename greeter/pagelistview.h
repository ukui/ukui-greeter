/* pagelistview.h
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
#ifndef PAGELISTVIEW_H
#define PAGELISTVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QVector>
#include "userentry.h"
#include "usersmodel.h"

class PageListView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int page NOTIFY pageChanged)
public:
    explicit PageListView(QWidget *parent = 0);
    void setModel(QSharedPointer<QAbstractItemModel>);
    void pageUp();      //切换到上一页    留给父窗口通过按钮调用
    void pageDown();    //切换到下一页
    bool hasPrev();
    bool hasNext();

protected:
    void keyReleaseEvent(QKeyEvent *);
    void resizeEvent(QResizeEvent *);

private:
    UserEntry* createEntry(int );
    void drawPage();
    void goHome();      //切换到首页
    void goEnd();       //切换到尾页
    void preItem();     //切换到上一项
    void nextItem();    //切换到下一项
    void moveFocus();   //移动焦点

signals:
    void pageChanged();  //当焦点所在项发生变化时发出
    void selectedChanged(const QModelIndex& index);    //当选定用户时发出

private slots:
    void onEntrySelected(const QString &name);
    void onUserChanged(const QModelIndex&, const QModelIndex&);
    void onUserInserted(const QModelIndex&, int, int);
    void onUserRemoved(const QModelIndex&, int, int);

private:
    QSharedPointer<QAbstractItemModel> m_model;
    QVector<UserEntry*> m_itemList;
    QHBoxLayout *m_layout;

    int m_lastItem;     //上一次选择的项
    int m_curItem;      //当前焦点所在项
    int m_end;          //当前页的最后一项
    int m_lastend;      //上一次的最后一项
    int m_itemCount;
};

#endif // PAGELISTVIEW_H
