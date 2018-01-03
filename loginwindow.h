#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QLightDM/Greeter>
#include <QLightDM/UsersModel>
#include "iconedit.h"
#include "ui_login.h"


class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow(){}

private:
    void initUI();
public:
    void addMessage(const QString &);

signals:

public slots:

    void showMessage_cb(QString text, QLightDM::Greeter::MessageType type);
    void showPrompt_cb(QString text, QLightDM::Greeter::PromptType type);
    void authenticationComplete_cb();
    void autologinTimerExpired_cb();
    void reset_cb();
    void login_cb(const QString &str);

private:
    Ui::Form *ui;

    QLightDM::Greeter *m_greeter;
    QLightDM::UsersModel *m_usersModel;

    IconButton *iconButton;
/*
    QLabel  *m_faceLabel;         //头像
    QLabel  *m_userLabel;         //用户名
    QLabel  *m_isLoginLabel;      //提示是否已登录
    QLabel  *m_messageLabel;      //提示信息
*/
    IconEdit *m_passwordEdit;     //密码

};

#endif // LOGINWINDOW_H
