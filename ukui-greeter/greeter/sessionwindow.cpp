/* sessionwindow.cpp
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
#include "sessionwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QListWidget>
#include "globalv.h"

#define ICON_SIZE 18
#define PROMPT_LEFT 150
#define LISTWIDGET_SAPCE 3
#define ITEM_WIDTH 300
#define ITEM_HEIGHT 40
#define DISPLAY_ROWS 3  //只显示3行，可以通过上下键移动焦点
#define LISTWIDGET_HEIGHT (DISPLAY_ROWS * (LISTWIDGET_SAPCE * 2 + ITEM_HEIGHT))

SessionWindow::SessionWindow(QAbstractItemModel *model, QWidget *parent)
    : QMenu(parent),
      m_sessionsModel(model)
{
    initUI();
    setSessionModel(model);
    connect(this, &QMenu::triggered,
                this, &SessionWindow::onSessionMenuTrigged);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->installEventFilter(this);
}

void SessionWindow::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("SessionWnd"));

}

void SessionWindow::setSessionModel(QAbstractItemModel *model)
{
    if(model == nullptr) {
        return ;
    }
    m_sessionsModel = model;

    addSessionLabels();
}

void SessionWindow::addSessionLabels()
{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++) {
        QString sessionName = m_sessionsModel->index(i, 0).data().toString();
        QString sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
        QAction *action = new QAction(QIcon(getSessionIcon(sessionKey)), sessionName, this);
        action->setData(sessionKey);
        addAction(action);
        adjustSize();
    }
}

void SessionWindow::setCurrentSession(const QString &session)
{
    for(auto action : this->actions())
    {
        if(action->data().toString()== session)
            this->setActiveAction(action);
    }
}

void SessionWindow::onSessionMenuTrigged(QAction *action)
{
    QString sessionkey  = action->data().toString();
    emit sessionChanged(sessionkey);
}


QString SessionWindow::getSessionIcon(const QString &session)
{
    QString sessionPrefix = session.left(session.indexOf('-'));
    QString sessionIcon = IMAGE_DIR + QString("badges/%1_badge.svg").arg(sessionPrefix.toLower());
    QFile iconFile(sessionIcon);
    if(!iconFile.exists()){
        sessionIcon = IMAGE_DIR + QString("badges/unknown_badge.svg");
    }
    return sessionIcon;
}

bool SessionWindow::eventFilter(QObject *obj, QEvent *event)
{
    //失去焦点时隐藏窗口
    if(event->type() == 23)
    {
        close();
    }
    return false;
}
