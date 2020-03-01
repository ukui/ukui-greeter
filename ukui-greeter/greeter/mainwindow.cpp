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
#include <QScreen>
#include <QProcess>
#include <QtMath>
#include <QTimer>
#include "globalv.h"
#include "greeterwindow.h"
#include "common/configuration.h"
#include "common/monitorwatcher.h"
#include "display-switch/displayservice.h"

bool MainWindow::m_first = true;

QT_BEGIN_NAMESPACE
extern void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

#define BLUR_RADIUS 100

QPixmap blurPixmap(QPixmap pixmap)
{
    QPainter painter(&pixmap);
    QImage srcImg = pixmap.toImage();
    qt_blurImage(&painter, srcImg, BLUR_RADIUS, false, false);
    painter.end();
    return pixmap;
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_screenModel(new ScreenModel(this)),
      m_configuration(Configuration::instance()),
      m_activeScreen(nullptr),
      m_monitorWatcher(new MonitorWatcher(this)),
      m_timer(nullptr),
      m_background(nullptr)
{
    QDesktopWidget *_desktop = QApplication::desktop();
    connect(_desktop, &QDesktopWidget::resized, this, &MainWindow::onScreenResized);
    /* QDesktopWidget对显示器的插拔的支持不好 */
    connect(m_monitorWatcher, &MonitorWatcher::monitorCountChanged, this, &MainWindow::onScreenCountChanged);

    //设置窗口大小
    setFixedSize(QApplication::primaryScreen()->virtualSize());
    //设置监控鼠标移动
    setMouseTracking(true);

    //cof
    //m_cof.load(m_configuration->getValue("cof").toString());

    //背景图片 优先级：用户桌面背景、背景图片、背景颜色
    m_defaultBackgroundPath = m_configuration->getDefaultBackgroundName();
    bool drawUserBackground = m_configuration->getValue("draw-user-background").toBool();
    if(drawUserBackground) {
        m_backgroundMode = DRAW_USER_BACKGROUND;
    } else {
        m_background = QSharedPointer<Background>(new Background);
        QString backgroundPath = m_configuration->getValue("background").toString();
        if(!backgroundPath.isEmpty()) {
            m_backgroundMode = DRAW_BACKGROUND;

            m_background->type = BACKGROUND_IMAGE;
            m_background->image = backgroundPath;
        } else {
            QString color = m_configuration->getValue("background-color").toString();
            if(!color.isEmpty()) {
                m_backgroundMode = DRAW_COLOR;

                m_background->type = BACKGROUND_COLOR;
                m_background->color = color;
            } else {
                m_backgroundMode = DRAW_DEFAULT;

                m_background->type = BACKGROUND_IMAGE;
                m_background->image = m_defaultBackgroundPath;
            }
        }
    }
    qDebug() << "default background:" << m_defaultBackgroundPath;
    if(m_background)
        qDebug() << m_background->type << m_background->image;

    m_timer = new QTimer(this);
    m_transition.started = false;

    //激活屏幕(即Greeter窗口所在屏幕位置)
    m_greeterWnd = new GreeterWindow(this);
    moveToScreen(QApplication::primaryScreen());
    m_greeterWnd->initUI();

    m_monitorWatcher->start();

    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTransition);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    for(QScreen *screen : QApplication::screens()){
        //在每个屏幕上绘制背景
        QRect rect = screen->geometry();


        if(m_transition.started)
            drawTransitionAlpha(rect);
        else {
            drawBackground(m_background, rect);
        }

        QPainter painter(this);

        //在没有登录窗口的屏幕上显示图标
//        if(screen != m_activeScreen)
//        {
//            QRect cofRect(rect.left() + (rect.width()-m_cof.width())/2,
//                          rect.top() + (rect.height()-m_cof.height())/2,
//                          m_cof.width(), m_cof.height());
//            painter.drawPixmap(cofRect, m_cof);
//        }
    }
    return QWidget::paintEvent(e);
}


/**
 * 根据鼠标指针移动位置移动Greeter窗口所在屏幕
 */
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(QApplication::screens().count() > 1){
        QPoint point = e->pos();
        QScreen *curScreen = nullptr;
        for(QScreen *screen : QApplication::screens()){
            QRect screenRect = screen->geometry();
            if(screenRect.contains(point)) {
                curScreen = screen;
                break;
            }
        }
        if(curScreen != m_activeScreen && curScreen != nullptr){
            qDebug() << "active screen: from " << m_activeScreen << "to " << curScreen;
            moveToScreen(curScreen);
        }
    }
    return QWidget::mouseMoveEvent(e);
}

/**
 * 有屏幕分辨率发生改变,移动Greeter窗口位置
 */
void MainWindow::onScreenResized()
{
    setGeometry(QApplication::desktop()->geometry());
    qDebug() << "screen resize to " << geometry();

    moveToScreen(QApplication::primaryScreen());
}

/**
 * 有屏幕插拔,移动GreeterWindow到主屏幕
 */
void MainWindow::onScreenCountChanged(int newCount)
{
    if(newCount < 2) {
        QProcess enableMonitors;
        enableMonitors.start("xrandr --auto");
        enableMonitors.waitForFinished(-1);
    } else {
        DisplayService displayService;
        displayService.switchDisplayMode(DISPLAY_MODE_EXTEND);
    }
//    if(m_first){
//        show();
//        activateWindow();
//    }

    move(0, 0);
    setFixedSize(m_monitorWatcher->getVirtualSize());
    moveToScreen(QApplication::primaryScreen());
    repaint();
    qDebug() << "total screen resize to " << geometry();
}

/**
 * 移动Greeter窗口到screen屏幕上
 */
void MainWindow::moveToScreen(QScreen *screen)
{
    m_activeScreen = screen;
    QRect activeScreenRect = m_activeScreen->geometry();

    qDebug() << "moveToScreen activeScreenRect " << activeScreenRect;
    //if(m_monitorWatcher->getMonitorCount() == 1)
    //    activeScreenRect = QRect(QPoint(0, 0), m_monitorWatcher->getVirtualSize());

    m_greeterWnd->setGeometry(activeScreenRect);
    Q_EMIT activeScreenChanged(activeScreenRect);

    repaint();
}

void MainWindow::setBackground(QSharedPointer<Background> &background)
{
    if(background)
        qDebug() << background->image;

    if(m_backgroundMode != DRAW_USER_BACKGROUND)
        return;

    stopTransition();

    if(background) {
        if(background->image.isEmpty())
            background->image = m_defaultBackgroundPath;
    }

    if(m_background && background &&
       m_background->image == background->image){
        repaint();
        return;
    }

    //如果是第一次绘制背景，则不需要渐变
    if(!m_background.isNull())
        startTransition(m_background, background);

    m_background = background;
}

void MainWindow::startTransition(QSharedPointer<Background> &from,
                                 QSharedPointer<Background> &to)
{
    stopTransition();

    m_transition.from = from;
    m_transition.to = to;
    m_transition.stage = 0.0;
    m_transition.started = true;

    m_timer->start(50);
}

void MainWindow::stopTransition()
{
    if(m_timer && m_timer->isActive())
        m_timer->stop();
    m_transition.stage = 1.0;
    m_transition.started = false;
}

void MainWindow::onTransition()
{
    m_transition.stage += 0.05;//= (1 - cos(M_PI * m_transition.stage)) / 2;

    if(m_transition.stage >= 1.0)
        stopTransition();

    repaint();
}

void MainWindow::drawTransitionAlpha(const QRect &rect)
{
    drawBackground(m_transition.from, rect, 1.0 - m_transition.stage);

    drawBackground(m_transition.to, rect, m_transition.stage); 
}

void MainWindow::drawBackground(QSharedPointer<Background> &background,
                                const QRect &rect, float alpha)
{
    if(background == nullptr)
        return;

    QPainter painter(this);
    painter.setOpacity(alpha);

    switch(background->type) {
    case BACKGROUND_IMAGE:
    {
        QPixmap *pixmap = getBackground(background->image, rect);
        if(pixmap->isNull())
        {
            QString color = m_configuration->getValue("background-color").toString();
            QColor cor;
            if(!color.isEmpty())
                cor = color;
            else
                cor = "#035290";
            painter.setBrush(cor);
            painter.drawRect(rect);
        }
	else 
            painter.drawPixmap(rect, *pixmap);

        break;
    }
    case BACKGROUND_COLOR:
    {
        painter.setBrush(background->color);
        painter.drawRect(rect);
        break;
    }
    }
}

QPixmap * MainWindow::getBackground(const QString &path, const QRect &rect)
{
    QString resolution = QString("%1x%2").arg(rect.width()).arg(rect.height());
    QPair<QString, QString> key(path, resolution);

    if(!m_backgrounds.contains(key))
        m_backgrounds[key] = new QPixmap(scaledPixmap(width(), height(), path));

    return m_backgrounds[key];
}
