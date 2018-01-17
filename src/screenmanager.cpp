#include "screenmanager.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include "greeterwindow.h"
#include "globalv.h"

ScreenManager::ScreenManager(QObject *parent)
    : QObject(parent),
      m_model(new ScreenModel(this)),
      m_activeScreen(0),
      m_greeterWnd(nullptr)
{
    connect(m_model.data(), SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(onScreenChanged(QModelIndex, QModelIndex)));
    connect(m_model.data(), SIGNAL(modelReset()), this, SLOT(onScreenModelReset()));

    QDesktopWidget *dw = QApplication::desktop();
    qDebug() << dw->rect();
    QPalette palette;
    QPixmap background(":/resource/background.png");
    for(int i = 0; i<m_model->rowCount(); i++){
        QRect screenRect = m_model->index(i, 0).data(Qt::UserRole).toRect();
        QWidget *widget = new QWidget();
        widget->setObjectName(QString("widget")+QString::number(i));
//        widget->setMouseTracking(true);
        widget->setGeometry(screenRect);
        widget->setAutoFillBackground(true);
        background = background.scaled(screenRect.width(), screenRect.height(),
                                       Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        palette.setBrush(QPalette::Window, QBrush(background));
        widget->setPalette(palette);
        m_windows.append(widget);
    }
    m_activeScreen = dw->primaryScreen();
    QRect activeRect = m_model->index(m_activeScreen, 0).data(Qt::UserRole).toRect();
    m_greeterWnd = new GreeterWindow(m_windows[m_activeScreen]);
    m_greeterWnd->setGeometry(activeRect);
    m_greeterWnd->initUI();
}

void ScreenManager::onScreenChanged(const QModelIndex &topLeft, const QModelIndex &rightBottom)
{

}

void ScreenManager::onScreenModelReset()
{

}
void ScreenManager::onMouseMove(const QPoint &point)
{
    for(int i = 0; i< m_windows.size(); i++){
        QRect screenRect = m_windows[i]->rect();
        if(point.x() >= screenRect.left() && point.x() <= screenRect.right() &&
           point.y() >= screenRect.top() && point.y() <= screenRect.bottom()) {
//            QDEBUG << "move to screen " << i;
//            m_greeterWnd->setParent(m_windows[i]);
            m_greeterWnd->setGeometry(screenRect);
//            m_greeterWnd->move(screenRect.left(), screenRect.top());
        }
    }
}

void ScreenManager::showWindow()
{
    for(int i = 0; i < m_windows.size(); i++){
        m_windows[i]->show();
    }
}

