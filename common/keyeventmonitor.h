/* keyeventmonitor.h
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
#ifndef KEYEVENTMONITOR_H
#define KEYEVENTMONITOR_H

#include <QThread>
#include <X11/Xlib.h>
#include <X11/extensions/record.h>

class KeyEventMonitor : public QThread
{
	Q_OBJECT

private:
    explicit KeyEventMonitor(QObject *parent = 0);
    bool checkCapsState();

public:
    ~KeyEventMonitor();
    static KeyEventMonitor *instance(QObject *parent = 0);

Q_SIGNALS:
    void CapsLockChanged(int);
    void displaySwitchSelect();
    void displaySwitchConfirm();

protected:
	static void callback(XPointer trash, XRecordInterceptData* data);
	void handleRecordEvent(XRecordInterceptData *);
	void run();

private:
    bool                    hasModifer;
    bool                    isCapsLock;
    static KeyEventMonitor *instance_;
};

#endif // KEYEVENTMONITOR_H
