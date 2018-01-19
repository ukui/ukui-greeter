#include "mainwindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include "globalv.h"
#include "greeterwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_model(new ScreenModel(this)),
      m_greeterWnd(new GreeterWindow(this)),
      m_activeScreen(0)
{
    QDesktopWidget *dw = QApplication::desktop();
    setFixedSize(dw->rect().width(), dw->rect().height());
    setMouseTracking(true);

    m_activeScreen = dw->primaryScreen();
    m_greeterWnd->setGeometry(m_model->index(m_activeScreen, 0).data(Qt::UserRole).toRect());
    m_greeterWnd->initUI();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPixmap background(":/resource/background.png");
    QPixmap cof(":/resource/cof.png");
    for(int i = 0; i < m_model->rowCount(); i++){
        //绘制背景
        QRect rect = m_model->index(i, 0).data(Qt::UserRole).toRect();
        background = background.scaled(rect.width(), rect.height(),
                                       Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPainter painter(this);
        painter.setBrush(QBrush(background));
        painter.drawRect(rect);
        if(i != m_activeScreen)
            painter.drawPixmap(rect.left()+rect.width()/2-33, rect.top()+rect.height()/2-33, 66, 66, cof);
    }
    return QWidget::paintEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint point = e->pos();
    int curScreen = -1;
    for(int i = 0; i< m_model->rowCount(); i++){
        QRect screenRect = m_model->index(i, 0).data(Qt::UserRole).toRect();
        if(isPointInRect(point, screenRect)) {
            curScreen = i;
        }
    } 
    if(curScreen != m_activeScreen && curScreen > 0){
        qDebug() << "active screen: from " << m_activeScreen << "to " << curScreen;
        m_activeScreen = curScreen;
        m_greeterWnd->setGeometry(m_model->index(m_activeScreen, 0).data(Qt::UserRole).toRect());
    }
    return QWidget::mouseMoveEvent(e);
}

