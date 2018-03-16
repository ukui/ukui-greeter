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

class QLabel;
class QPushButton;
class PowerWindow : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
public:
    explicit PowerWindow(bool hasOpenSessions = false, QWidget *parent = 0);
    void initUI();
    void startBackground(int begin, int end, bool show);
    int opacity();
    void setOpacity(int);
    void close();

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
//    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void aboutToClose();

private:
    bool    m_hasOpenSessions;
    qreal   m_opacity;
    QLightDM::PowerInterface *m_power;

    QWidget *m_centerWidget;
    QWidget *m_borderWidget;
    QLabel *m_prompt;
    QLabel *m_suspend;
    QLabel *m_hibernate;
    QLabel *m_restart;
    QLabel *m_shutdown;
    QPushButton *m_close;
    QLabel *m_suspendLabel;
    QLabel *m_hibernateLabel;
    QLabel *m_restartLabel;
    QLabel *m_shutdownLabel;
};

#endif // POWERWINDOW_H
