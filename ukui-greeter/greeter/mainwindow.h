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
#include <QtConcurrent/QtConcurrentRun>
#include <QSharedPointer>
#include <QAbstractNativeEventFilter>

#include "screenmodel.h"


enum BackgroundMode
{
    DRAW_USER_BACKGROUND,
    DRAW_BACKGROUND,
    DRAW_COLOR,
    DRAW_DEFAULT
};

enum BackgroundType
{
    BACKGROUND_IMAGE,
    BACKGROUND_COLOR
};

struct Background
{
    BackgroundType type;
    QColor color;
    QString image;
};

struct Transition
{
    QSharedPointer<Background>  from;
    QSharedPointer<Background>  to;
    float       stage;
    bool        started;
};

class GreeterWindow;
class Configuration;
class MonitorWatcher;
class QTimer;
class MainWindow : public QWidget , public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void setBackground(QSharedPointer<Background> &);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

signals:
    void activeScreenChanged(const QRect& rect);

private slots:
    void onScreenResized();
    void onScreenCountChanged(int newCount);
    void onTransition();
    void screenCountEvent();
    void RRScreenChangeEvent();

private:
    void moveToScreen(QScreen *screen = nullptr);
    void startTransition(QSharedPointer<Background> &, QSharedPointer<Background> &);
    void stopTransition();
    void drawTransitionAlpha(const QRect &rect);
    void drawBackground(QSharedPointer<Background> &, const QRect &, float alpha = 1.0);
    QPixmap * getBackground(const QString &path, const QRect &rect);
    void showLater();
private:
    ScreenModel     *m_screenModel;
    GreeterWindow   *m_greeterWnd;
    Configuration   *m_configuration;
    QScreen         *m_activeScreen;
    bool             m_drawUserBackground;
    QString          m_defaultBackgroundPath;
    QString          m_backgroundPath;
    QString          m_backgroundColor;
    QString          m_lastBackgroundPath;
    QPixmap          m_logo;
    QPixmap          m_cof;
    QPixmap         *pixmap;
    //QMap<QPair<m_backgroundPath, resolution>, background>
    //对每张背景图片的不同分辨率进行缓存，减少CPU占用率（这里分辨率格式：1080x960）
    QMap<QPair<QString, QString>, QPixmap*>   m_backgrounds;

    static bool      m_first;
    //MonitorWatcher  *m_monitorWatcher;
    QTimer          *m_timer = nullptr;

//    bool             m_switchBackground;
    QSharedPointer<Background> m_background;
    Transition       m_transition;
//    QPainter        *m_painter;
    BackgroundMode   m_backgroundMode;
    QFuture<void>    future;
    int rr_event_base;
    int rr_error_base;

    int m_monitorCount;
};

#endif // MAINWINDOW_H
