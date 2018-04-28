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
#include <QKeyEvent>
#include <QDebug>
#include <QDir>
#include "globalv.h"
#include "greeterwindow.h"
#include "common/configuration.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_screenModel(new ScreenModel(this)),
      m_configuration(Configuration::instance()),
      m_activeScreen(0)
{
    connect(m_screenModel, &ScreenModel::dataChanged, this, &MainWindow::onScreenResized);
    connect(m_screenModel, &ScreenModel::modelReset, this, &MainWindow::onScreenCountChanged);

    //设置窗口大小
    QDesktopWidget *dw = QApplication::desktop();
    setFixedSize(dw->rect().width(), dw->rect().height());
    qDebug() << geometry();

    //设置监控鼠标移动
    setMouseTracking(true);

    //logo
    m_logo = m_configuration->getLogo();

    //背景图片 优先级：用户桌面背景、背景图片、背景颜色
    m_defaultBackgroundPath = m_backgroundPath = IMAGE_DIR + "background.jpg";
//    m_background = scaledPixmap(width(), height(), m_defaultBackgroundPath);

    m_drawUserBackground = m_configuration->getValue("draw-user-background").toBool();
    if(!m_drawUserBackground) {
        m_backgroundPath = m_configuration->getValue("background").toString();
        if(m_backgroundPath == "") {
            m_backgroundColor = m_configuration->getValue("background-color").toString();
            if(m_backgroundColor == "")
                m_backgroundPath = m_defaultBackgroundPath;
        }
    }

    //激活屏幕(即Greeter窗口所在屏幕位置)
    m_greeterWnd = new GreeterWindow(this);
    if(m_drawUserBackground)
        connect(m_greeterWnd, &GreeterWindow::backgroundChanged, this, &MainWindow::onBackgoundChanged);
    moveToScreen(dw->primaryScreen());
    m_greeterWnd->initUI();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    for(int i = 0; i < m_screenModel->rowCount(); i++){
        //在每个屏幕上绘制背景
        QRect rect = m_screenModel->index(i, 0).data(Qt::UserRole).toRect();
        QPainter painter(this);
        if(m_backgroundPath != ""){
            QString resolution = QString("%1x%2").arg(rect.width()).arg(rect.height());
            QPair<QString, QString> key( m_backgroundPath, resolution);
            if(!m_backgrounds.contains(key))
                m_backgrounds[key] = scaledPixmap(width(), height(), m_backgroundPath);
            painter.drawPixmap(rect, m_backgrounds[key]);

        } else {
            painter.fillRect(rect, QColor(m_backgroundColor));
        }
        //绘制logo
        painter.setOpacity(0.5);
        QRect logoRect(rect.left(), rect.bottom()-80, m_logo.width(), m_logo.height());
        painter.drawPixmap(logoRect, m_logo);

        //在没有登录窗口的屏幕上显示图标
        if(i != m_activeScreen)
        {
            QPixmap cof(":/resource/cof.png");
            QRect cofRect(rect.left() + (rect.width()-cof.width())/2,
                          rect.top() + (rect.height()-cof.height())/2,
                          cof.width(), cof.height());
            painter.drawPixmap(cofRect, cof);
        }
    }
    return QWidget::paintEvent(e);
}


/**
 * 根据鼠标指针移动位置移动Greeter窗口所在屏幕
 */
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(m_screenModel->rowCount() > 1){
        QPoint point = e->pos();
        int curScreen = -1;
        for(int i = 0; i< m_screenModel->rowCount(); i++){
            QRect screenRect = m_screenModel->index(i, 0).data(Qt::UserRole).toRect();
            if(screenRect.contains(point)) {
                curScreen = i;
                break;
            }
        }
        if(curScreen != m_activeScreen && curScreen >= 0){
            qDebug() << "active screen: from " << m_activeScreen << "to " << curScreen;
            moveToScreen(curScreen);
        }
    }
    return QWidget::mouseMoveEvent(e);
}


/**
 * 有屏幕分辨率发生改变,移动Greeter窗口位置
 */
void MainWindow::onScreenResized(const QModelIndex &, const QModelIndex &)
{
    QDesktopWidget *dw = QApplication::desktop();
    setFixedSize(dw->width(), dw->height());
    move(0, 0);
    qDebug() << "screen resize to " << dw->geometry();

    moveToScreen(m_activeScreen);
}

/**
 * 有屏幕插拔,移动GreeterWindow到主屏幕
 */
void MainWindow::onScreenCountChanged()
{
    qDebug() << "screen count changed to " << m_screenModel->rowCount();
    QDesktopWidget *dw = QApplication::desktop();
    setFixedSize(dw->width(), dw->height());
    move(0, 0);
    moveToScreen(dw->primaryScreen());
}

/**
 * 移动Greeter窗口到第screen个屏幕上
 */
void MainWindow::moveToScreen(int screen)
{
    m_activeScreen = screen;
    QRect activeScreenRect = m_screenModel->index(m_activeScreen, 0).data(Qt::UserRole).toRect();
    m_greeterWnd->setGeometry(activeScreenRect);
    Q_EMIT activeScreenChanged(activeScreenRect);
}
/**
 * 当前用户选择焦点发生变化时，如果配置了绘制用户背景，则切换背景
 */
void MainWindow::onBackgoundChanged(const QString &bgPath)
{
    QString tmpBgPath = bgPath;

    QFile bgfile(bgPath);
    if(!bgfile.exists())
        tmpBgPath = m_defaultBackgroundPath;

    if(tmpBgPath != m_backgroundPath){
        m_backgroundPath = tmpBgPath;
        repaint();
    }
}
