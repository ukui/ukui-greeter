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

class PowerWindow;
class QProcess;
class GreeterWindow : public QWidget
{
    Q_OBJECT

public:
    GreeterWindow(QWidget *parent = 0);
    virtual ~GreeterWindow();
    void initUI();
    bool eventFilter(QObject *, QEvent *);

private slots:
    void onSelectedUserChanged(const QModelIndex &);
    void onBacktoUsers();
    void onBacktoLogin();
    void onSelectSession(const QString&);
    void timedAutologin();
    void onMenuItemClicked(QAction *action);
private:
    void switchWnd(int);
    void showPowerWnd();
    void showLanguageMenu();
    void showBoard();

    UserWindow      *m_userWnd;
    LoginWindow     *m_loginWnd;
    SessionWindow   *m_sessionWnd;
    PowerWindow     *m_powerWnd;
    QWidget         *m_blackbgWnd;
//    QStackedLayout  *m_layout;
//    QWidget         *m_firstWnd;
//    QWidget         *m_secondWnd;
//    QWidget         *m_thirdWnd;
    QLabel          *m_keyboardLB;
    QLabel          *m_powerLB;
    QLabel          *m_languageLB;
    QMenu           *m_languageMenu;
    QProcess        *m_board;
//    QAction         *m_en;
//    QAction         *m_zh;
    QSharedPointer<UsersModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
};

#endif // GREETERWINDOW_H
