/* powerwindow.h
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

#ifndef POWERMANAGER_H
#define POWERMANAGER_H
#include <QWidget>
#include <QListWidget>
#include <QLightDM/Power>
#include <QSize>
#include <QTime>

#define ITEM_WIDTH 168
#define ITEM_HEIGHT ITEM_WIDTH
#define ITEM_SPACING (ITEM_WIDTH/8)

class QListWidget;
class QListWidgetItem;
class PowerManager:public QListWidget
{
    Q_OBJECT

public:
    PowerManager(QWidget *parent = 0);
    QSize windowSize();

private:
    void initUI();
    QListWidget *list;
    QWidget *lockWidget;
    QWidget *switchWidget;
    QWidget *suspendWidget;
    QWidget *rebootWidget;
    QWidget *shutdownWidget;
    QLightDM::PowerInterface *m_power;
    QTime lasttime;

private:
    void lockWidgetClicked();
    void switchWidgetClicked();
    void suspendWidgetCliced();
    void rebootWidgetClicked();
    void shutdownWidgetClicked();

private Q_SLOTS:
    void powerClicked(QListWidgetItem *item);

Q_SIGNALS:
    void switchToUser();
    void lock();
};

#endif // POWERMANAGER_H
