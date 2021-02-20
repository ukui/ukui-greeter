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
#ifndef BIOMETRICAUTHWIDGET_H
#define BIOMETRICAUTHWIDGET_H

#include <QWidget>
#include "biometricproxy.h"

class QLabel;
//class QDBusPendingCallWatcher;
//class BiometricProxy;
//class DeviceIdentityPtr;
class BiometricAuthWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BiometricAuthWidget(BiometricProxy *proxy, QWidget *parent = 0);

    /**
     * @brief 进行生物识别认证
     * @param deviceInfo    使用的设备
     * @param uid           待认证的用户id
     */
    void startAuth(DeviceInfoPtr device, int uid);

    /**
     * @brief 终止生物识别认证
     */
    void stopAuth();

    /**
     * @brief 是否正在认证
     * @return
     */
    bool isAuthenticating()
    { return isInAuth; }

    void setMinImage(float val);

protected:
    void resizeEvent(QResizeEvent *event);

Q_SIGNALS:
    /**
     * @brief 认证完成
     * @param result    认证结果
     */
    void authComplete(bool result);

private Q_SLOTS:
    void onIdentifyComplete(QDBusPendingCallWatcher *watcher);
    void onStatusChanged(int drvid, int status);
    void onMoviePixmapUpdate();

private:
    void initUI();
    void updateImage(int type = 0);
    void setImage(const QString &path);
    void startAuth_();
    bool getAuthDouble();

private:
    QLabel          *lblNotify;
    QLabel          *lblDevice;
    QLabel          *lblImage;

    BiometricProxy      *proxy;
    int                 uid;
    QString             userName;
    DeviceInfoPtr       device;
    bool                isInAuth;
    QTimer              *movieTimer;
    int                 failedCount;
    bool                beStopped;  //被强制终止
    int                 timeoutCount;
    QTimer              *retrytimer;
    bool                usebind;
};

#endif // BIOMETRICAUTHWIDGET_H
