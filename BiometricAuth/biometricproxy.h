#ifndef BIOMETRICPROXY_H
#define BIOMETRICPROXY_H

#include <QDBusAbstractInterface>
#include "biometricdeviceinfo.h"

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
