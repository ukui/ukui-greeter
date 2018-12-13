/* powerwindow.h
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
#ifndef POWERWINDOW_H
#define POWERWINDOW_H

#include <QWidget>
#include <QLightDM/Power>
#include "fakedialog.h"

class QLabel;
class PowerWindow : public FakeDialog
{
    Q_OBJECT
public:
    explicit PowerWindow(bool hasOpenSessions = false, QWidget *parent = 0);
    void initUI();

protected:
    bool eventFilter(QObject *, QEvent *);
    void mousePressEvent(QMouseEvent *event);

private:
    bool    m_hasOpenSessions;
    QLightDM::PowerInterface *m_power;

    QLabel *m_prompt;
    QLabel *m_suspend;
    QLabel *m_hibernate;
    QLabel *m_restart;
    QLabel *m_shutdown;
    QLabel *m_suspendLabel;
    QLabel *m_hibernateLabel;
    QLabel *m_restartLabel;
    QLabel *m_shutdownLabel;
};

#endif // POWERWINDOW_H
