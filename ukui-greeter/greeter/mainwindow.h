/* mainwindow.h
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include <QAbstractNativeEventFilter>

#include "screenmodel.h"


#include "backgroundwindow.h"
#include <QVector>


class GreeterWindow;
class Configuration;
class MonitorWatcher;
class QTimer;
class MainWindow : public QWidget , public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

signals:
    void activeScreenChanged(const QRect& rect);

private slots:
    void onScreenResized();
    void onScreenCountChanged(int newCount);
    //void onTransition();
    void screenCountEvent();
    void RRScreenChangeEvent();

    void slotBackgroundChanged(QSharedPointer<Background> &);
    void slotAddScreen(QScreen *);
    void slotRemoveScreen(QScreen *);
    //通过追踪background窗体确认当前m_activeScreen
    void onGlobalButtonDrag(int,int);

private:
    void moveToScreen(QScreen *screen = nullptr);
    BackGroundWindow* addBackgroundWindow(QScreen *screen);
private:
    ScreenModel     *m_screenModel;
    GreeterWindow   *m_greeterWnd;
    Configuration   *m_configuration;
    QScreen         *m_activeScreen;

    QScreen *m_curScreen;
    //bool             m_drawUserBackground;
    QString          m_defaultBackgroundPath;
    //QString          m_backgroundPath;
    //QString          m_backgroundColor;
    //QString          m_lastBackgroundPath;
    //QPixmap          m_logo;
    //QPixmap          m_cof;
    //QMap<QPair<m_backgroundPath, resolution>, background>
    //对每张背景图片的不同分辨率进行缓存，减少CPU占用率（这里分辨率格式：1080x960）
    QMap<QPair<QString, QString>, QPixmap*>   m_backgrounds;

    static bool      m_first;
    MonitorWatcher  *m_monitorWatcher;
    QTimer          *m_timer;

//    bool             m_switchBackground;
    QSharedPointer<Background> m_background;
    Transition       m_transition;
//    QPainter        *m_painter;
    BackgroundMode   m_backgroundMode;

    int rr_event_base;
    int rr_error_base;

    int m_monitorCount;

    QMap<QScreen*, BackGroundWindow*> m_backgroundwindows;
};

#endif // MAINWINDOW_H
