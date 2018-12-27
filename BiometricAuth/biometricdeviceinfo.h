#ifndef BIOMETRICDEVICEINFO_H
#define BIOMETRICDEVICEINFO_H

#include <memory>
#include <QtDBus>

#define BIOMETRIC_DBUS_SERVICE      "org.ukui.Biometric"
#define BIOMETRIC_DBUS_PATH         "/org/ukui/Biometric"
#define BIOMETRIC_DBUS_INTERFACE    "org.ukui.Biometric"

#define UKUI_BIOMETRIC_IMAGES_PATH  "/usr/share/ukui-biometric/images/"
#define UKUI_BIOMETRIC_CONFIG_PATH  ".biometric_auth/ukui_biometric.conf"
#define UKUI_BIOMETRIC_SYS_CONFIG_PATH  "/etc/biometric-auth/ukui-biometric.conf"

#define BIOMETRIC_PAM       "BIOMETRIC_PAM"
#define BIOMETRIC_IGNORE    "BIOMETRIC_IGNORE"
#define BIOMETRIC_SUCCESS   "BIOMETRIC_SUCCESS"

/**
 * @brief 设备类型
 */
class DeviceType : public QObject
{
    Q_OBJECT
public:
    DeviceType();
    enum Type {
        FingerPrint,
        FingerVein,
        Iris,
        Face,
        VoicePrint,
        __MAX_NR_TYPES
    };
    Q_ENUM(Type)
    /**
     * @brief 获取设备类型的字符串表现形式
     * @param deviceType 设备类型
     * @return
     */
    static QString getDeviceType(int deviceType);

    /**
     * @brief 获取设备类型的国际化字符串
     * @param deviceType 设备类型
     * @return
     */
    static QString getDeviceType_tr(int deviceType);
};

/**
 * @brief StatusChanged D-Bus 信号触发时的状态变化类型
 */
enum StatusType {
    STATUS_DEVICE,
    STATUS_OPERATION,
    STATUS_NOTIFY
};

/**
 * @brief 识别、终止操作等DBus调用的结果，即返回值里的 result
 */
enum DBusResult {
    DBUS_RESULT_SUCCESS = 0,
    DBUS_RESULT_NOTMATCH = -1,
    DBUS_RESULT_ERROR = -2,
    DBUS_RESULT_DEVICEBUSY = -3,
    DBUS_RESULT_NOSUCHDEVICE = -4,
    DBUS_RESULT_PERMISSIONDENIED = -5
};

/**
 * @brief 设备的信息
 */
struct DeviceInfo
{
    int         id;
    QString     shortName;
    QString     fullName;
    int         driverEnable;
    int         deviceNum;
    int         deviceType;
    int         storageType;
    int         eigType;
    int         verifyType;
    int         identifyType;
    int         busType;
    int         deviceStatus;
    int         OpsStatus;
};

QDBusArgument &operator <<(QDBusArgument &arg, const DeviceInfo &deviceInfo);
const QDBusArgument &operator >>(const QDBusArgument &arg, DeviceInfo &deviceInfo);

void registerMetaType();

typedef std::shared_ptr<DeviceInfo> DeviceInfoPtr;
typedef QList<DeviceInfoPtr> DeviceList;
typedef QMap<int, DeviceList> DeviceMap;

QDebug operator <<(QDebug stream, const DeviceInfo &deviceInfo);

Q_DECLARE_METATYPE(DeviceInfo)

/**
 * @brief 获取默认设备
 * @return
 */
QString GetDefaultDevice(const QString &userName);

/**
 * @brief 获取失败后自动重新开始的最大次数
 * @return
 */
int GetMaxAutoRetry(const QString &userName);

#endif // BIOMETRICDEVICEINFO_H
