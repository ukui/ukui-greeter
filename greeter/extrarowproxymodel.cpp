/* extrarowproxymodel.cpp
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
#include "extrarowproxymodel.h"
#include <QDebug>
ExtraRowProxyModel::ExtraRowProxyModel(QObject *parent)
    : QAbstractListModel(parent),
      m_extraModel(new QStandardItemModel(this))
{
    connect(m_extraModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(onExtraRowsInserted(const QModelIndex&, int, int)));
    connect(m_extraModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(onExtraRowsRemoved(const QModelIndex&, int, int)));
    connect(m_extraModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(onExtraDataChanged(const QModelIndex&, const QModelIndex&)));

}

QVariant ExtraRowProxyModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < sourceRowCount())
        return m_model.data()->index(index.row(), 0).data(role);
    else
        return m_extraModel->index(index.row() - sourceRowCount(), 0).data(role);
}

int ExtraRowProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return sourceRowCount() + m_extraModel->rowCount();
}

QStandardItemModel* ExtraRowProxyModel::extraRowModel()
{
    return m_extraModel;
}

void ExtraRowProxyModel::setSourceModel(QAbstractListModel *sourceModel)
{
    if(!m_model.isNull())
    {
        disconnect(m_model.data(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(onSourceRowsInserted(const QModelIndex&, int, int)));
        disconnect(m_model.data(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                this, SLOT(onSourceRowsRemoved(const QModelIndex&, int, int)));
        disconnect(m_model.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                   this, SLOT(onSourceDataChanged(const QModelIndex&, const QModelIndex&)));

    }
    m_model = QWeakPointer<QAbstractListModel>(sourceModel);
    reset();
    setRoleNames(m_model.data()->roleNames());

    connect(m_model.data(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(onSourceRowsInserted(const QModelIndex&, int, int)));
    connect(m_model.data(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(onSourceRowsRemoved(const QModelIndex&, int, int)));
    connect(m_model.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
               this, SLOT(onSourceDataChanged(const QModelIndex&, const QModelIndex&)));
}

int ExtraRowProxyModel::sourceRowCount() const
{
    return m_model.isNull() ? 0 : m_model.data()->rowCount(/*QModelIndex()*/);
}

void ExtraRowProxyModel::onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    dataChanged(createIndex(topLeft.row(), 0), createIndex(bottomRight.row(), 0));
}

void ExtraRowProxyModel::onSourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    beginInsertRows(parent, start, end);
    endInsertRows();
}

void ExtraRowProxyModel::onSourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    beginRemoveRows(parent, start, end);
    endRemoveRows();
}

void ExtraRowProxyModel::onExtraDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    dataChanged(createIndex(sourceRowCount() + topLeft.row(), 0), createIndex(sourceRowCount() + bottomRight.row(), 0));
}

void ExtraRowProxyModel::onExtraRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    beginInsertRows(parent, sourceRowCount() + start, sourceRowCount() + end);
    endInsertRows();
}

void ExtraRowProxyModel::onExtraRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    beginRemoveRows(parent, sourceRowCount() + start, sourceRowCount() + end);
    endRemoveRows();
}
