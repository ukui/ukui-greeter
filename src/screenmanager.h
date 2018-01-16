#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include "screenmodel.h"

class ScreenManager : public QObject
{
    Q_OBJECT
public:
    explicit ScreenManager(QObject *parent = 0);

private slots:
    //void onScreenChanged(const QModelIndex&, const QModelIndex&);
    //void onScreenModelReset();

private:
    QSharedPointer<QAbstractListModel> m_model;
};

#endif // SCREENMANAGER_H
