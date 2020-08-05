/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#ifndef XEVENTMONITOR_H
#define XEVENTMONITOR_H

#include <QThread>
#include <QMetaMethod>
#include <QDebug>

class XEventMonitorPrivate;
class XEventMonitor : public QThread
{
    Q_OBJECT

public:
    static XEventMonitor *instance() { return instance_; }

private:
    XEventMonitor(QObject *parent = 0);
    ~XEventMonitor();
    
Q_SIGNALS:
    ///
    /// \brief 鼠标按下
    /// \param x
    /// \param y
    ///
    void buttonPress(int x, int y);
    ///
    /// \brief 鼠标拖动
    /// \param x
    /// \param y
    ///
    void buttonDrag(int x, int y);
    ///
    /// \brief 鼠标松开
    /// \param x
    /// \param y
    ///
    void buttonRelease(int x, int y);

    ///
    /// \brief keyPress
    /// \param 按键键码，不包含修饰键
    ///
    void keyPress(int keyCode);
    void keyRelease(int keyCode);

    ///
    /// \brief keyPress
    /// \param 按键名称，如果是组合键，则包含修饰键的名称，如Ctrl_L+r
    ///
    void keyPress(const QString &key);
    void keyRelease(const QString &key);

protected:
    void run();
    
private:
    Q_DECLARE_PRIVATE(XEventMonitor)
    XEventMonitorPrivate *d_ptr;

    static XEventMonitor *instance_;
};

/**
 * @brief 检查大写开关状态
 * @return 开(true)，关(false)
 */
bool checkCapsState();
bool checkNumLockState();
#endif
