#include "biodeviceview.h"
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QHeaderView>
#include <QtMath>
#include <QLabel>
#include <QMovie>
#include <QListWidget>
#include <QPushButton>
#include <QKeyEvent>


BioDeviceView::BioDeviceView(qint32 uid, QWidget *parent)
    : QWidget(parent),
      uid(uid),
      currentIndex(0)
{
    qRegisterMetaType<DeviceInfo>();

    deviceTypes << "fingerprint" << "fingervein" << "iris";

    serviceInterface = new QDBusInterface("cn.kylinos.Biometric", "/cn/kylinos/Biometric",
                                          "cn.kylinos.Biometric", QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647);

    getDevicesList();

}

void BioDeviceView::setUid(qint32 uid)
{
    this->uid = uid;
    getFeaturesList();

    deviceCount = deviceFeaturesNum.size() + 1;

#ifdef TEST
    addTestDevices();
#endif

    initUI();
}

/**
 * @brief BioDeviceView::getDevicesList
 * 获取设备列表
 */
void BioDeviceView::getDevicesList()
{
    /* 返回值为 i -- int 和 av -- array of variant */
    QDBusMessage msg = serviceInterface->call("GetDrvList");
    if(msg.type() == QDBusMessage::ErrorMessage){
        qDebug() << msg.errorMessage();
        return;
    }
    /* 设备数量 */
    int deviceNum = msg.arguments().at(0).toInt();

    /* 读取设备详细信息，并存储到列表中 */
    QDBusArgument argument = msg.arguments().at(1).value<QDBusArgument>();
    QList<QVariant> infos;
    argument >> infos;
    for(int i = 0; i < deviceNum; i++) {
        DeviceInfo *deviceInfo = new DeviceInfo;
        infos.at(0).value<QDBusArgument>() >> *deviceInfo;

        if(deviceInfo->device_available > 0)     //设备可用
            deviceInfos.push_back(deviceInfo);
    }
}

/**
 * @brief BioDeviceView::getFeaturesList
 * 获取设备中的生物特征数量
 */
void BioDeviceView::getFeaturesList()
{
    deviceFeaturesNum.clear();

    for(int i = 0; i < deviceInfos.size(); i++) {
        DeviceInfo *deviceInfo = deviceInfos.at(i);
        QDBusMessage msg = serviceInterface->call("GetFeatureList", QVariant(deviceInfo->device_id),
                               QVariant(uid), QVariant(0), QVariant(-1));
        if(msg.type() == QDBusMessage::ErrorMessage){
            qDebug() << msg.errorMessage();
            continue;
        }
        int featuresNum = msg.arguments().at(0).toInt();
        if(featuresNum > 0)
            deviceFeaturesNum[deviceInfo->device_id] = featuresNum;
    }
    qDebug() << "存在" << deviceFeaturesNum.size() << "个录入了生物特征额设备";
}

/**
 * @brief BioDeviceView::devicesNum
 * @return
 * 返回可用的生物识别设备的数量
 */
int BioDeviceView::deviceNum()
{
    return deviceInfos.size();
}

/**
 * @brief BioDeviceView::featuresNum
 * @return
 * 返回录入了生物特征的可用生物识别设备的数量
 */
int BioDeviceView::usedDeviceNum()
{
    return deviceFeaturesNum.size();
}

void BioDeviceView::initUI()
{
    /* 只为该用户显示录入了生物特征的设备 */

    devicesList = new QListWidget(this);
    devicesList->setObjectName(QStringLiteral("devicesList"));
    devicesList->setFlow(QListWidget::LeftToRight);
    devicesList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    devicesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    devicesList->setFocusPolicy(Qt::NoFocus);
    devicesList->setSelectionMode(QListWidget::NoSelection);
    devicesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    devicesList->setFixedHeight(ITEM_SIZE);
    connect(devicesList, &QListWidget::currentItemChanged, this, [&]{
        currentIndex = devicesList->currentRow();
        QListWidgetItem *item = devicesList->currentItem();
        devicesList->itemWidget(item)->setFocus();
    });

    /* 为每一个设备添加一个icon到table中 */
    for(int i = 0; i < deviceCount; i++){
        QString iconName;
        if(i == 0)
            iconName = QString(":/resource/password-icon.png");
        else{
            DeviceInfo *deviceInfo = deviceInfos.at(i - 1);
            QString deviceType = deviceTypes.at(deviceInfo->biotype);
            iconName = QString(":/resource/%1-icon.png").arg(deviceType);
        }

        QLabel *iconLabel = new QLabel(devicesList);
        iconLabel->setObjectName(QString("bioIconLabel")+QString::number(i));
        iconLabel->installEventFilter(this);
        iconLabel->setFixedSize(ITEM_SIZE, ITEM_SIZE);
        iconLabel->setStyleSheet("QLabel{border:1px solid rgba(255, 255, 255, 100);}"
                                 "QLabel::hover{background:white;}"
                                 "QLabel::focus{background:white;}");
        QPixmap icon(iconName);
        icon = icon.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        iconLabel->setPixmap(icon);
        iconLabel->setAlignment(Qt::AlignCenter);

        QListWidgetItem *item = new QListWidgetItem(devicesList);
        item->setSizeHint(QSize(ITEM_SIZE * 2, ITEM_SIZE));
        devicesList->insertItem(i, item);
        devicesList->setItemWidget(item, iconLabel);

        setCurrentRow(0);
    }

    devicesList->resize(ITEM_SIZE*MAX_NUM*2, ITEM_SIZE);

    if(deviceCount > MAX_NUM) {
        prevButton = new QPushButton(this);
        prevButton->setObjectName(QStringLiteral("bioPrevButton"));
        QRect prevBtnRect(devicesList->geometry().right(),
                          devicesList->geometry().bottom() - ARROW_SIZE,
                          ARROW_SIZE, ARROW_SIZE);
        prevButton->setGeometry(prevBtnRect);

        nextButton = new QPushButton(this);
        nextButton->setObjectName(QStringLiteral("bioNextButton"));
        QRect nextBtnRect(prevBtnRect.right(), prevBtnRect.top(),
                          ARROW_SIZE, ARROW_SIZE);
        nextButton->setGeometry(nextBtnRect);
    }

    resize(350, BIODEVICEVIEW_HEIGHT);
}

void BioDeviceView::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Up:
        setCurrentRow(currentIndex-1);
        break;
    case Qt::Key_Down:
        setCurrentRow(currentIndex+1);
        break;
    case Qt::Key_Return:
        onDeviceIconClicked(currentIndex);
        break;
    }
    return QWidget::keyReleaseEvent(event);
}

void BioDeviceView::focusInEvent(QFocusEvent *event)
{
    QListWidgetItem *item = devicesList->item(currentIndex);
    devicesList->itemWidget(item)->setFocus();
    return QWidget::focusInEvent(event);
}

bool BioDeviceView::eventFilter(QObject *obj, QEvent *event)
{
    QString objName = obj->objectName();
    if(objName.left(12) == "bioIconLabel" && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        if(e->button() == Qt::LeftButton){
            int index = objName.right(objName.size() - 12).toInt();
            qDebug() << index;
            setCurrentRow(index);
            onDeviceIconClicked(index);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void BioDeviceView::setCurrentRow(int row)
{
    if(row >= devicesList->count() || row < 0)
        return;
    currentIndex = row;
    QListWidgetItem *item = devicesList->item(currentIndex);
    devicesList->setCurrentItem(item);
}

void BioDeviceView::onDeviceIconClicked(int index)
{
    qDebug() << index;
    if(index > deviceInfos.size())
        return;

    if(index == 0){
        qDebug() << "密码登录";
        Q_EMIT backToPasswd();
        return;
    }
    if(index > deviceCount){
        qDebug() << "test device";
        return;
    }

    DeviceInfo *deviceInfo = deviceInfos.at(index-1);

    Q_EMIT startVerification(*deviceInfo);

}

#ifdef TEST
void BioDeviceView::addTestDevices()
{
    for(int i = 0; i < 3; i++){
        DeviceInfo *info = new DeviceInfo;
        info->biotype = BIOTYPE_FINGERPRINT;
        info->driver_enable = 1;
        info->device_available = 1;
        deviceInfos.push_back(info);
    }
    for(int i = 0; i < 3; i++){
        DeviceInfo *info = new DeviceInfo;
        info->biotype = BIOTYPE_IRIS;
        info->driver_enable = 1;
        info->device_available = 1;
        deviceInfos.push_back(info);
    }
    deviceCount += 4;
}

#endif

