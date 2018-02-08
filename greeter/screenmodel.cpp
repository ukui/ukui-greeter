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

