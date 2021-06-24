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
#ifndef BIOMETRICDEVICESWIDGET_H
#define BIOMETRICDEVICESWIDGET_H

#include <QWidget>
#include "biometricproxy.h"

class QLabel;
class QPushButton;
class QComboBox;


class BiometricDevicesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BiometricDevicesWidget(BiometricProxy *proxy,int uid,QWidget *parent = nullptr);
    void setCurrentDevice(int drvid);
    void setCurrentDevice(const QString &deviceName);
    void setCurrentDevice(const DeviceInfoPtr &pDeviceInfo);
    DeviceInfoPtr findDeviceById(int drvid);
    DeviceInfoPtr findDeviceByName(const QString &name);
    bool deviceExists(int drvid);
    bool deviceExists(const QString &deviceName);
    void setUser(int uid);

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void deviceChanged(const DeviceInfoPtr &pDeviceInfo);
    void deviceCountChanged(int newCount);

public slots:
    void onOKButtonClicked();

private slots:
    void onCmbDeviceTypeCurrentIndexChanged(int index);
    void onUSBDeviceHotPlug(int drvid, int action, int devNum);

private:
    void initUI();
    void updateDevice();

private:
    typedef QMap<int, QPushButton*> QButtonMap;

    QLabel              *lblPrompt;
    QLabel              *lblDeviceType;
    QLabel              *lblDeviceName;
    QComboBox           *cmbDeviceType;
    QComboBox           *cmbDeviceName;
    QPushButton         *btnOK;
    QPushButton         *btnCancel;

    BiometricProxy      *proxy;
    DeviceMap           deviceMap;
    DeviceInfoPtr       currentDevice;
    int                 m_uid;
};

#endif // BIOMETRICDEVICESWIDGET_H
