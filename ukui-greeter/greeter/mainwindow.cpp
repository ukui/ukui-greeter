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
#include <QtConcurrent/QtConcurrentRun>
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
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <libintl.h>
#include <unistd.h>
#include "xeventmonitor.h"
bool MainWindow::m_first = true;

QT_BEGIN_NAMESPACE
extern void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

#define BLUR_RADIUS 300

QPixmap * blurPixmap(QPixmap *pixmap)
{
    QPainter painter(pixmap);
    QImage srcImg = pixmap->toImage();
    qt_blurImage(&painter, srcImg, BLUR_RADIUS, false, false);
    painter.end();
    return pixmap;
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_screenModel(new ScreenModel(this)),
      m_configuration(Configuration::instance()),
      m_activeScreen(nullptr),
      m_background(nullptr),
      m_monitorCount(0)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint);
    XRRQueryExtension(QX11Info::display(), &rr_event_base, &rr_error_base);
    XRRSelectInput(QX11Info::display(), QX11Info::appRootWindow(), RRScreenChangeNotifyMask);

    QDesktopWidget *_desktop = QApplication::desktop();
    connect(_desktop, &QDesktopWidget::resized, this, &MainWindow::onScreenResized);
    connect(_desktop, &QDesktopWidget::screenCountChanged, this, &MainWindow::onScreenResized);
    //设置窗口大小
    qDebug()<<_desktop->geometry();
    setGeometry(_desktop->geometry());
    //设置监控鼠标移动
    setMouseTracking(true);
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

    m_transition.started = false;

    m_greeterWnd = new GreeterWindow(this);
    future = QtConcurrent::run([=](){
        QString backgroundname = m_greeterWnd->guessBackground();
        QString resolution = QString("%1x%2").arg(QApplication::primaryScreen()->geometry().width()).arg(QApplication::primaryScreen()->geometry().height());
        QPair<QString, QString> key(backgroundname, resolution);
        if(!m_backgrounds.contains(key)){
            QPixmap *pixmap =  new QPixmap(scaledPixmap(QApplication::primaryScreen()->geometry().width(), QApplication::primaryScreen()->geometry().height(), backgroundname));
            m_backgrounds[key] = blurPixmap(pixmap);
        }
    });

    //登录界面移动到鼠标所在的屏幕上
    if(QApplication::screens().count() > 1){
        QPoint point = QCursor::pos();
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
    }else{
        moveToScreen(QApplication::primaryScreen());
    }

    m_greeterWnd->initUI();

    qApp->installNativeEventFilter(this);

    showLater();

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

void MainWindow::showLater()
{
    if(checkNumLockState()){
        unsigned int num_mask = XkbKeysymToModifiers (QX11Info::display(), XK_Num_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, 0);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, num_mask);
    }else{
        unsigned int num_mask = XkbKeysymToModifiers (QX11Info::display(), XK_Num_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, num_mask);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, num_mask, 0);
    }

    if(checkCapsState()){
        unsigned int caps_mask = XkbKeysymToModifiers (QX11Info::display(), XK_Caps_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, caps_mask, 0);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, caps_mask, caps_mask);
    }else{
        unsigned int caps_mask = XkbKeysymToModifiers (QX11Info::display(), XK_Caps_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, caps_mask, caps_mask);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, caps_mask, 0);
    }

    XDefineCursor(QX11Info::display(), QX11Info::appRootWindow(), XCreateFontCursor(QX11Info::display(), XC_arrow));

    bindtextdomain("Linux-PAM","/usr/share/locale");
    textdomain("Linux-PAM");
    
    XEventMonitor::instance()->start();
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

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
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
    QDesktopWidget *desktop = QApplication::desktop();
    setGeometry(desktop->geometry());

    qDebug() << "screen resize to " << geometry();

    moveToScreen(QApplication::primaryScreen());
    show();
    m_greeterWnd->setRootWindow();
}

void MainWindow::screenCountEvent()
{
	
    QDesktopWidget *desktop = QApplication::desktop();
    setGeometry(desktop->geometry());

    moveToScreen(QApplication::primaryScreen());
    //需要重新绘制，否则背景图片大小会不正确
    repaint();
}

/**
 * 有屏幕插拔,移动GreeterWindow到主屏幕
 */
void MainWindow::onScreenCountChanged(int newCount)
{
    if(newCount == m_monitorCount)
        return;
    
    m_monitorCount = newCount;

    if(newCount < 2) {
        QProcess enableMonitors;
        //默认设置显示最大分辨率
        enableMonitors.start("xrandr --auto");
        enableMonitors.waitForFinished(-1);
	QTimer::singleShot(600, [&]{
	     enableMonitors.start(QString("xrandr --output %1 --auto").arg(QApplication::primaryScreen()->name()));
	     enableMonitors.waitForFinished(-1);
	});
    } else {
        DisplayService displayService;
        int mode = m_configuration->getValue("display-mode").toInt();
        displayService.switchDisplayMode((DisplayMode)mode);
    }

    //在调用xrandr打开显示器以后，不能马上设置窗口大小，会设置不正确的
    //分辨率，延时500ms正常。
    QTimer::singleShot(500,this,SLOT(screenCountEvent()));
}

/**
 * 移动Greeter窗口到screen屏幕上
 */
void MainWindow::moveToScreen(QScreen *screen)
{
    m_activeScreen = screen;
    QRect activeScreenRect = m_activeScreen->geometry();

    qDebug() << "moveToScreen activeScreenRect " << activeScreenRect;
    if(QApplication::desktop()->screenCount() == 1)
        m_greeterWnd->setGeometry(QApplication::primaryScreen()->geometry());
    else
        m_greeterWnd->setGeometry(activeScreenRect);

   // Q_EMIT activeScreenChanged(activeScreenRect);
 
    update();
}

void MainWindow::setBackground(QSharedPointer<Background> &background)
{
    qDebug()<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
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
    //现在由于绘制模糊背景，不需要渐变了
    if(!m_background.isNull()){
        startTransition(m_background, background);
    }

    m_background = background;

}

void MainWindow::startTransition(QSharedPointer<Background> &from,
                                 QSharedPointer<Background> &to)
{
    if(!m_timer){
         m_timer = new QTimer(this);
         connect(m_timer, &QTimer::timeout, this, &MainWindow::onTransition);
    }
    stopTransition();

    m_transition.from = from;
    m_transition.to = to;
    m_transition.stage = 0.0;
    m_transition.started = true;

    m_timer->start(20);
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
    m_transition.stage += 0.1;//= (1 - cos(M_PI * m_transition.stage)) / 2;

    if(m_transition.stage >= 1.0)
        stopTransition();

    //repaint();
    update();
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
        {
            painter.drawPixmap(rect, *pixmap);
        }
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

    if(m_backgrounds.isEmpty()&&future.isRunning()){
            future.waitForFinished();
    }else{
        if(!future.isFinished() && future.isStarted()){
            future.waitForFinished();
        }
    }
    if(!m_backgrounds.contains(key)){
        QPixmap *pixmap =  new QPixmap(scaledPixmap(width(), height(), path));
        m_backgrounds[key] = blurPixmap(pixmap);
    }
    return m_backgrounds[key];
}
