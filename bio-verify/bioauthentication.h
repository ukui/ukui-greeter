/* bioauthentication.h
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
#ifndef BIOAUTHENTICATION_H
#define BIOAUTHENTICATION_H

#include <QObject>
#include <QTimer>
#include "biocustomtype.h"

class QDBusInterface;
class BioAuthentication : public QObject
{
    Q_OBJECT
public:
    explicit BioAuthentication(qint32 uid, const DeviceInfo &deviceInfo, QObject *parent = nullptr);
    void startAuthentication();
    void stopAuthentication();

signals:
    void authenticationComplete(bool result);
    void notify(const QString &message);

private slots:
    void onSearchResult(QDBusPendingCallWatcher *watcher);
    void onStatusChanged(int deviceId, int statusType);

private:
    QDBusInterface      *serviceInterface;

    qint32              uid;
    DeviceInfo          deviceInfo;
    QTimer              *timer;
};

#endif // BIOAUTHENTICATION_H
