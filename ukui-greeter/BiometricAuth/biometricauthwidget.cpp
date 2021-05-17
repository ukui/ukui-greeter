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
#include "biometricauthwidget.h"
#include <QLabel>
#include <QDebug>
#include <pwd.h>

BiometricAuthWidget::BiometricAuthWidget(BiometricProxy *proxy, QWidget *parent) :
    QWidget(parent),
    proxy(proxy),
    isInAuth(false),
    movieTimer(nullptr),
    retrytimer(nullptr),
    failedCount(0),
    usebind(false),
    beStopped(false),
    timeoutCount(0)
{
    usebind = getAuthDouble();
    qDebug() << "BiometricAuthWidget::BiometricAuthWidget";
    initUI();
    resize(400, 200);

    if(this->proxy)
    {
        connect(this->proxy, &BiometricProxy::StatusChanged,
                this, &BiometricAuthWidget::onStatusChanged);
    }
}

void BiometricAuthWidget::initUI()
{
    //显示提示信息
    lblNotify = new QLabel(this);
    lblNotify->setWordWrap(true);
    lblNotify->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    //显示当前设备
    lblDevice = new QLabel(this);
    lblDevice->setWordWrap(true);
    lblDevice->setAlignment(Qt::AlignCenter);

    //显示图片
    lblImage = new QLabel(this);
    lblImage->setFixedSize(100, 100);
}


void BiometricAuthWidget::resizeEvent(QResizeEvent */*event*/)
{
    lblNotify->setGeometry(0, 0, width(), 45);
    lblDevice->setGeometry(0, lblNotify->geometry().bottom()+5, width(), 30);
    lblImage->setGeometry((width() - lblImage->width()) / 2,
                           lblDevice->geometry().bottom() + 10,
                           lblImage->width(), lblImage->height());
}

void BiometricAuthWidget::startAuth(DeviceInfoPtr device, int uid)
{
    if(!proxy)
    {
        qWarning() << "BiometricProxy doesn't exist.";
        return;
    }

    if(isInAuth)
    {
        qDebug() << "Identification is currently under way, stop it";
        stopAuth();
    }

    this->device = device;
    this->uid = uid;
    this->userName = getpwuid(uid)->pw_name;
    this->failedCount = 0;
    this->timeoutCount = 0;
    this->beStopped = false;
    startAuth_();
}

void BiometricAuthWidget::startAuth_()
{
    lblDevice->setText(tr("Current device: ") + device->shortName);

    qDebug().noquote() << QString("Identify:[drvid: %1, uid: %2]").arg(device->id).arg(uid);

    isInAuth = true;

    QDBusPendingCall call = proxy->Identify(device->id, uid);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &BiometricAuthWidget::onIdentifyComplete);

    updateImage(1);
}

void BiometricAuthWidget::stopAuth()
{
    beStopped = true;
    if(!isInAuth)
    {
        return;
    }
    proxy->StopOps(device->id);
    if(retrytimer&&retrytimer->isActive()){
        retrytimer->stop();
        delete retrytimer;
        retrytimer = nullptr;
    }
    isInAuth = false;
    updateImage(0);
}

void BiometricAuthWidget::onIdentifyComplete(QDBusPendingCallWatcher *watcher)
{
    if(beStopped == true)
        return ;

    QDBusPendingReply<int, int> reply = *watcher;
    if(reply.isError())
    {
        qWarning() << "Identify error: " << reply.error().message();
	isInAuth = false;
        Q_EMIT authComplete(false);
        updateImage(0);
        return;
    }
    int result = reply.argumentAt(0).toInt();
    int authUid = reply.argumentAt(1).toInt();
    qDebug() << result << authUid << uid;

    // 特征识别成功，而且用户id匹配
    if(result == DBUS_RESULT_SUCCESS && authUid == uid)
    {
        qDebug() << "Identify success";
        Q_EMIT authComplete(true);
    }
    // 特征识别不匹配
    else if(result == DBUS_RESULT_NOTMATCH)
    {
        if(usebind){
	    isInAuth = false;
            Q_EMIT authComplete(false);
            return;
        }

        qDebug() << "Identify failed";
        failedCount++;
        qDebug() << "max auto retry: " << GetMaxFailedAutoRetry(userName) << failedCount;
        if(failedCount >= GetMaxFailedAutoRetry(userName))
        {
            Q_EMIT authComplete(false);
        }
        else
        {
            lblNotify->setText(tr("Identify failed, Please retry."));
             if(!beStopped){
               // QTimer::singleShot(1000, this, &BiometricAuthWidget::startAuth_);
                if(!retrytimer){
                    retrytimer = new QTimer(this);
                    retrytimer->setSingleShot(true);
                    connect(retrytimer, &QTimer::timeout, this, &BiometricAuthWidget::startAuth_);
                 }
                 retrytimer->start(1000);
             }
        }
    }
    //识别发生错误
    else if(result == DBUS_RESULT_ERROR)
    {
        if(usebind){
	    isInAuth = false;
            Q_EMIT authComplete(false);
            return;
        }

        StatusReslut ret = proxy->UpdateStatus(device->id);
        //识别操作超时
        if(ret.result == 0 && ret.opsStatus == IDENTIFY_TIMEOUT)
        {
            timeoutCount++;
            if(timeoutCount >= GetMaxTimeoutAutoRetry(userName))
            {
                Q_EMIT authComplete(false);
            }
            else
            {
                QTimer::singleShot(1000, [&]{
                    if(!beStopped)
                    {
                        startAuth_();
                    }
                });
            }
        }else{
	    Q_EMIT authComplete(false);
	}
    }else{
    	Q_EMIT authComplete(false);
    }
    updateImage(0);
}

void BiometricAuthWidget::onStatusChanged(int drvid, int status)
{
    if(!isInAuth)
    {
        return;
    }
    if(drvid != device->id)
    {
        return;
    }

    // 显示来自服务的提示信息
    if(status == STATUS_NOTIFY)
    {
        QString notifyMsg = proxy->GetNotifyMesg(drvid);
        lblNotify->setText(notifyMsg);
    }
}

static int count = 0;
void BiometricAuthWidget::updateImage(int type)
{
    if(type == 0)
    {
        if(movieTimer && movieTimer->isActive())
        {
            movieTimer->stop();
        }

        QString imagePath = QString(UKUI_BIOMETRIC_IMAGES_PATH "%1/01.png")
                .arg(DeviceType::getDeviceType(device->deviceType));
        setImage(imagePath);
    }
    else
    {
        if(!movieTimer)
        {
            movieTimer = new QTimer(this);
            movieTimer->setInterval(100);
            connect(movieTimer, &QTimer::timeout,
                    this, &BiometricAuthWidget::onMoviePixmapUpdate);
        }
        count = 0;
        movieTimer->start();
    }
}

void BiometricAuthWidget::onMoviePixmapUpdate()
{
    if(count >= 18)
    {
        count = 0;
    }
    count++;
    QString fileName = (count < 10 ? "0" : "") + QString::number(count);
    QString imagePath = QString(UKUI_BIOMETRIC_IMAGES_PATH "%1/%2.png")
            .arg(DeviceType::getDeviceType(device->deviceType))
            .arg(fileName);
    setImage(imagePath);
}

void BiometricAuthWidget::setImage(const QString &path)
{
    QPixmap image(path);
    image = image.scaled(lblImage->width(), lblImage->height(),
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    lblImage->setPixmap(image);
}

bool BiometricAuthWidget::getAuthDouble()
{
    QSettings settings("/etc/biometric-auth/ukui-biometric.conf", QSettings::IniFormat);
    bool distribId = settings.value("DoubleAuth").toBool();
    return distribId;
}

void BiometricAuthWidget::setMinImage(float val)
{
    resize(400,100+100*val);
    lblImage->setFixedSize(100*val, 100*val);

}
