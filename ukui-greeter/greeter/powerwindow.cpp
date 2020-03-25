/* powerwindow.cpp
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

#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QListWidgetItem>
#include <QListWidget>
#include <QException>
#include <QDebug>
#include <QDBusInterface>
#include "powerwindow.h"

PowerManager::PowerManager(QWidget *parent)
 : QListWidget(parent),
   m_power(new QLightDM::PowerInterface(this)),
   lasttime(QTime::currentTime())
{
    if(m_power->canSuspend())
        resize(ITEM_WIDTH*4, ITEM_HEIGHT);
    else
        resize(ITEM_WIDTH*3, ITEM_HEIGHT);

    setFlow(QListWidget::LeftToRight);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QListWidget::NoSelection);

    QObject::connect(this,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(powerClicked(QListWidgetItem*)));
    initUI();

}

QSize PowerManager::windowSize()
{
    if(m_power->canSuspend())
        return QSize(ITEM_WIDTH*4, ITEM_HEIGHT);
    else
        return QSize(ITEM_WIDTH*3, ITEM_HEIGHT);
}

void PowerManager::powerClicked(QListWidgetItem *item)
{
    int interval = lasttime.msecsTo(QTime::currentTime());
    if(interval < 200 && interval > -200)
        return ;
    lasttime = QTime::currentTime();

    int x = row(item);

    switch (x) {
    case 0:
        switchWidgetClicked();
        break;
    case 1:
        rebootWidgetClicked();
        break;
    case 2:
        shutdownWidgetClicked();
        break;
    case 3:
        suspendWidgetCliced();
        break;
    default:
        break;
    }
}

void PowerManager::switchWidgetClicked()
{
    emit switchToUser();
}

void PowerManager::shutdownWidgetClicked()
{
    try{
        qDebug() << "shutdown";
        m_power->shutdown();
        close();
    }catch(QException &e) {
        qWarning() << e.what();
    }
}

void PowerManager::rebootWidgetClicked()
{
    try{
        m_power->restart();
        close();
    }catch(QException &e) {
        qWarning() << e.what();
    }
}

void PowerManager::suspendWidgetCliced()
{
    try{
        emit switchToUser();
	m_power->suspend();
    }catch(QException &e) {
        qWarning() << e.what();
    }
}

void PowerManager::initUI()
{

    switchWidget = new QWidget(this);
    switchWidget->setObjectName("switchWidget");
    QLabel *switchFace = new QLabel(this);
    QLabel *switchLabel =  new QLabel(this);
    switchFace->setAlignment(Qt::AlignCenter);
    switchLabel->setAlignment(Qt::AlignCenter);
    switchFace->setPixmap(QPixmap(":/resource/avatar.png").scaled(58,58));
    switchLabel->setText(tr("SwitchUser"));
    switchWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    QVBoxLayout *switchlayout = new QVBoxLayout(switchWidget);
    switchlayout->addWidget(switchFace);
    switchlayout->addWidget(switchLabel);

    suspendWidget = new QWidget(this);
    suspendWidget->setObjectName("suspendWidget");
    QLabel *suspendFace = new QLabel(this);
    QLabel *suspendLabel = new QLabel(this);
    suspendFace->setAlignment(Qt::AlignCenter);
    suspendLabel->setAlignment(Qt::AlignCenter);
    suspendFace->setPixmap(QPixmap(":/resource/suspend.png").scaled(58,58));
    suspendLabel->setText(tr("Suspend"));
    suspendWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    QVBoxLayout *suspendlayout = new QVBoxLayout(suspendWidget);
    suspendlayout->addWidget(suspendFace);
    suspendlayout->addWidget(suspendLabel);


    rebootWidget = new QWidget(this);
    rebootWidget->setObjectName("rebootWidget");
    QLabel *rebootFace = new QLabel(this);
    QLabel *rebootLabel = new QLabel(this);
    rebootFace->setAlignment(Qt::AlignCenter);
    rebootLabel->setAlignment(Qt::AlignCenter);
    rebootFace->setPixmap(QPixmap(":/resource/reboot.png").scaled(58,58));
    rebootLabel->setText(tr("Restart"));
    rebootWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    QVBoxLayout *rebootlayout = new QVBoxLayout(rebootWidget);
    rebootlayout->addWidget(rebootFace);
    rebootlayout->addWidget(rebootLabel);

    if(m_power->canSuspend()){
        shutdownWidget = new QWidget(this);
        shutdownWidget->setObjectName("shutdownWidget");
        QLabel *shutdownFace  = new QLabel(this);
        QLabel *shutdownLabel = new QLabel(this);
        shutdownLabel->setAlignment(Qt::AlignCenter);
        shutdownFace->setAlignment(Qt::AlignCenter);
        shutdownFace->setPixmap(QPixmap(":/resource/shutdown.png").scaled(58,58));
        shutdownLabel->setText(tr("Shutdown"));
        shutdownWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
        QVBoxLayout *shutdownlayout = new QVBoxLayout(shutdownWidget);
        shutdownlayout->addWidget(shutdownFace);
        shutdownlayout->addWidget(shutdownLabel);
    }

    QListWidgetItem *item0 = new QListWidgetItem();
    item0->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(0, item0);
    setItemWidget(item0, switchWidget);

    QListWidgetItem *item1 = new QListWidgetItem();
    item1->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(1, item1);
    setItemWidget(item1, rebootWidget);

    QListWidgetItem *item2 = new QListWidgetItem();
    item2->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(2, item2);
    setItemWidget(item2, shutdownWidget);

    if(m_power->canSuspend()){
        QListWidgetItem *item3 = new QListWidgetItem();
        item3->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        insertItem(3, item3);
        setItemWidget(item3, suspendWidget);
    }

    adjustSize();

}
