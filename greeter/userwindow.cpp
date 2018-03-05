/* userwindow.cpp
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
#include "userwindow.h"
#include "usersmodel.h"
#include <QMouseEvent>
#include <QDebug>
#include <QPalette>
#include<QHBoxLayout>
#include "globalv.h"
UserWindow::UserWindow(QWidget *parent)
    : QWidget(parent), m_userList(nullptr)
{
    initUI();
}

void UserWindow::initUI()
{
    int labelTop[] = {20, 26, 34, 42, 50, 60};
    int index = scale*10-5;

    if (this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("this"));
    this->setFixedSize(1100*scale, 300*scale);

    m_userList = new PageListView(this);
    m_userList->setObjectName(QStringLiteral("m_userList"));
    QRect listRect(64*scale, 0, m_userList->width(), m_userList->height());
    m_userList->setGeometry(listRect);
    connect(m_userList, SIGNAL(pageChanged()), this, SLOT(onPageChanged()));
    connect(m_userList, SIGNAL(selectedChanged(QModelIndex)), this, SLOT(onSelectedChanged(QModelIndex)));

    m_prevLabel = new QLabel(this);
    m_prevLabel->setObjectName(QStringLiteral("m_prevLabel"));
    QRect prevRect(0, labelTop[index], 64*scale, 128*scale);
    m_prevLabel->setGeometry(prevRect);
    m_prevLabel->installEventFilter(this);
    m_prevLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/prev.png"));
    m_prevLabel->setStyleSheet("QLabel{background-color: rgba(255, 255, 255, 0.1)}"
                               "QLabel::hover{background-color: rgba(255, 255, 255, 0.2)}");

    m_nextLabel = new QLabel(this);
    m_nextLabel->setObjectName(QStringLiteral("m_nextLabel"));
    QRect nextRect(listRect.right()+10*scale, labelTop[index], 64*scale, 128*scale);
    m_nextLabel->setGeometry(nextRect);
    m_nextLabel->installEventFilter(this);
    m_nextLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/next.png"));
    m_nextLabel->setStyleSheet("QLabel{background-color: rgba(255, 255, 255, 0.1)}"
                               "QLabel::hover{background-color: rgba(255, 255, 255, 0.3)}");
}

void UserWindow::showEvent(QShowEvent *e)
{
    m_userList->setFocus();
    QWidget::showEvent(e);
}

bool UserWindow::eventFilter(QObject *obj, QEvent *event)
{

    if(obj == m_prevLabel)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_userList->pageUp();
                return true;
            }
        }
    }
    if(obj == m_nextLabel)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_userList->pageDown();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserWindow::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if(m_userList){
        m_userList->setModel(model);
    }
}

void UserWindow::onPageChanged()
{
    if(m_userList->hasPrev()) {
        m_prevLabel->show();
    } else {
        m_prevLabel->hide();
    }
    if(m_userList->hasNext()) {
        m_nextLabel->show();
    } else {
        m_nextLabel->hide();
    }
}

void UserWindow::onSelectedChanged(const QModelIndex& index)
{
    emit selectedChanged(index);
}
