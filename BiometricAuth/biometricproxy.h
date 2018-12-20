#ifndef BIOMETRICPROXY_H
#define BIOMETRICPROXY_H

#include <QtDBus>
#include <QDBusAbstractInterface>

#define BIOMETRIC_DBUS_SERVICE      "org.ukui.Biometric"
#define BIOMETRIC_DBUS_PATH         "/org/ukui/Biometric"
#define BIOMETRIC_DBUS_INTERFACE    "org.ukui.Biometric"

#define UKUI_BIOMETRIC_IMAGES_PATH  "/usr/share/ukui-biometric/images/"
#define UKUI_BIOMETRIC_CONFIG_PATH  ".config/ukui-biometric/biometric-auth.conf"
#define UKUI_BIOMETRIC_SYS_CONFIG_PATH  "/usr/share/ukui-biometric/biometric-auth.conf"

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
 * @brief UpdateStauts调用返回的结果
 */
struct StatusReslut
{
    int result;
    int enable;
    int devNum;
    int devStatus;
    int opsStatus;
    int notifyMessageId;
};

/**
 * @brief USB设备插拔动作
 */
enum USBDeviceAction
{
    ACTION_ATTACHED = 1,
    ACTION_DETACHED = -1
};

/**
 * @brief DBus代理类，负责调用对应的DBus接口
 */
class BiometricProxy : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    explicit BiometricProxy(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief 使用指定id的设备进行用户认证
     * @param drvid         驱动（设备）id
     * @param uid           用户id
     * @param indexStart    用于认证的特征索引范围
     * @param indexEnd
     * @return  结果：<int result, int uid> （结果，用户id)
     */
    QDBusPendingCall Identify(int drvid, int uid, int indexStart = 0, int indexEnd = -1);
    /**
     * @brief 终止设备上正在进行的操作
     * @param drvid     设备id
     * @param waiting   等待时间（秒）
     * @return
     */
    int StopOps(int drvid, int waiting = 5);
    /**
     * @brief 获取已连接的设备列表
     * @return
     */
    DeviceList GetDevList();
    /**
     * @brief 获取设备数量
     * @return
     */
    int GetDevCount();
    /**
     * @brief 获取默认设备
     * @return
     */
    QString GetDefaultDevice(const QString &userName);
    /**
     * @brief 获取设备消息
     * @param drvid 驱动id
     * @return
     */
    QString GetDevMesg(int drvid);
    /**
     * @brief GetNotifyMesg 获取通知消息
     * @param drvid 驱动id
     * @return
     */
    QString GetNotifyMesg(int drvid);
    /**
     * @brief GetOpsMesg    获取操作消息
     * @param drvid 驱动id
     * @return
     */
    QString GetOpsMesg(int drvid);
    /**
     * @brief UpdateStatus  获取更新的设备状态
     * @param drvid 驱动id
     * @return  结果：<int result, int enable, int devNum,
     *                int devStatus, int opsStatus, notifyMessageId, ...>
     */
    StatusReslut UpdateStatus(int drvid);

Q_SIGNALS:
    /**
     * @brief 设备状态发生变化
     * @param drvid     设备id
     * @param status    设备状态
     */
    void StatusChanged(int drvid, int status);
    /**
     * @brief USB设备热插拔
     * @param drvid     设备id
     * @param action    插拔动作（1：插入，-1：拔出）
     * @param deviceNum 插拔动作后该驱动拥有的设备数量
     */
    void USBDeviceHotPlug(int drvid, int action, int deviceNum);
};

#endif // BIOMETRICPROXY_H
