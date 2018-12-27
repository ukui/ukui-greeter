#include "biometricdeviceinfo.h"

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

QString GetDefaultDevice(const QString &userName)
{
    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
    QSettings settings(configPath, QSettings::IniFormat);
    qDebug() << "configure path: " << settings.fileName();

    QString defaultDevice = settings.value("DefaultDevice").toString();
    if(defaultDevice.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        defaultDevice = sysSettings.value("DefaultDevice").toString();
    }

    return defaultDevice;
}

int GetMaxAutoRetry(const QString &userName)
{
    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
    QSettings settings(configPath, QSettings::IniFormat);

    int maxAutoRetry = settings.value("MaxAutoRetry").toInt();
    if(maxAutoRetry == 0)
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        maxAutoRetry = sysSettings.value("MaxAutoRetry", 3).toInt();
    }
    return maxAutoRetry;
}
