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
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QSettings>
#include <QTimer>
#include <QLightDM/Greeter>
#include <QLightDM/UsersModel>
#include "iconedit.h"
#include "greeterwrapper.h"

class BioDeviceView;
struct DeviceInfo;

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QSharedPointer<GreeterWrapper> greeter, QWidget *parent = 0);
    ~LoginWindow(){}

    void setUsersModel(QSharedPointer<QAbstractItemModel> model);
    bool setUserIndex(const QModelIndex& index);
    void setSessionsModel(QSharedPointer<QAbstractItemModel> model);
    bool setSessionIndex(const QModelIndex& index);
    void setGreeter(QSharedPointer<GreeterWrapper> greeter);
    void setUserName(const QString& userName);
    QString getUserName();
    void setFace(const QString& faceFile);
    void setLoggedIn(bool isLoggedIn);
    void setPrompt(const QString& text);
    QString getPassword();
    void setSession(QString);
    QString getSession();
    void recover();

private:
    void initUI();    
    int sessionIndex(const QString &session);
    void saveLastLoginUser();
    void backToUsers();
    void clearMessage();
    bool enableBioAuthentication(QString &message);

protected:
    void showEvent(QShowEvent *);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void back();
    void selectSession(const QString& );

public slots:
    void startAuthentication();
    void startWaiting();
    void stopWaiting();
    void updatePixmap();
    void onSessionSelected(const QString&);
    void onShowMessage(QString text, QLightDM::Greeter::MessageType type);
    void onShowPrompt(QString text, QLightDM::Greeter::PromptType type);
    void onAuthenticationComplete();
    void onLogin(const QString &str);
    void onSessionButtonClicked();
//    void onBioStartVerification(const DeviceInfo& deviceInfo);
    void onBioAuthenticationReslut(bool result);
    void onBioBackToPassword();
    void onBioNotify(const QString &message);

private:
    QSharedPointer<QAbstractItemModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
    QString     m_session;  //session的标识
    QString     m_name;     //m_nameLabel显示的是全名(显示的),m_name保存的是真名(用于登录的)
    qint32      m_uid;      //用户id
    QSettings  *m_config;
    QTimer     *m_timer;
    QPixmap     m_waiting;
    //手动输入用户标记，设置该标记的原因是判断是不是手动输入用户，
    //如果输入了无法登录的用户，就会一直输出密码错误信息
    bool        isManual;
    //密码错误标记，设置该标志的原因是，在有生物识别模块的情况下用户选择了密码登录，输入了错误的密码，
    //此时应该直接进入密码登录，而不是再次进入登录生物识别设备选择界面
    bool        isPasswordError;

    // UI
    QPushButton *m_backLabel;         //返回用户列表
    QPushButton *m_sessionLabel;      //session图标
    QLabel      *m_faceLabel;         //头像
    QLabel      *m_nameLabel;         //用户名
    QLabel      *m_isLoginLabel;      //提示是否已登录
    QVector<QLabel*> m_messageLabels;      //提示信息
    IconEdit   *m_passwordEdit;       //密码输入框
    BioDeviceView *bioDeviceView;      //生物识别设备列表窗口
    QPushButton *bioButton;
};

#endif // LOGINWINDOW_H
