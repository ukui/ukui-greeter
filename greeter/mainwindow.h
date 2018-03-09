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
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyReleaseEvent(QKeyEvent *);

signals:
    void capslockChanged(bool);

public slots:
    void onScreenResized(const QModelIndex&, const QModelIndex&);
    void onScreenCountChanged();
    void moveToScreen(int screen = 0);

private:
    ScreenModel     *m_screenModel;
    GreeterWindow   *m_greeterWnd;
    int              m_activeScreen;
    QPixmap          m_logo;
    QPixmap          m_background;
    bool             m_capsLock;
};

#endif // MAINWINDOW_H
