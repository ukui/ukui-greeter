#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QAbstractItemModel>
#include <QWeakPointer>
#include <QLightDM/Greeter>
#include <QLightDM/UsersModel>
#include "iconedit.h"

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow(){}

    void addMessage(const QString &);
//    void setEntry(const QString& name, const QString& facePath, bool islogin);
    void setModel(QAbstractItemModel *model);
    bool setIndex(int index);
private:
    void initUI();

protected:
    bool eventFilter(QObject *, QEvent *);

signals:
    void back();

public slots:

    void showMessage_cb(QString text, QLightDM::Greeter::MessageType type);
    void showPrompt_cb(QString text, QLightDM::Greeter::PromptType type);
    void authenticationComplete_cb();
    void autologinTimerExpired_cb();
    void reset_cb();
    void login_cb(const QString &str);

private:
    QLightDM::Greeter *m_greeter;
    QWeakPointer<QAbstractItemModel> m_model;

    QLabel  *m_backLabel;         //返回用户列表
    QLabel  *m_faceLabel;         //头像
    QLabel  *m_nameLabel;         //用户名
    QLabel  *m_isLoginLabel;      //提示是否已登录
    QLabel  *m_messageLabel;      //提示信息
    IconEdit *m_passwordEdit;     //密码

};

#endif // LOGINWINDOW_H
