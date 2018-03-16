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

protected:
    void showEvent(QShowEvent *);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    void back();
    void selectSession(const QString& );
    void authenticationSuccess();

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

private:
    QSharedPointer<QAbstractItemModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
    QString     m_session;  //session的标识
    QString     m_name;     //m_nameLabel显示的是全名(显示的),m_name保存的是真名(用于登录的)
    QSettings  *m_config;
    QTimer     *m_timer;
    QPixmap     m_waiting;

    // UI
    QPushButton *m_backLabel;         //返回用户列表
    QPushButton *m_sessionLabel;      //session图标
    QLabel      *m_faceLabel;         //头像
    QLabel      *m_nameLabel;         //用户名
    QLabel      *m_isLoginLabel;      //提示是否已登录
    QVector<QLabel*> m_messageLabels;      //提示信息
    IconEdit   *m_passwordEdit;       //密码输入框
};

#endif // LOGINWINDOW_H
