/* displayservice.h
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
#ifndef DISPLAYSERVICE_H
#define DISPLAYSERVICE_H

#include <QObject>
#include <QVector>
#include <QMap>

enum DisplayMode{
    DISPLAY_MODE_ORI        = 1,
    DISPLAY_MODE_CLONE      = 2,
    DISPLAY_MODE_EXTEND     = 3,
    DISPLAY_MODE_ONLY_OUT   = 4
};

class DisplayService : public QObject
{
    Q_OBJECT
public:
    explicit DisplayService(QObject *parent = nullptr);
    bool switchDisplayMode(DisplayMode targetMode);

private:
    void getMonitors();

    QMap<QString, QVector<QString>> monitors;
    QStringList monitorNames;
};

#endif // DISPLAYSERVICE_H
