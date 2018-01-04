#ifndef EXTRAROWPROXYMODEL_H
#define EXTRAROWPROXYMODEL_H

#include <QAbstractListModel>
#include <QWeakPointer>
#include <QStandardItemModel>

/* 扩展QAbstractListModel, 用代理的方式实现在固定的model中增加额外的数据 */

class ExtraRowProxyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ExtraRowProxyModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

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

#endif // EXTRAROWPROXYMODEL_H
