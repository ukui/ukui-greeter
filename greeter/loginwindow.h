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
    void addMessage(const QString &);
    QString getPassword();
    void setSession(QString);
    QString getSession();

private:
    void initUI();    
    int sessionIndex(const QString &session);
    void saveLastLoginUser();
    void backToUsers();

protected:
    bool eventFilter(QObject *, QEvent *);
    void showEvent(QShowEvent *);
    void keyReleaseEvent(QKeyEvent *);

signals:
    void back();
    void selectSession(const QString& );

public slots:
    void startAuthentication(const QString& username);
    void startWaiting();
    void stopWaiting();
    void updatePixmap();
    void onSessionSelected(const QString&);
    void onShowMessage(QString text, QLightDM::Greeter::MessageType type);
    void onShowPrompt(QString text, QLightDM::Greeter::PromptType type);
    void onAuthenticationComplete();
    void onLogin(const QString &str);

private:
    QSharedPointer<QAbstractItemModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
    QString     m_session;  //session的标识
    QSettings   *m_config;
    QTimer      *m_timer;
    QPixmap     m_waiting;

    // UI
    QLabel      *m_backLabel;         //返回用户列表
    QLabel      *m_faceLabel;         //头像
    QWidget     *m_sessionBg;
    QSvgWidget  *m_sessionLabel;      //session图标
    QLabel      *m_nameLabel;         //用户名
    QLabel      *m_isLoginLabel;      //提示是否已登录
    QLabel      *m_messageLabel;      //提示信息
    IconEdit   *m_passwordEdit;       //密码输入框
};

#endif // LOGINWINDOW_H
