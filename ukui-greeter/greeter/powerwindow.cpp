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
#include <QEvent>
#include <QException>
#include <QDebug>
#include <QDBusInterface>
#include "powerwindow.h"

PowerManager::PowerManager(QWidget *parent)
 : QListWidget(parent),
   m_power(new QLightDM::PowerInterface(this)),
   lasttime(QTime::currentTime())
{
    setFlow(QListWidget::LeftToRight);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QListWidget::NoSelection);

    QObject::connect(this,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(powerClicked(QListWidgetItem*)));
    initUI();
}

QSize PowerManager::windowSize()
{
    return QSize(ITEM_WIDTH*this->count(), ITEM_HEIGHT);
}


void PowerManager::powerClicked(QListWidgetItem *item)
{
    int interval = lasttime.msecsTo(QTime::currentTime());
    if(interval < 200 && interval > -200)
        return ;
    lasttime = QTime::currentTime();
/*
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
        if(m_power->canSuspend())
            suspendWidgetCliced();
        else if(m_power->canHibernate())
            hibernateWidgetClicked();
        break;
    case 4:
        if(m_power->canHibernate())
            hibernateWidgetClicked();
    default:
        break;
    }
*/
    QString name = itemWidget(item)->objectName();
    if(name == switchWidget->objectName())
        switchWidgetClicked();
    else if(name == rebootWidget->objectName())
        rebootWidgetClicked();
    else if(name == shutdownWidget->objectName())
        shutdownWidgetClicked();
    else if(suspendWidget &&  name == suspendWidget->objectName())
        suspendWidgetClicked();
    else if(hibernateWidget && name == hibernateWidget->objectName())
        hibernateWidgetClicked();

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

void PowerManager::suspendWidgetClicked()
{
    try{
        emit switchToUser();
	m_power->suspend();
    }catch(QException &e) {
        qWarning() << e.what();
    }
}

void PowerManager::hibernateWidgetClicked()
{
    try{
        emit switchToUser();
    m_power->hibernate();
    }catch(QException &e) {
        qWarning() << e.what();
    }
}

void PowerManager::refreshTranslate()
{
      switchLabel->setText(tr("Switch User"));
      rebootLabel->setText(tr("Restart"));
      shutdownLabel->setText(tr("Power Off"));
}

void PowerManager::initUI()
{

    switchWidget = new QWidget(this);
    switchWidget->setObjectName("switchWidget");
    QLabel *switchFace = new QLabel(this);
    switchLabel =  new QLabel(this);
    switchFace->setAlignment(Qt::AlignCenter);
    switchLabel->setAlignment(Qt::AlignCenter);
    switchFace->setPixmap(QPixmap(":/images/avatar.png").scaled(58,58));
    switchLabel->setText(tr("Switch User"));
    switchWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    QVBoxLayout *switchlayout = new QVBoxLayout(switchWidget);
    switchlayout->addWidget(switchFace);
    switchlayout->addWidget(switchLabel);

    rebootWidget = new QWidget(this);
    rebootWidget->setObjectName("rebootWidget");
    QLabel *rebootFace = new QLabel(this);
    rebootLabel = new QLabel(this);
    rebootFace->setAlignment(Qt::AlignCenter);
    rebootLabel->setAlignment(Qt::AlignCenter);
    rebootFace->setPixmap(QPixmap(":/images/reboot.png").scaled(58,58));
    rebootLabel->setText(tr("Restart"));
    rebootWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    QVBoxLayout *rebootlayout = new QVBoxLayout(rebootWidget);
    rebootlayout->addWidget(rebootFace);
    rebootlayout->addWidget(rebootLabel);
    
    shutdownWidget = new QWidget(this);
    shutdownWidget->setObjectName("shutdownWidget");
    QLabel *shutdownFace  = new QLabel(this);
    shutdownLabel = new QLabel(this);
    shutdownLabel->setAlignment(Qt::AlignCenter);
    shutdownFace->setAlignment(Qt::AlignCenter);
    shutdownFace->setPixmap(QPixmap(":/images/shutdown.png").scaled(58,58));
    shutdownLabel->setText(tr("Power Off"));
    shutdownWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    QVBoxLayout *shutdownlayout = new QVBoxLayout(shutdownWidget);
    shutdownlayout->addWidget(shutdownFace);
    shutdownlayout->addWidget(shutdownLabel);

    if(m_power->canSuspend()){
        suspendWidget = new QWidget(this);
        suspendWidget->setObjectName("suspendWidget");
    	QLabel *suspendFace = new QLabel(this);
    	QLabel *suspendLabel = new QLabel(this);
    	suspendFace->setAlignment(Qt::AlignCenter);
    	suspendLabel->setAlignment(Qt::AlignCenter);
        suspendFace->setPixmap(QPixmap(":/images/suspend.png").scaled(48,48));
        suspendLabel->setText(tr("Suspend"));
    	suspendWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
    	QVBoxLayout *suspendlayout = new QVBoxLayout(suspendWidget);
    	suspendlayout->addWidget(suspendFace);
    	suspendlayout->addWidget(suspendLabel);
    }

    if(m_power->canHibernate()) {
        hibernateWidget = new QWidget(this);
        hibernateWidget->setObjectName("hibernateWidget");
        QLabel *hibernateFace = new QLabel(this);
        QLabel *hibernateLabel = new QLabel(this);
        hibernateFace->setAlignment(Qt::AlignCenter);
        hibernateLabel->setAlignment(Qt::AlignCenter);
        hibernateFace->setPixmap(QPixmap(":/images/hibernate.png").scaled(48,48));
        hibernateLabel->setText(tr("Sleep"));
        hibernateWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
        QVBoxLayout *hibernatelayout = new QVBoxLayout(hibernateWidget);
        hibernatelayout->addWidget(hibernateFace);
        hibernatelayout->addWidget(hibernateLabel);
    }
   
    QListWidgetItem *item0 = new QListWidgetItem();
    item0->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(this->count(), item0);
    setItemWidget(item0, switchWidget);
 
    if(m_power->canHibernate()){
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        insertItem(this->count(), item);
        setItemWidget(item,hibernateWidget);
    }

    if(m_power->canSuspend()){
        QListWidgetItem *item3 = new QListWidgetItem();
        item3->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        insertItem(this->count(), item3);
        setItemWidget(item3, suspendWidget);
    }

    QListWidgetItem *item1 = new QListWidgetItem();
    item1->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(this->count(), item1);
    setItemWidget(item1, rebootWidget);

    QListWidgetItem *item2 = new QListWidgetItem();
    item2->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(this->count(), item2);
    setItemWidget(item2, shutdownWidget);

    adjustSize();

}
