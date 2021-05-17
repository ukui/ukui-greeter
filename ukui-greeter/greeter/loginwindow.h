/* loginwindow.h
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QMap>
#include <QLightDM/Greeter>
#include <QLightDM/UsersModel>

#include "biometricdeviceinfo.h"

class QTimer;
class QLabel;
class QPushButton;
class QListWidget;

class GreeterWrapper;
class IconEdit;
class BiometricProxy;
class BiometricAuthWidget;
class BiometricDevicesWidget;

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(GreeterWrapper *greeter, QWidget *parent = 0);
    ~LoginWindow(){}

    bool setUserIndex(const QModelIndex& index);
    void setGreeter(GreeterWrapper *greeter);
    void setUserName(const QString& userName);
    QString getUserName();
    QString getRealName();
    void setFace(const QString& faceFile);
    void setLoggedIn(bool isLoggedIn);
    void setPrompt(const QString& text);
    QString getPassword();
    void reset();
    bool isloginauth;
    void setDirLogin();
    bool getIsChooseDev();
    bool isinput_passwd;


protected:
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void back();
    void bioDeviceIsChoosed(bool);

    /**
     * @brief 手动输入用户名
     * @param userName 用户名
     */
    void userChangedByManual(const QString &userName);

public slots:
    void onShowMessage(QString text, QLightDM::Greeter::MessageType type);
    void onShowPrompt(QString text, QLightDM::Greeter::PromptType type);
    void onAuthenticationComplete();
    void setUserNotInView(const QString &userName);
    void changeEvent(QEvent *event);
    void pamBioSuccess();
        
private slots:
    void onLogin(const QString &str);
    void onBackButtonClicked();
    void onBiometricButtonClicked();
    void onPasswordButtonClicked();
    void onOtherDevicesButtonClicked();
    void onRetryButtonClicked();
    void onDeviceChanged(const DeviceInfoPtr &deviceInfo);
    void onBiometricAuthComplete(bool result);
    void onMessageButtonClicked();
    void onPrepareForSleep(bool sleep);

private:
    void initUI();
    void initBiometricWidget();
    void initBiometricButtonWidget();
    void setChildrenGeometry();
    void setBiometricWidgetGeometry();
    void setBiometricButtonWidgetGeometry();
    void startAuthentication();
    void startWaiting();
    void stopWaiting();
    void clearMessage();
    void performBiometricAuth();
    void skipBiometricAuth();
    void showPasswordAuthWidget();
    void showBiometricAuthWidget();
    void showBiometricDeviceWidget();
    QPixmap DrawRound(QPixmap &src, int radius);
    QPixmap PixmapToRound(const QPixmap &src, int radius);
    void show_authenticated (bool successful = true);
    void refreshTranslate();
    void startBioAuth();
    void restartBioAuth();

private:
    GreeterWrapper      *m_greeter;
    QString     m_name;     //m_nameLabel显示的是全名(显示的),m_name保存的是真名(用于登录的)
    qint32      m_uid;      //用户id
    //手动输入用户标记，设置该标记的原因是判断是不是手动输入用户，
    //如果输入了无法登录的用户，就会一直输出密码错误信息
    bool        isManual;
    //密码错误标记，设置该标志的原因是，在有生物识别模块的情况下用户选择了密码登录，输入了错误的密码，
    //此时应该直接进入密码登录，而不是再次进入登录生物识别设备选择界面
//    bool        isPasswordError;

    enum AuthMode { PASSWORD, BIOMETRIC, UNKNOWN };

    AuthMode authMode;
    // 生物识别认证
    int                     m_deviceCount;
    int			    m_featureCount;
    QString                 m_deviceName;
    DeviceInfoPtr           m_deviceInfo;
    BiometricProxy          *m_biometricProxy;
    BiometricAuthWidget     *m_biometricAuthWidget;
    BiometricDevicesWidget  *m_biometricDevicesWidget;
    QWidget                 *m_buttonsWidget;
    QPushButton             *m_biometricButton;
    QPushButton             *m_passwordButton;
    QPushButton             *m_otherDeviceButton;
    QPushButton             *m_retryButton;

    // UI
    QPushButton     *m_backButton;         //返回用户列表
    QLabel          *m_faceLabel;         //头像
    QLabel          *m_nameLabel;         //用户名
    QLabel          *m_isLoginLabel;      //提示是否已登录

    QWidget         *m_passwdWidget;        //放置密码输入框和信息列表
    IconEdit        *m_passwordEdit;       //密码输入框
    QLabel          *m_messageLabel;         //PAM消息显示
    QPushButton     *m_messageButton;
    QTimer          *m_bioTimer;

    bool isretry = true;
    bool prompted = false;
    bool unacknowledged_messages = false;
    bool m_name_is_login = false;
    bool isauto_switch = false;
    bool direct_login = false;
    bool isChooseDev = false;
    bool useDoubleAuth = false;
    bool isBioSuccess = false;
    bool manualStopBio = false;
    bool useFirstDevice;
    bool doubleBioStarted = false;

    QMap<qint32,int>    m_failMap;
    int                 maxFailedTimes;
    bool                isHiddenSwitchButton;
    QString             manualLoginName;
};

#endif // LOGINWINDOW_H
