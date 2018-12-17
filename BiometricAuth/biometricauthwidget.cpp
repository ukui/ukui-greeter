#include "biometricauthwidget.h"
#include <QLabel>
#include <QDebug>
//#include "biometricproxy.h"

BiometricAuthWidget::BiometricAuthWidget(BiometricProxy *proxy, QWidget *parent) :
    QWidget(parent),
    proxy(proxy),
    isInAuth(false),
    movieTimer(nullptr)
{
    initUI();
    resize(400, 300);

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
    lblNotify->setGeometry(0, 0, width(), 40);
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

    lblDevice->setText(tr("Current device: ") + device->shortName);

    qDebug().noquote() << QString("Identify:[drvid: %1, uid: %2]").arg(1).arg(2);

    isInAuth = true;

    QDBusPendingCall call = proxy->Identify(device->id, uid);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &BiometricAuthWidget::onIdentifyComplete);

    updateImage(1);
}

void BiometricAuthWidget::stopAuth()
{
    if(!isInAuth)
    {
        return;
    }
    proxy->StopOps(device->id);
    isInAuth = false;
    updateImage(0);
}

void BiometricAuthWidget::onIdentifyComplete(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<int, int> reply = *watcher;
    if(reply.isError())
    {
        qWarning() << "Identify error: " << reply.error().message();
        Q_EMIT authComplete(false);
        updateImage(0);
        return;
    }
    int result = reply.argumentAt(0).toInt();
    int authUid = reply.argumentAt(1).toInt();

    // 特征识别成功，而且用户id匹配
    if(result == DBUS_RESULT_SUCCESS && authUid == uid)
    {
        qDebug() << "Identify success";
        Q_EMIT authComplete(true);
    }
    else
    {
        qDebug() << "Identify failed";
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
