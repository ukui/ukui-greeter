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
//    qDebug() << "configure path: " << settings.fileName();

    QString defaultDevice = settings.value("DefaultDevice").toString();
    if(defaultDevice.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        defaultDevice = sysSettings.value("DefaultDevice").toString();
    }

    return defaultDevice;
}

static int getValueFromSettings(const QString &userName, const QString &key, int defaultValue = 3)
{
    //从家目录下的配置文件中获取
    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
    QSettings settings(configPath, QSettings::IniFormat);
    QString valueStr = settings.value(key).toString();

    //如果没有获取到，则从系统配置文件中获取
    if(valueStr.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        valueStr = sysSettings.value(key).toString();
    }

    bool ok;
    int value = valueStr.toInt(&ok);
    if( (value == 0 && !ok) || valueStr.isEmpty() )
    {
        value = defaultValue;
    }
    return value;
}

int GetMaxFailedAutoRetry(const QString &userName)
{
    return getValueFromSettings(userName, "MaxFailedAutoRetry");
}

int GetMaxTimeoutAutoRetry(const QString &userName)
{
    return getValueFromSettings(userName, "MaxTimeoutAutoRetry");
}
