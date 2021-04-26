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
#include <QWindow>
#include <QX11Info>
#include <QByteArray>
#include <QProcess>
#include <QtMath>
#include <QTimer>
#include "globalv.h"
#include "greeterwindow.h"
#include "common/configuration.h"
#include "common/monitorwatcher.h"
#include "display-switch/displayservice.h"
#include <xcb/xcb.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <unistd.h>
#include "xeventmonitor.h"
bool MainWindow::m_first = true;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_screenModel(new ScreenModel(this)),
      m_configuration(Configuration::instance()),
      m_activeScreen(nullptr),
      m_monitorWatcher(new MonitorWatcher(this)),
      m_monitorCount(0),
      m_timer(nullptr),
      m_background(nullptr)
{
    m_backgroundwindows.clear();
    m_curScreen = nullptr;

    XRRQueryExtension(QX11Info::display(), &rr_event_base, &rr_error_base);
    XRRSelectInput(QX11Info::display(), QX11Info::appRootWindow(), RRScreenChangeNotifyMask);

    QDesktopWidget *_desktop = QApplication::desktop();
    connect(_desktop, &QDesktopWidget::resized, this, &MainWindow::onScreenResized);
    /* QDesktopWidget对显示器的插拔的支持不好 */
 //  connect(m_monitorWatcher, &MonitorWatcher::monitorCountChanged, this, &MainWindow::onScreenCountChanged);
    connect(_desktop, &QDesktopWidget::screenCountChanged, this, &MainWindow::onScreenResized);
    connect(qApp,&QApplication::screenAdded,this,&MainWindow::slotAddScreen);
    connect(qApp,&QApplication::screenRemoved,this,&MainWindow::slotRemoveScreen);
    //设置窗口大小
    QDesktopWidget *desktop = QApplication::desktop();
    qDebug()<<desktop->geometry();
    setGeometry(desktop->geometry());

    //设置监控鼠标移动
    connect(XEventMonitor::instance(), SIGNAL(buttonDrag(int,int)),
            this, SLOT(onGlobalButtonDrag(int,int)));
    //cof
    //m_cof.load(m_configuration->getValue("cof").toString());

    //draw background at different window
    for(QScreen *screen : QApplication::screens()) {
        BackGroundWindow *win = addBackgroundWindow(screen);
        m_backgroundwindows[screen] = win;
        win->show();
    }

    //激活屏幕(即Greeter窗口所在屏幕位置)
    m_greeterWnd = new GreeterWindow();

    connect(m_greeterWnd, SIGNAL(signalBackgroundChanged(QSharedPointer<Background>&)),this,
            SLOT(slotBackgroundChanged(QSharedPointer<Background>&)));
    m_greeterWnd->initUI();
    //根据鼠标的实时位置，确定位置。
    moveToScreen(QApplication::primaryScreen());

    m_greeterWnd->show();

    //m_monitorWatcher->start();
    //connect(m_timer, &QTimer::timeout, this, &MainWindow::onTransition);

    bool numlockState = true;
    numlockState = m_configuration->getLastNumLock();
/*
    if(numlockState){
        //默认打开numlock需要设置两次，否则灯和效果可能不一致，原因不知
        unsigned int num_mask = XkbKeysymToModifiers (QX11Info::display(), XK_Num_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, 0);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, num_mask);
    }else{
        unsigned int num_mask = XkbKeysymToModifiers (QX11Info::display(), XK_Num_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, num_mask);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, 0);
    }
*/
    qApp->installNativeEventFilter(this);
}

void MainWindow::onGlobalButtonDrag(int x, int y)
{
    if(QApplication::screens().count() > 1){
        QPoint point(x,y);
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
}

void MainWindow::RRScreenChangeEvent()
{
    XRRScreenResources *screen;
    screen = XRRGetScreenResources(QX11Info::display(), QX11Info::appRootWindow());
    XRROutputInfo *info;
    int count = 0;

    for (int i = 0; i < screen->noutput; i++) {
        info = XRRGetOutputInfo(QX11Info::display(), screen, screen->outputs[i]);
        if (info->connection == RR_Connected) {
            count++;
        }
        XRRFreeOutputInfo(info);
    }

    onScreenCountChanged(count);
    XRRFreeScreenResources(screen);
}

void MainWindow::slotBackgroundChanged(QSharedPointer<Background> &background)
{
    for(int i =0; i< m_backgroundwindows.size(); i++) {
        m_backgroundwindows.values().at(i)->setBackground(background);
    }
}

void MainWindow::slotAddScreen(QScreen* screen)
{
    if(screen == nullptr)
        return;
    BackGroundWindow *win = addBackgroundWindow(screen);
    QSharedPointer<Background> temp = m_backgroundwindows.values().at(0)->getBackground();
    win->setBackground(temp);
    m_backgroundwindows[screen] = win;
    win->show();
}

void MainWindow::slotRemoveScreen(QScreen *screen)
{
    if(screen == nullptr)
        return;
    BackGroundWindow *win = m_backgroundwindows[screen];
    m_backgroundwindows.remove(screen);
    win->hide();
    win->deleteLater();
    moveToScreen(QApplication::primaryScreen());
}

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (qstrcmp(eventType, "xcb_generic_event_t") != 0) {
        return false;
    }
    xcb_generic_event_t *event = reinterpret_cast<xcb_generic_event_t*>(message);
    const uint8_t responseType = event->response_type & ~0x80;
    if(responseType == rr_event_base + RRScreenChangeNotify){
        RRScreenChangeEvent();
    }
    return false;
}

/**
 * 有屏幕分辨率发生改变,移动Greeter窗口位置
 */
void MainWindow::onScreenResized()
{
    hide();
    //QDesktopWidget *desktop = QApplication::desktop();
    //setGeometry(desktop->geometry());

    qDebug() << "screen resize to " << geometry();

    moveToScreen(QApplication::primaryScreen());
    //show();
}

void MainWindow::screenCountEvent()
{
    //QDesktopWidget *desktop = QApplication::desktop();
    //setGeometry(desktop->geometry());

    moveToScreen(QApplication::primaryScreen());
    //需要重新绘制，否则背景图片大小会不正确
    //repaint();

}

/**
 * 有屏幕插拔,移动GreeterWindow到主屏幕
 */
void MainWindow::onScreenCountChanged(int newCount)
{
	return ;
    if(newCount == m_monitorCount)
        return;
    
    m_monitorCount = newCount;

    if(newCount < 2) {
        QProcess enableMonitors;
        //默认设置显示最大分辨率
        enableMonitors.start("xrandr --auto");
        enableMonitors.waitForFinished(-1);
    } else {
        DisplayService displayService;
        int mode = m_configuration->getValue("display-mode").toInt();
        displayService.switchDisplayMode((DisplayMode)mode);
    }
//    if(m_first){
//        show();
//        activateWindow();
//    }

    //在调用xrandr打开显示器以后，不能马上设置窗口大小，会设置不正确的
    //分辨率，延时500ms正常。
    //QTimer::singleShot(500,this,SLOT(screenCountEvent()));
}

/**
 * 移动Greeter窗口到screen屏幕上
 */
void MainWindow::moveToScreen(QScreen *screen)
{
    m_activeScreen = screen;
    QRect activeScreenRect = m_activeScreen->geometry();

    qDebug() << "moveToScreen activeScreenRect " << activeScreenRect;
    m_greeterWnd->hide();
    m_greeterWnd->show();
    m_greeterWnd->windowHandle()->setScreen(screen);
    m_greeterWnd->setGeometry(activeScreenRect);
    //m_greeterWnd->move(activeScreenRect.topLeft());
  //  m_greeterWnd->move(activeScreenRect.topLeft());
  //  qApp->processEvents();
}

BackGroundWindow *MainWindow::addBackgroundWindow(QScreen *screen)
{
    BackGroundWindow *window = new BackGroundWindow(nullptr);
    window->setScreen(screen);
    //window->setGeometry(screen->geometry());
    //connect(window, SIGNAL(signalWidgetEntered()), this, SLOT(slotEnterBackgroundWindow()));

    return window;
}
