/* screenmodel.cpp
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
#include "screenmodel.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
ScreenModel::ScreenModel(QObject *parent)
    : QAbstractListModel(parent),
      _desktop(QApplication::desktop())
{
    loadScreens();
    connect(_desktop, &QDesktopWidget::workAreaResized, this, &ScreenModel::onScreenResized);
    connect(_desktop, &QDesktopWidget::resized, this, &ScreenModel::onScreenResized);
    connect(_desktop, &QDesktopWidget::screenCountChanged, this, &ScreenModel::onScreenCountChanged);
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
    qDebug() << "screen " << screen << " resized to" << _desktop->screenGeometry(screen);
    if(screen >= 0 && screen < m_screen.size()){
        m_screen[screen] = _desktop->screenGeometry(screen);
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
    for(int i = 0; i < _desktop->screenCount(); i++){
        m_screen.append(_desktop->screenGeometry(i));
    }
    endResetModel();
}

