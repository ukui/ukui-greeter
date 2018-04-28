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
#include "screenmodel.h"

class GreeterWindow;
class Configuration;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);

signals:
    void activeScreenChanged(const QRect& rect);

public slots:
    void onScreenResized(const QModelIndex&, const QModelIndex&);
    void onScreenCountChanged();
    void moveToScreen(int screen = 0);
    void onBackgoundChanged(const QString &);

private:
    ScreenModel     *m_screenModel;
    GreeterWindow   *m_greeterWnd;
    Configuration   *m_configuration;
    int              m_activeScreen;
    bool             m_drawUserBackground;
    QString          m_defaultBackgroundPath;
    QString          m_backgroundPath;
    QString          m_backgroundColor;
    QPixmap          m_logo;
    //QMap<QPair<m_backgroundPath, resolution>, background>
    //对每张背景图片的不同分辨率进行缓存，减少CPU占用率（这里分辨率格式：1080x960）
    QMap<QPair<QString, QString>, QPixmap>   m_backgrounds;

};

#endif // MAINWINDOW_H
