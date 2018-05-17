/* biodevices.h
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
#ifndef BIODEVICES_H
#define BIODEVICES_H

#include <QObject>
#include <QMap>
#include <QList>
#include "biocustomtype.h"

class QDBusInterface;
class BioDevices : public QObject
{
    Q_OBJECT
public:
    explicit BioDevices(QObject *parent = nullptr);
    int deviceCount();
    int featuresNum(qint32 uid);
    QList<DeviceInfo> getAvaliableDevices(qint32 uid);

private:
    void connectToService();
    void getDevicesList();
    void getFeaturesList(qint32 uid);

private:
    QDBusInterface                          *serviceInterface;

    static bool                             isFirst;
    static QList<DeviceInfo*>               deviceInfos;
    static QMap<int, QList<DeviceInfo>>     savedDeviceInfos;       //[uid, DeviceInfos]
};



#endif // BIODEVICES_H
