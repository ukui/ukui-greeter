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
    Q_PROPERTY(qreal opacity WRITE setOpacity)
public:
    GreeterWindow(QWidget *parent = 0);
    virtual ~GreeterWindow();
    void initUI();
    bool eventFilter(QObject *, QEvent *);
//    void keyReleaseEvent(QKeyEvent *);
    void setOpacity(qreal);

private slots:
    void onSelectedUserChanged(const QModelIndex &);
    void onBacktoUsers();
    void onBacktoLogin();
    void onSelectSession(const QString&);
    void onMenuItemClicked(QAction *action);
    void timedAutologin();
    void setRootImage();
    void startTransparent();

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
    QLabel          *m_keyboardLB;
    QLabel          *m_powerLB;
    QLabel          *m_languageLB;
    QMenu           *m_languageMenu;
    QProcess        *m_board;
    QSharedPointer<UsersModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
    qreal            m_opacity;
};

#endif // GREETERWINDOW_H
