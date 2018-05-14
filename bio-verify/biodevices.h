#ifndef BIODEVICES_H
#define BIODEVICES_H

#include <QObject>
#include <QMap>
#include <QList>
#include "biocustomtype.h"

class QDBusInterface;
class BioDevices : public QObject
{
    Q_OBJECT
public:
    explicit BioDevices(QObject *parent = nullptr);
    int deviceCount();
    int featuresNum(qint32 uid);
    QList<DeviceInfo> getAvaliableDevices(qint32 uid);

private:
    void connectToService();
    void getDevicesList();
    void getFeaturesList(qint32 uid);

private:
    QDBusInterface                          *serviceInterface;

    static bool                             isFirst;
    static QList<DeviceInfo*>               deviceInfos;
    static QMap<int, QList<DeviceInfo>>     savedDeviceInfos;       //[uid, DeviceInfos]
};



#endif // BIODEVICES_H
