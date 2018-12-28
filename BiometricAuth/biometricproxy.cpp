/**
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
#include "biometricproxy.h"
#include <QDebug>

BiometricProxy::BiometricProxy(QObject *parent)
    : QDBusAbstractInterface(BIOMETRIC_DBUS_SERVICE,
                             BIOMETRIC_DBUS_PATH,
                             BIOMETRIC_DBUS_INTERFACE,
                             QDBusConnection::systemBus(),
                             parent)
{
    registerMetaType();
    setTimeout(2147483647);
}

QDBusPendingCall BiometricProxy::Identify(int drvid, int uid, int indexStart, int indexEnd)
{
    QList<QVariant> argList;
    argList << drvid << uid << indexStart << indexEnd;
    return asyncCallWithArgumentList(QStringLiteral("Identify"), argList);
}

int BiometricProxy::StopOps(int drvid, int waiting)
{
    QDBusReply<int> reply = call(QStringLiteral("StopOps"), drvid, waiting);
    if(!reply.isValid())
    {
        qWarning() << "StopOps error:" << reply.error();
        return -1;
    }
    return reply.value();
}

DeviceList BiometricProxy::GetDevList()
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return DeviceList();
    }
    auto dbusArg =  result.arguments().at(1).value<QDBusArgument>();
    QList<QVariant> variantList;
    DeviceList deviceList;
    dbusArg >> variantList;
    for(int i = 0; i < variantList.size(); i++)
    {
        DeviceInfoPtr pDeviceInfo = std::make_shared<DeviceInfo>();

        auto arg = variantList.at(i).value<QDBusArgument>();
        arg >> *pDeviceInfo;

        deviceList.push_back(pDeviceInfo);
    }

    return deviceList;
}

int BiometricProxy::GetDevCount()
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return 0;
    }
    int count = result.arguments().at(0).value<int>();
    return count;
}

QString BiometricProxy::GetDevMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetDevMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

QString BiometricProxy::GetNotifyMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetNotifyMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetNotifyMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

QString BiometricProxy::GetOpsMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetOpsMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetOpsMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

StatusReslut BiometricProxy::UpdateStatus(int drvid)
{
    StatusReslut status;
    QDBusMessage result = call(QStringLiteral("UpdateStatus"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "UpdateStatus error:" << result.errorMessage();
        status.result = -1;
        return status;
    }

    status.result           = result.arguments().at(0).toInt();
    status.enable           = result.arguments().at(1).toInt();
    status.devNum           = result.arguments().at(2).toInt();
    status.devStatus        = result.arguments().at(3).toInt();
    status.opsStatus        = result.arguments().at(4).toInt();
    status.notifyMessageId  = result.arguments().at(5).toInt();

    return status;
}
