#include "biodevices.h"
#include <QDBusInterface>

bool BioDevices::isFirst = true;
QList<DeviceInfo*>  BioDevices::deviceInfos;
QMap<int, QList<DeviceInfo>>      BioDevices::savedDeviceInfos;       //[uid, featuresNum]

BioDevices::BioDevices(QObject *parent)
    : QObject(parent)
{
    connectToService();
    if(isFirst){
        getDevicesList();
        isFirst = false;
    }
}

void BioDevices::connectToService()
{
    qRegisterMetaType<DeviceInfo>();

    serviceInterface = new QDBusInterface("cn.kylinos.Biometric", "/cn/kylinos/Biometric",
                                          "cn.kylinos.Biometric", QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647);
}


/**
 * 获取设备列表
 */
void BioDevices::getDevicesList()
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
 * 获取设备中的生物特征数量
 */
void BioDevices::getFeaturesList(qint32 uid)
{
    if(savedDeviceInfos.contains(uid)){
        qDebug() << "saved";
        return;
    }

    savedDeviceInfos[uid] = QList<DeviceInfo>();

    for(int i = 0; i < deviceInfos.size(); i++) {
        DeviceInfo *deviceInfo = deviceInfos.at(i);
        QDBusMessage msg = serviceInterface->call("GetFeatureList", QVariant(deviceInfo->device_id),
                               QVariant(uid), QVariant(0), QVariant(-1));
        if(msg.type() == QDBusMessage::ErrorMessage){
            qDebug() << msg.errorMessage();
            continue;
        }
        int featuresNum = msg.arguments().at(0).toInt();

        if(featuresNum > 0 && !savedDeviceInfos[uid].contains(*deviceInfo))
            savedDeviceInfos[uid].push_back(*deviceInfo);
    }
    qDebug() << "存在" << savedDeviceInfos[uid].size() << "个录入了生物特征设备";
}

int BioDevices::deviceCount()
{
    return deviceInfos.size();
}

int BioDevices::featuresNum(qint32 uid)
{
    getFeaturesList(uid);
    return savedDeviceInfos[uid].size();
}

QList<DeviceInfo> BioDevices::getAvaliableDevices(qint32 uid)
{
    getFeaturesList(uid);
    return savedDeviceInfos[uid];
}
