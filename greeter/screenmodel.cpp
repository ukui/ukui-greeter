/* screenmodel.cpp
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
#include "screenmodel.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
ScreenModel::ScreenModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadScreens();
    QDesktopWidget *dw = QApplication::desktop();
    connect(dw, SIGNAL(resized(int)), this, SLOT(onScreenResized(int)));
    connect(dw, SIGNAL(screenCountChanged(int)), this, SLOT(onScreenCountChanged(int)));
}

int ScreenModel::rowCount(const QModelIndex &parent) const
{
    if(parent == QModelIndex()){
        return m_screen.size();
    }
    return 0;
}

QVariant ScreenModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    int row = index.row();
    if(row < m_screen.size() && role == Qt::UserRole){
        return m_screen[row];
    } else {
        return QVariant();
    }
}

void ScreenModel::onScreenResized(int screen)
{
    QDesktopWidget *dw = QApplication::desktop();
    if(screen > 0 && screen < m_screen.size()){
        m_screen[screen] = dw->screenGeometry(screen);
    }
    QModelIndex index = createIndex(screen, 0);
    emit dataChanged(index, index);
}

void ScreenModel::onScreenCountChanged(int newCount)
{
    Q_UNUSED(newCount)
    loadScreens();
}

void ScreenModel::loadScreens()
{
    beginResetModel();
    m_screen.clear();
    QDesktopWidget *desktopWidget = QApplication::desktop();
    for(int i = 0; i < desktopWidget->screenCount(); i++){
        m_screen.append(desktopWidget->screenGeometry(i));
    }
    endResetModel();
}

