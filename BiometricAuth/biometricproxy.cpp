#include "biometricproxy.h"
#include <QDebug>

QString DeviceType::getDeviceType(int deviceType)
{
    if(deviceType >= __MAX_NR_TYPES)
    {
        return "";
    }
    QMetaEnum meta = QMetaEnum::fromType<Type>();
    const char *typeString = meta.valueToKey(deviceType);
    return QString(typeString);
}

QString DeviceType::getDeviceType_tr(int deviceType)
{
    switch(deviceType)
    {
    case FingerPrint:
        return tr("FingerPrint");
    case FingerVein:
        return tr("FingerVein");
    case Iris:
        return tr("Iris");
    case Face:
        return tr("Face");
    case VoicePrint:
        return tr("VoicePrint");
    default:
        return "";
    }
}

QDebug operator <<(QDebug stream, const DeviceInfo &deviceInfo)
{
    stream << "["
           << deviceInfo.id
           << deviceInfo.shortName
           << deviceInfo.fullName
           << deviceInfo.deviceType
           << deviceInfo.driverEnable
           << deviceInfo.deviceNum
           << "]";
    return stream;
}

QDBusArgument &operator <<(QDBusArgument &arg, const DeviceInfo &deviceInfo)
{
    arg.beginStructure();
    arg << deviceInfo.id
        << deviceInfo.shortName
        << deviceInfo.fullName
        << deviceInfo.driverEnable
        << deviceInfo.deviceNum
        << deviceInfo.deviceType
        << deviceInfo.storageType
        << deviceInfo.eigType
        << deviceInfo.verifyType
        << deviceInfo.identifyType
        << deviceInfo.busType
        << deviceInfo.deviceStatus
        << deviceInfo.OpsStatus;
    arg.endStructure();
    return arg;
}
const QDBusArgument &operator >>(const QDBusArgument &arg, DeviceInfo &deviceInfo)
{
    arg.beginStructure();
    arg >> deviceInfo.id
        >> deviceInfo.shortName
        >> deviceInfo.fullName
        >> deviceInfo.driverEnable
        >> deviceInfo.deviceNum
        >> deviceInfo.deviceType
        >> deviceInfo.storageType
        >> deviceInfo.eigType
        >> deviceInfo.verifyType
        >> deviceInfo.identifyType
        >> deviceInfo.busType
        >> deviceInfo.deviceStatus
        >> deviceInfo.OpsStatus;
    arg.endStructure();
    return arg;
}

void registerMetaType()
{
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qDBusRegisterMetaType<DeviceInfo>();
}

BiometricProxy::BiometricProxy(QObject *parent)
    : QDBusAbstractInterface(BIOMETRIC_DBUS_SERVICE,
                             BIOMETRIC_DBUS_PATH,
                             BIOMETRIC_DBUS_INTERFACE,
                             QDBusConnection::systemBus(),
                             parent)
{
    registerMetaType();
}

QDBusPendingCall BiometricProxy::Identify(int drvid, int uid, int indexStart, int indexEnd)
{
    QList<QVariant> argList;
    argList << drvid << uid << indexStart << indexEnd;
    return asyncCallWithArgumentList(QStringLiteral("Identify"), argList);
}

int BiometricProxy::StopOps(int drvid, int waiting)
{
    QDBusReply<int> reply = call(QStringLiteral("StopOps"), drvid, waiting);
    if(!reply.isValid())
    {
        qWarning() << "StopOps error:" << reply.error();
        return -1;
    }
    return reply.value();
}

DeviceList BiometricProxy::GetDevList()
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return DeviceList();
    }
    auto dbusArg =  result.arguments().at(1).value<QDBusArgument>();
    QList<QVariant> variantList;
    DeviceList deviceList;
    dbusArg >> variantList;
    for(int i = 0; i < variantList.size(); i++)
    {
        DeviceInfoPtr pDeviceInfo = std::make_shared<DeviceInfo>();

        auto arg = variantList.at(i).value<QDBusArgument>();
        arg >> *pDeviceInfo;

        deviceList.push_back(pDeviceInfo);
    }

    return deviceList;
}

int BiometricProxy::GetDevCount()
{
    QDBusMessage result = call(QStringLiteral("GetDevList"));
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevList error:" << result.errorMessage();
        return 0;
    }
    int count = result.arguments().at(0).value<int>();
    return count;
}

QString BiometricProxy::GetDefaultDevice(const QString &userName)
{
    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
    QSettings settings(configPath, QSettings::IniFormat);

    QString defaultDevice = settings.value("DefaultDevice").toString();
    if(defaultDevice.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        defaultDevice = sysSettings.value("DefaultDevice").toString();
    }

    return defaultDevice;
}

QString BiometricProxy::GetDevMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetDevMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetDevMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

QString BiometricProxy::GetNotifyMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetNotifyMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetNotifyMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

QString BiometricProxy::GetOpsMesg(int drvid)
{
    QDBusMessage result = call(QStringLiteral("GetOpsMesg"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "GetOpsMesg error:" << result.errorMessage();
        return "";
    }
    return result.arguments().at(0).toString();
}

StatusReslut BiometricProxy::UpdateStatus(int drvid)
{
    StatusReslut status;
    QDBusMessage result = call(QStringLiteral("UpdateStatus"), drvid);
    if(result.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "UpdateStatus error:" << result.errorMessage();
        status.result = -1;
        return status;
    }

    status.result           = result.arguments().at(0).toInt();
    status.enable           = result.arguments().at(1).toInt();
    status.devNum           = result.arguments().at(2).toInt();
    status.devStatus        = result.arguments().at(3).toInt();
    status.opsStatus        = result.arguments().at(4).toInt();
    status.notifyMessageId  = result.arguments().at(5).toInt();

    return status;
}
