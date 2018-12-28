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
#include <QApplication>
#include <QDebug>
#include "biometricproxy.h"
#include "biometricauthwidget.h"
#include "biometricdeviceswidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BiometricProxy proxy;
    BiometricAuthWidget biometricAuthWidget(&proxy);
    biometricAuthWidget.hide();
    BiometricDevicesWidget biometricDeviceWidget(&proxy);
    QObject::connect(&biometricDeviceWidget, &BiometricDevicesWidget::deviceChanged,
                     &a, [&](const DeviceInfoPtr &pDeviceInfo){
        biometricAuthWidget.startAuth(pDeviceInfo, 1000);
        biometricAuthWidget.show();
    });
    QObject::connect(&biometricDeviceWidget, &BiometricDevicesWidget::deviceCountChanged,
                     &a, [&](int count){
        qDebug() << "device count changed: " << count;
    });
    biometricDeviceWidget.show();

    return a.exec();
}
