#include "screenmanager.h"
#include "greeterwindow.h"
#include <QDesktopWidget>
#include <QApplication>

ScreenManager::ScreenManager(QObject *parent)
    : QObject(parent), m_model(new ScreenModel(this))
{
    connect(m_model.data(), SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(onScreenChanged(QModelIndex, QModelIndex)));
    connect(m_model.data(), SIGNAL(modelReset()), this, SLOT(onScreenModelReset()));

    for(int i = 0; i<m_model->rowCount(); i++){
        QRect screenRect = m_model->index(i, 0).data(Qt::UserRole).toRect();
        QDesktopWidget *dw = QApplication::desktop();
        if(i == dw->primaryScreen()){
            GreeterWindow *gw = new GreeterWindow();
            gw->setGeometry(screenRect);
        } else {
            QWidget *screenWidget = new QWidget();
            screenWidget->setGeometry(screenRect);
        }
    }
}
