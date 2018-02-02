#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSvgWidget>
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

    void addMessage(const QString &);
    void setUsersModel(QSharedPointer<QAbstractItemModel> model);
    bool setUserIndex(const QModelIndex& index);
    void setSessionsModel(QSharedPointer<QAbstractItemModel> model);
    bool setSessionIndex(const QModelIndex& index);
    void setGreeter(QSharedPointer<GreeterWrapper> greeter);
private:
    void initUI();
    void setSession(QString);
    int sessionIndex(const QString &session);
    void saveLastLoginUser();

protected:
    bool eventFilter(QObject *, QEvent *);
    void showEvent(QShowEvent *);

signals:
    void back();
    void selectSession(const QString& );

public slots:
    void startAuthenticate(const QString& username);
//    void cancelAuthenticate();
    void startSession();
    void saveRootImage();
    void startWaiting();
    void stopWaiting();
    void updatePixmap();
    void onSessionSelected(const QString&);
    void onShowMessage(QString text, QLightDM::Greeter::MessageType type);
    void onShowPrompt(QString text, QLightDM::Greeter::PromptType type);
    void onAuthenticationComplete();
    void onAutologinTimerExpired();
    void onReset();
    void onLogin(const QString &str);

private:
    QSharedPointer<QAbstractItemModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
    QString     m_session;
    QSettings   *m_config;
    QTimer      *m_timer;
    QPixmap     m_waiting;

    QLabel      *m_backLabel;         //返回用户列表
    QLabel      *m_faceLabel;         //头像
    QSvgWidget  *m_sessionLabel;      //session图标
    QLabel      *m_nameLabel;         //用户名
    QLabel      *m_isLoginLabel;      //提示是否已登录
    QLabel      *m_messageLabel;      //提示信息
    IconEdit    *m_passwordEdit;     //密码输入框
};

#endif // LOGINWINDOW_H
