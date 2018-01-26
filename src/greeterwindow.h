#ifndef GREETERWINDOW_H
#define GREETERWINDOW_H

#include <QWidget>
#include <QStackedLayout>
#include <QSharedPointer>
#include "userwindow.h"
#include "loginwindow.h"
#include "sessionwindow.h"
#include "usersmodel.h"
#include "greeterwrapper.h"

class GreeterWindow : public QWidget
{
    Q_OBJECT

public:
    GreeterWindow(QWidget *parent = 0);
    ~GreeterWindow();
    void initUI();
    bool eventFilter(QObject *, QEvent *);

private slots:
    void onLoggedIn(const QModelIndex &);
    void onBacktoUsers();
    void onBacktoLogin();
    void onSelectSession(const QString&);
    void timedAutologin();

private:
    UserWindow      *m_userWnd;
    LoginWindow     *m_loginWnd;
    SessionWindow   *m_sessionWnd;
    QStackedLayout  *m_layout;
    QWidget         *m_firstWnd;
    QWidget         *m_secondWnd;
    QWidget         *m_thirdWnd;
    QLabel          *m_keyboardLB;
    QLabel          *m_powerLB;
    QSharedPointer<UsersModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
};

#endif // GREETERWINDOW_H
