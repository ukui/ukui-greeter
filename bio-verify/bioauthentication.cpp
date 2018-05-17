/* bioauthentication.cpp
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
#include "bioauthentication.h"
#include <QList>

BioAuthentication::BioAuthentication(qint32 uid, const DeviceInfo &deviceInfo, QObject *parent)
    : QObject(parent),
      uid(uid),
      deviceInfo(deviceInfo)
{
    serviceInterface = new QDBusInterface("cn.kylinos.Biometric", "/cn/kylinos/Biometric",
                                          "cn.kylinos.Biometric", QDBusConnection::systemBus());
    connect(serviceInterface, SIGNAL(StatusChanged(int, int)),
            this, SLOT(onStatusChanged(int,int)));
    serviceInterface->setTimeout(2147483647);
}

void BioAuthentication::startAuthentication()
{
    /* 开始认证识别 */
    qDebug() << "start biometric verification";
    QList<QVariant> args;
    args << QVariant(deviceInfo.device_id) << QVariant(uid)
         << QVariant(0) << QVariant(-1);
    QDBusPendingCall call = serviceInterface->asyncCallWithArgumentList("Search", args);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &BioAuthentication::onSearchResult);
}


void BioAuthentication::stopAuthentication()
{
    serviceInterface->asyncCall("StopOps", QVariant(deviceInfo.device_id), QVariant(5));
}

void BioAuthentication::onSearchResult(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<qint32, qint32, qint32, QString> reply = *watcher;
    if(reply.isError()){
        qDebug() << reply.error();
        return;
    }
    qint32 result = reply.argumentAt(0).toInt();
    qint32 retUid = reply.argumentAt(1).toInt();
    qint32 idx = reply.argumentAt(2).toInt();
    QString idxName = reply.argumentAt(3).toString();
    qDebug() << result << " " << retUid << " " << idx << " " << idxName;

    /* 识别生物特征成功，发送认证结果 */
    if(result == DBUS_RESULT_SUCCESS && retUid == uid)
        Q_EMIT authenticationComplete(true);
}


void BioAuthentication::onStatusChanged(int deviceId, int statusType)
{
    if(statusType != STATUS_NOTIFY)
        return;
    qDebug() << "status changed " << deviceId << " " << statusType;
    QDBusMessage msg = serviceInterface->call("GetNotifyMesg", QVariant(deviceId));
    if(msg.type() == QDBusMessage::ErrorMessage){
        qDebug() << msg.errorMessage();
        return;
    }
    QString message = msg.arguments().at(0).toString();
    qDebug() << message;
    Q_EMIT notify(message);
}

