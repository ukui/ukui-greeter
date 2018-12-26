#ifndef BIOMETRICAUTHWIDGET_H
#define BIOMETRICAUTHWIDGET_H

#include <QWidget>
#include "biometricproxy.h"

class QLabel;
//class QDBusPendingCallWatcher;
//class BiometricProxy;
//class DeviceIdentityPtr;
class BiometricAuthWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BiometricAuthWidget(BiometricProxy *proxy, QWidget *parent = 0);

    /**
     * @brief 进行生物识别认证
     * @param deviceInfo    使用的设备
     * @param uid           待认证的用户id
     */
    void startAuth(DeviceInfoPtr device, int uid);

    /**
     * @brief 终止生物识别认证
     */
    void stopAuth();

    /**
     * @brief 是否正在认证
     * @return
     */
    bool isAuthenticating()
    { return isInAuth; }

protected:
    void resizeEvent(QResizeEvent *event);

Q_SIGNALS:
    /**
     * @brief 认证完成
     * @param result    认证结果
     */
    void authComplete(bool result);

private Q_SLOTS:
    void onIdentifyComplete(QDBusPendingCallWatcher *watcher);
    void onStatusChanged(int drvid, int status);
    void onMoviePixmapUpdate();

private:
    void initUI();
    void updateImage(int type = 0);
    void setImage(const QString &path);
    void startAuth_();

private:
    QLabel          *lblNotify;
    QLabel          *lblDevice;
    QLabel          *lblImage;

    BiometricProxy      *proxy;
    int                 uid;
    QString             userName;
    DeviceInfoPtr       device;
    bool                isInAuth;
    QTimer              *movieTimer;
    int                 failedCount;
};

#endif // BIOMETRICAUTHWIDGET_H
