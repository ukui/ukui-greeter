#include "bioauthenticationview.h"
#include <QLabel>
#include <QPushButton>
#include <QMovie>
#include <QKeyEvent>

#define MOVIE_SIZE 240

BioAuthenticationView::BioAuthenticationView(QWidget *parent)
    : QWidget(parent),
      movie(nullptr),
      stopped(false)
{
    serviceInterface = new QDBusInterface(DBUS_SERVICE,
                                          DBUS_PATH,
                                          DBUS_INTERFACE,
                                          QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647);
    connect(serviceInterface, SIGNAL(StatusChanged(int, int)),
            this, SLOT(onDeviceStatusChanged(qint32,qint32)));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &BioAuthenticationView::startVerification);

    initUI();
}

void BioAuthenticationView::initUI()
{
    backButton = new QPushButton(this);
    backButton->setObjectName(QStringLiteral("backButton"));
    QRect backBtnRect(0, 0, 32, 32);
    backButton->setGeometry(backBtnRect);
    connect(backButton, &QPushButton::clicked, this,
            &BioAuthenticationView::onBackButtonClicked);
    backButton->setShortcut(QKeySequence("esc"));

    movieLabel = new QLabel(this);
    QRect movieLbRect(100, 0, MOVIE_SIZE, MOVIE_SIZE);
    movieLabel->setGeometry(movieLbRect);

    notificationLabel = new QLabel(this);
    notificationLabel->setAlignment(Qt::AlignCenter);
    notificationLabel->setFont(QFont("ubuntu", 12));
    QRect ntfLbRect(movieLbRect.left(), movieLbRect.bottom()+10,
                    movieLbRect.width(), 30);
    notificationLabel->setGeometry(ntfLbRect);

    resize(500, MOVIE_SIZE + 50);
    QWidget *parent = parentWidget();
    move(parent->geometry().left() + (parent->width() - width()) / 2,
         parent->geometry().top() + (parent->height() - height()) / 2);
}

void BioAuthenticationView::setUid(qint32 uid)
{
    this->uid = uid;
    qDebug() << "uid: "<< uid;
}

void BioAuthenticationView::setDeviceInfo(const DeviceInfo &deviceInfo)
{
    this->deviceInfo = std::move(deviceInfo);
    setMovie();
    qDebug() << "device name: "<< deviceInfo.device_fullname;
}

void BioAuthenticationView::setMovie()
{
    QString movieName;
    switch(deviceInfo.biotype){
    case BIOTYPE_FINGERPRINT:
        movieName = ":/resource/fingerprint.gif";
        break;
    case BIOTYPE_FINGERVEIN:
        movieName = ":/resource/fingervein.gif";
        break;
    case BIOTYPE_IRIS:
        movieName = ":/resource/iris.gif";
        break;
    }

    if(movie)
        delete(movie);
    movie = new QMovie(movieName);
    movieLabel->setMovie(movie);
    movie->setScaledSize(QSize(MOVIE_SIZE,MOVIE_SIZE));
}

void BioAuthenticationView::resizeEvent(QResizeEvent *event)
{
    return QWidget::resizeEvent(event);
}

void BioAuthenticationView::closeEvent(QCloseEvent *event)
{
    stopVerification();
    return QWidget::closeEvent(event);
}


void BioAuthenticationView::onBackButtonClicked()
{
    stopVerification();
    hide();
    Q_EMIT back();
}

void BioAuthenticationView::startVerification()
{
    this->show();
    movie->start();
    /* 开始认证识别 */
    qDebug() << "start biometric verification";
    QList<QVariant> args;
    args << QVariant(deviceInfo.device_id) << QVariant(uid)
         << QVariant(0) << QVariant(-1);
    QDBusPendingCall call = serviceInterface->asyncCallWithArgumentList("Search", args);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &BioAuthenticationView::onDeviceSearchResult);
}

void BioAuthenticationView::stopVerification()
{
    if(timer->isActive())
        timer->stop();

    serviceInterface->asyncCall("StopOps", QVariant(deviceInfo.device_id), QVariant(5));
//    if(msg.type() == QDBusMessage::ErrorMessage) {
//        qDebug() << msg.errorMessage();
//    }

    stopped = true;

    qDebug()<< "stop biometric verification";
}

/**
 * @brief BioAuthenticationView::onDeviceSearchResult
 * @param watcher
 * 获取到搜索结果
 */
void BioAuthenticationView::onDeviceSearchResult(QDBusPendingCallWatcher *watcher)
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
        Q_EMIT authenticationResult(true);
    /* 识别生物特征失败，重新开始查找 */
    else if(!stopped)
        timer->start(3*1000);   //先停3s
}

/**
 * @brief BioAuthenticationView::onDeviceStatusChanged
 * @param deviceId
 * @param statusType
 * 设备状态发生变化
 */
void BioAuthenticationView::onDeviceStatusChanged(int deviceId, int statusType)
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
    notificationLabel->setText(message);
}
