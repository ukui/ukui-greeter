/* pagelistview.cpp
 * Copyright (C) 2018 yanghao <yanghao@kylinos.cn>
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
#include "pagelistview.h"
#include <QPainter>
#include <qmath.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include <QSettings>
#include <QLightDM/UsersModel>
#include "globalv.h"
#include "userentry.h"

PageListView::PageListView(QWidget *parent)
    : QWidget(parent),
      m_layout(new QHBoxLayout(this)),
      m_lastItem(-1)
{
    setFixedSize(950*scale, 300*scale);
//    setFocus();
}

void PageListView::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Return:
        {
            UserEntry *entry = m_itemList[m_curItem];
            onEntrySelected(entry->userName());
            break;
        }
        case Qt::Key_PageUp:
            pageUp();
            break;
        case Qt::Key_PageDown:
            pageDown();
            break;
        case Qt::Key_Left:
            preItem();
            break;
        case Qt::Key_Right:
            nextItem();
            break;
        case Qt::Key_Home:
            goHome();
            break;
        case Qt::Key_End:
            goEnd();
            break;
        default:
            QWidget::keyReleaseEvent(event);
    }
}

void PageListView::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull())
        return;
    if(!m_model.isNull())
    {

    }
    m_model = model;
    connect(m_model.data(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onUserInserted(QModelIndex,int,int)));
    connect(m_model.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(onUserRemoved(QModelIndex,int,int)));
    connect(m_model.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onUserChanged(QModelIndex,QModelIndex)));
    m_itemList = QVector<UserEntry*>(m_model.data()->rowCount(), NULL);

    m_lastend = -1;
    m_itemCount = m_model.data()->rowCount();
    m_end = m_itemCount >= 5 ? 4 : m_itemCount - 1; //每页最多显示5个
    /* 读取上一次登录的用户 */
    QSettings config(configFile, QSettings::IniFormat);
    QString lastUser = config.value("lastLoginUser").toString();
    qDebug() << "lastLoginUser: " << lastUser;
    bool find = false;
    for(int i = 0; i < m_model->rowCount(); i++) {
        QString user = m_model->index(i, 0).data().toString();
        if(lastUser == user) {
            m_curItem = i;
            find = true;
            break;
        }
    }
    if(!find) {
        m_curItem = 0;
    }

    drawPage();
}

void PageListView::drawPage()
{
    int begin = m_end - (m_itemCount > 5 ? 4 : m_itemCount - 1);

    if(m_lastend > 0)
    {
        int lastbegin = m_lastend - (m_itemCount > 5 ? 4 : m_itemCount - 1);
        for(int i = lastbegin; i <= m_lastend; i++)
        {
            m_layout->removeWidget(m_itemList[i]);
            m_itemList[i]->hide();
        }
    }
    if(m_itemCount < 5)
        m_layout->addStretch();

    for(int i = begin; i <= m_end; i++)
    {
        //如果entry不存在则创建，添加到layout中
        if(m_itemList[i] == NULL) {
            m_itemList[i] = createEntry(i);
        } else if(m_itemList[i]->userName() != m_model->index(i, 0).data().toString()) {
            m_itemList.insert(i, createEntry(i));
        }
        m_itemList[i]->show();
        m_layout->addWidget(m_itemList[i], 0);
    }
    if(m_itemCount < 5)
        m_layout->addStretch();

    if(m_itemList.size() > 0) {
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
    }
    emit pageChanged();
}

UserEntry* PageListView::createEntry(int i)
{
    QModelIndex index = m_model->index(i, 0);
    UserEntry *entry = new UserEntry(index.data(Qt::DisplayRole).toString(),
                                     index.data(QLightDM::UsersModel::ImagePathRole).toString(),
                                     index.data(QLightDM::UsersModel::LoggedInRole).toBool(), this);
    connect(entry, SIGNAL(clicked(QString)), this, SLOT(onEntrySelected(QString)));
    return entry;
}

void PageListView::pageUp()
{
    if(m_itemCount <= 5 || m_end == 4)  //位于首页
    {
        m_curItem = 0;
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
        return;
    }
    m_lastend = m_end;
    m_end -= (m_end-5 >= 4 ? 5 : (m_end-4));
    m_curItem = m_end - 4;
    drawPage();
}

void PageListView::pageDown()
{
    if(m_itemCount <= 5 || m_end == m_itemCount - 1)    //位于尾页
    {
        m_curItem = m_end;
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
        return;
    }
    m_lastend = m_end;
    m_end += (m_itemCount-1-m_end >= 5 ? 5 : (m_itemCount-1-m_end));
    m_curItem = m_end;
    drawPage();
}

void PageListView::goHome()
{
    if(m_itemCount <= 5 || m_end == 4)
    {
        m_curItem = 0;
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
        return;
    }
    m_lastend = m_end;
    m_end = 4;
    m_curItem = 0;
    drawPage();
}

void PageListView::goEnd()
{
    if(m_itemCount <= 5 || m_end == m_itemCount - 1)    //位于尾页
    {
        m_curItem = m_end;
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
        return;
    }
    m_lastend = m_end;
    m_end = m_itemCount - 1;
    m_curItem = m_end;
    drawPage();
}

void PageListView::preItem()
{
    int begin = m_itemCount > 5 ? m_end - 4 : 0;
    if(m_curItem - begin > 0)
    {
        --m_curItem;
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
    }
    else if(m_curItem > 0)
    {
        --m_curItem;
        m_lastend = m_end;
        --m_end;
        drawPage();
    }
}

void PageListView::nextItem()
{
    if(m_curItem < m_end)
    {
        ++m_curItem;
//        m_itemList[m_curItem]->setFocus();
        moveFocus();
    }
    else if(m_curItem < m_itemCount - 1)
    {
        ++m_curItem;
        m_lastend = m_end;
        ++m_end;
        drawPage();
    }
}

void PageListView::onEntrySelected(const QString &name)
{
    for(int i = 0; i < m_itemList.size(); i++){
        if(m_itemList[i]!=NULL && m_itemList[i]->userName() == name){
            m_curItem = i;
        }
    }
    moveFocus();
    emit selectedChanged(m_model.data()->index(m_curItem, 0));
}

bool PageListView::hasPrev()
{
    if(m_end > 4)
        return true;
    else
        return false;
}

bool PageListView::hasNext()
{
    if(m_end < m_itemCount -1)
        return true;
    else
        return false;
}

void PageListView::onUserInserted(const QModelIndex& parent, int begin, int end)
{
    Q_UNUSED(parent)
    for(int i = begin; i <= end; i++)
    {
        qDebug() << "User added:" << m_model->index(i, 0).data().toString();
    }

    m_itemCount = m_model->rowCount();
    m_curItem = begin;
    if(m_end >= begin) {
        m_lastend = m_end;
        drawPage();
    } else {
        pageDown();
    }
}

void PageListView::onUserRemoved(const QModelIndex & parent, int begin, int end)
{
    Q_UNUSED(parent)

    for(int i = begin; i<=end; i++) {
        if(m_itemList[i])
            delete(m_itemList[i]);
        m_itemList.remove(i);
    }
    if(m_end == m_itemCount-1) {
        m_end -= (end-begin+1);
        m_lastend = m_end;
    }
    if(m_curItem >= m_end) {
        m_curItem = m_end;
    }
    m_itemCount = m_model->rowCount();
    drawPage();
}

void PageListView::onUserChanged(const QModelIndex & topLeft,
                                 const QModelIndex& bottomRight)
{
    int begin = topLeft.row();
    int end = bottomRight.row();
    for(int i = begin; i <= end; i++) {
        if(m_itemList[i]) {
            QModelIndex index = m_model->index(i, 0);
            m_itemList[i]->setUserName(index.data().toString());
            m_itemList[i]->setFace(index.data(QLightDM::UsersModel::ImagePathRole).toString());
            m_itemList[i]->setLogin(index.data(QLightDM::UsersModel::LoggedInRole).toBool());
        }
    }
}

void PageListView::moveFocus()
{
    if(m_lastItem >= 0 && m_lastItem < m_itemCount && m_itemList[m_lastItem] != NULL)
        m_itemList[m_lastItem]->setSelected(false);
    m_itemList[m_curItem]->setSelected();
    m_lastItem = m_curItem;
}
