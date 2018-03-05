/* proxymodel.h
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

#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QAbstractListModel>
#include <QWeakPointer>
#include <QStandardItemModel>

/* 扩展QAbstractListModel, 用代理的方式实现在固定的model中增加额外的数据 */

class ProxyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ProxyModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void setSourceModel(QAbstractListModel *sourceModel);
    QStandardItemModel* extraRowModel();

public slots:
    void onSourceDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void onSourceRowsInserted ( const QModelIndex & parent, int start, int end );
    void onSourceRowsRemoved ( const QModelIndex & parent, int start, int end );

    void onExtraDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void onExtraRowsInserted ( const QModelIndex & parent, int start, int end );
    void onExtraRowsRemoved ( const QModelIndex & parent, int start, int end );

private:
    QWeakPointer<QAbstractListModel> m_model;
    QStandardItemModel *m_extraModel;

    int sourceRowCount() const;
};

#endif // PROXYMODEL_H
