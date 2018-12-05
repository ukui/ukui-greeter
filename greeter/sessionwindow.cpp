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

#define ICON_SIZE 22
#define PROMPT_LEFT 150
#define LISTWIDGET_SAPCE 10
#define ITEM_WIDTH 300
#define ITEM_HEIGHT 40
#define DISPLAY_ROWS 3  //只显示3行，可以通过上下键移动焦点
#define LISTWIDGET_HEIGHT (DISPLAY_ROWS * (LISTWIDGET_SAPCE * 2 + ITEM_HEIGHT))

class IconLabel : public QWidget
{
public:
    IconLabel(int width, int height, QWidget* parent = 0)
        : QWidget(parent),
          m_iconLabel(new QLabel(this)),
          m_textLabel(new QLabel(this))
    {
        m_iconLabel->setGeometry(3, (height-ICON_SIZE)/2, ICON_SIZE, ICON_SIZE);
        m_textLabel->setGeometry(height+5, 0, width-height-5, height);
    }

    void setIcon(const QString& icon)
    {
        m_iconLabel->setPixmap(scaledPixmap(ICON_SIZE, ICON_SIZE, icon));
    }

    void setText(const QString& text)
    {
        m_textLabel->setText(text);
        m_textLabel->setFont(QFont("Ubutnu", 14));
        m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

private:
    QLabel  *m_iconLabel;
    QLabel  *m_textLabel;
};



SessionWindow::SessionWindow(QAbstractItemModel *model, QWidget *parent)
    : BorderDialog(parent),
      m_sessionsModel(model)
{
    initUI();
    setSessionModel(model);
}

void SessionWindow::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("SessionWnd"));

    setDialogSize(600, 400);

    m_prompt = new QLabel(center());
    m_prompt->setObjectName(QStringLiteral("lblSessionPrompt"));
    m_prompt->setGeometry(0, 10, center()->width(), 40);
    m_prompt->setAlignment(Qt::AlignCenter);
    m_prompt->setText(tr("select the desktop environment"));

    m_sessionsList = new QListWidget(center());
    m_sessionsList->setObjectName(QStringLiteral("sessionsList"));
    m_sessionsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_sessionsList->setSpacing(LISTWIDGET_SAPCE);
    m_sessionsList->setGeometry(m_prompt->x() + 10, m_prompt->y() + m_prompt->height() + 20,
                                center()->width() - 20, center()->height() - m_prompt->height() - 40);
    connect(m_sessionsList, &QListWidget::itemDoubleClicked,
            this, &SessionWindow::onListCurrentItemDoubleClicked);
}

void SessionWindow::onListCurrentItemDoubleClicked(QListWidgetItem */*item*/)
{
    close();
}

void SessionWindow::setSessionModel(QAbstractItemModel *model)
{
    if(model == nullptr) {
        return ;
    }
    m_sessionsModel = model;

    addSessionLabels();

#ifdef TEST
    testAddSessionLabels();
#endif
}

void SessionWindow::addSessionLabels()
{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++) {
        QString sessionName = m_sessionsModel->index(i, 0).data().toString();
        QString sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
        IconLabel *sessionLabel = new IconLabel(ITEM_WIDTH, ITEM_HEIGHT, this);
        sessionLabel->setIcon(getSessionIcon(sessionKey));
        if(sessionKey == m_defaultSession)
            sessionLabel->setText(sessionName + tr(" (Default)"));
        else
            sessionLabel->setText(sessionName);

        QListWidgetItem *newItem = new QListWidgetItem();
        newItem->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        m_sessionsList->insertItem(i,newItem);
        m_sessionsList->setItemWidget(newItem, sessionLabel);
    }
}


void SessionWindow::showEvent(QShowEvent *event)
{
    m_sessionsList->setFocus();
    QWidget::showEvent(event);
}

void SessionWindow::close()
{
    int currentRow = m_sessionsList->currentRow();
    //传递给loginWindow的是session的唯一标识
    emit sessionChanged(m_sessionsModel->index(currentRow, 0).data(Qt::UserRole).toString());

    BorderDialog::close();
}

void SessionWindow::setCurrentSession(const QString &session)
{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++){
        QString sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
        if(sessionKey == session){
            m_sessionsList->setCurrentRow(i);
            return;
        }
    }
}

QString SessionWindow::getSessionIcon(const QString &session)
{
    QString sessionPrefix = session.left(session.indexOf('-'));
    QString sessionIcon = IMAGE_DIR + QString("badges/%1_badge.png").arg(sessionPrefix.toLower());
    QFile iconFile(sessionIcon);
    if(!iconFile.exists()){
        sessionIcon = IMAGE_DIR + QString("badges/unknown_badge.png");
    }
    return sessionIcon;
}

/**
 * @brief SessionWindow::testAddSessionLabels
 * 测试有多个session的情况
 */
void SessionWindow::testAddSessionLabels()
{
    QStringList sessionList{"MATE", "Ubuntu", "Xfce", "LUbuntu", "XUbuntu", "GNOME", "KDE", "Pantheon"};
    for(int i = 0; i < sessionList.size(); i++) {
        QString sessionName = sessionList[i];
        IconLabel *sessionLabel = new IconLabel(ITEM_WIDTH, ITEM_HEIGHT, this);
        sessionLabel->setIcon(getSessionIcon(sessionName));
        sessionLabel->setText(sessionName);
        QListWidgetItem *newItem = new QListWidgetItem();
        newItem->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        m_sessionsList->insertItem(i,newItem);
        m_sessionsList->setItemWidget(newItem, sessionLabel);
    }
}
