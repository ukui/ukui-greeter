/* displayswitch.h
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
#ifndef DISPLAYSWITCH_H
#define DISPLAYSWITCH_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class KeyEventMonitor;
class DisplaySwitch : public QMainWindow
{
    Q_OBJECT
public:
    explicit DisplaySwitch(QWidget *parent = nullptr);
    void initUi();

signals:

public slots:
    void onDisplaySwitchSelect();
    void onDisplaySwitchConfirm();
    void onPositionChanged(const QRect&);

private:
    Ui::MainWindow *ui;
    int     currentMode;
    KeyEventMonitor *keyEventMonitor;
};

#endif // DISPLAYSWITCH_H
