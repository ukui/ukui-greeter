/*
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
#ifndef BIOAUTHENTICATIONVIEW_H
#define BIOAUTHENTICATIONVIEW_H

#include <QWidget>
#include "biocustomtype.h"

class QLabel;
class QPushButton;
class QMovie;
class BioAuthenticationView : public QWidget
{
    Q_OBJECT
public:
    explicit BioAuthenticationView(QWidget *parent = nullptr);
    void initUI();
    void startVerification();
    void stopVerification();
    void setUid(qint32 uid);
    void setDeviceInfo(const DeviceInfo& deviceInfo);
    void setMovie();

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void back();
    void messageNotify(const QString &message);
    void authenticationResult(bool result);

public slots:
    void onDeviceSearchResult(QDBusPendingCallWatcher *watcher);
    void onDeviceStatusChanged(int deviceId, int statusType);
    void onBackButtonClicked();

private:
    QLabel              *movieLabel;
    QLabel              *notificationLabel;
    QPushButton         *backButton;
    QTimer              *timer;
    QDBusInterface      *serviceInterface;
    QMovie              *movie;

    DeviceInfo          deviceInfo;
    qint32              uid;
    bool                stopped;
};

#endif // BIOAUTHENTICATIONVIEW_H
