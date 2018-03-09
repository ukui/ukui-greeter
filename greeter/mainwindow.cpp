/* mainwindow.cpp
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
#include "mainwindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QDir>
#include "globalv.h"
#include "greeterwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_screenModel(new ScreenModel(this)),
      m_activeScreen(0)
{
    connect(m_screenModel, &ScreenModel::dataChanged, this, &MainWindow::onScreenResized);
    connect(m_screenModel, &ScreenModel::modelReset, this, &MainWindow::onScreenCountChanged);

    //设置窗口大小
    QDesktopWidget *dw = QApplication::desktop();
    resize(dw->rect().width(), dw->rect().height());

    //设置监控鼠标移动
    setMouseTracking(true);

    //获取当前大写键状态
    m_capsLock = getCapsLock();

    //激活屏幕(即Greeter窗口所在屏幕位置)
    m_activeScreen = dw->primaryScreen();
    m_greeterWnd = new GreeterWindow(this);
    m_greeterWnd->setGeometry(m_screenModel->index(m_activeScreen, 0).data(Qt::UserRole).toRect());
    m_greeterWnd->initUI();

    //logo
    m_logo = logoGenerator(getSystemVersion());

    //背景图片
    QDir dir(IMAGE_DIR);
    QStringList names{"background*"};
    QStringList files = dir.entryList(names, QDir::Files);
    if(files.size() > 0)
        m_background.load(IMAGE_DIR + files[0]);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    for(int i = 0; i < m_screenModel->rowCount(); i++){
        //在每个屏幕上绘制背景
        QRect rect = m_screenModel->index(i, 0).data(Qt::UserRole).toRect();
        m_background = m_background.scaled(rect.width(), rect.height(),
                                       Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPainter painter(this);
        painter.setBrush(QBrush(m_background));
        painter.drawRect(rect);
        //绘制logo
        painter.setOpacity(0.5);
        QRect logoRect(rect.left() + 50, rect.bottom()-80, m_logo.width(), m_logo.height());
        painter.drawPixmap(logoRect, m_logo);

        //在没有登录窗口的屏幕上显示图标
        if(i != m_activeScreen)
        {
            QPixmap cof(":/resource/cof.png");
            QRect cofRect((rect.left()+rect.width()-cof.width())/2,
                          (rect.top()+rect.height()-cof.height())/2,
                          cof.width(), cof.height());
            painter.drawPixmap(cofRect, cof);
        }
    }
    return QWidget::paintEvent(e);
}

/**
 * @brief MainWindow::mouseMoveEvent
 * @param e
 * 根据鼠标指针移动位置移动Greeter窗口所在屏幕
 */
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
        moveToScreen(curScreen);
    }
    return QWidget::mouseMoveEvent(e);
}
void MainWindow::keyReleaseEvent(QKeyEvent *)
{
    if(getCapsLock() != m_capsLock){
        m_capsLock = capsLock;
        Q_EMIT capslockChanged(m_capsLock);
    }
}

/**
 * @brief MainWindow::onScreenResized
 * @param topLeft
 * @param bottomRight
 * 有屏幕分辨率发生改变,移动Greeter窗口位置
 */
void MainWindow::onScreenResized(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    QDesktopWidget *dw = QApplication::desktop();
    resize(dw->width(), dw->height());
    update();
    moveToScreen(m_activeScreen);
}

/**
 * @brief MainWindow::onScreenCountChanged
 * 有屏幕插拔,移动GreeterWindow到主屏幕
 */
void MainWindow::onScreenCountChanged()
{
    QDesktopWidget *dw = QApplication::desktop();
    resize(dw->width(), dw->height());
    qDebug() << "screen changed to " << rect();
    update();
    moveToScreen(dw->primaryScreen());
}

/**
 * @brief MainWindow::moveToScreen
 * @param screen
 * 移动Greeter窗口到第screen个屏幕上
 */
void MainWindow::moveToScreen(int screen)
{
    m_activeScreen = screen;
    m_greeterWnd->setGeometry(m_screenModel->index(m_activeScreen, 0).data(Qt::UserRole).toRect());
    //计算缩放比例
//    QRect screen = m_screenModel->index(m_activeScreen, 0).data(Qt::UserRole).toRect();
//    scale = QString::number(screen.width() / 1920.0, 'f', 1).toFloat();
//    scale = scale > 1.0 ? 1.0 : (scale < 0.6 ? 0.6 : scale);
//    qDebug() <<"ScreenSize:" << screen.width() << " "<< screen.height()<< ", scale: "<< scale;
}
