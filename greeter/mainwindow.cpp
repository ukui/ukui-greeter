#include "mainwindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include "globalv.h"
#include "greeterwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_screenModel(new ScreenModel(this)),
      m_greeterWnd(new GreeterWindow(this)),
      m_activeScreen(0)
{
    connect(m_screenModel.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(onScreenResized(QModelIndex, QModelIndex)));
    connect(m_screenModel.data(), SIGNAL(modelReset()), this, SLOT(onScreenCountChanged()));

    QDesktopWidget *dw = QApplication::desktop();
    resize(dw->rect().width(), dw->rect().height());
    setMouseTracking(true);

    m_activeScreen = dw->primaryScreen();
    m_greeterWnd->setGeometry(m_screenModel->index(m_activeScreen, 0).data(Qt::UserRole).toRect());
    m_greeterWnd->initUI();

    m_logo = logoGenerator(getSystemVersion());
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPixmap background(IMAGE_DIR + QString("background.png"));
    QPixmap cof(":/resource/cof.png");
    for(int i = 0; i < m_screenModel->rowCount(); i++){
        //绘制背景
        QRect rect = m_screenModel->index(i, 0).data(Qt::UserRole).toRect();
        background = background.scaled(rect.width(), rect.height(),
                                       Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPainter painter(this);
        painter.setBrush(QBrush(background));
        painter.drawRect(rect);
        //绘制logo
        painter.setOpacity(0.5);
        QRect logoRect(rect.left() + 50, rect.bottom()-80, m_logo.width(), m_logo.height());
        painter.drawPixmap(logoRect, m_logo);

        //在没有登录窗口的屏幕上显示图标
        if(i != m_activeScreen)
            painter.drawPixmap((rect.left()+rect.width()-cof.width())/2,
                               (rect.top()+rect.height()-cof.height())/2,
                               cof.width(), cof.height(), cof);
    }
    return QWidget::paintEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint point = e->pos();
    int curScreen = -1;
    for(int i = 0; i< m_screenModel->rowCount(); i++){
        QRect screenRect = m_screenModel->index(i, 0).data(Qt::UserRole).toRect();
        if(screenRect.contains(point)) {
            curScreen = i;
        }
    } 
    if(curScreen != m_activeScreen && curScreen > 0){
        qDebug() << "active screen: from " << m_activeScreen << "to " << curScreen;
        m_activeScreen = curScreen;
        m_greeterWnd->setGeometry(m_screenModel->index(m_activeScreen, 0).data(Qt::UserRole).toRect());
    }
    return QWidget::mouseMoveEvent(e);
}

void MainWindow::onScreenResized(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{

}

void MainWindow::onScreenCountChanged()
{
    QDesktopWidget *dw = QApplication::desktop();
    resize(dw->width(), dw->height());
}

