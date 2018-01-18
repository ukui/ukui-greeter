#ifndef GREETERWINDOW_H
#define GREETERWINDOW_H

#include <QWidget>
#include <QStackedLayout>
#include <QSharedPointer>
#include "userwindow.h"
#include "loginwindow.h"
#include "usersmodel.h"
#include "greeterwrapper.h"

class GreeterWindow : public QWidget
{
    Q_OBJECT

public:
    GreeterWindow(QWidget *parent = 0);
    ~GreeterWindow();

public:
    void initUI();

private slots:
    void onLoggedIn(const QModelIndex &);
    void onBack();
private:
    UserWindow      *m_userWnd;
    LoginWindow     *m_loginWnd;
    QStackedLayout  *m_layout;
    QWidget         *m_firstWnd;
    QWidget         *m_secondWnd;
    QSharedPointer<UsersModel> m_usersModel;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    QSharedPointer<GreeterWrapper> m_greeter;
};

#endif // GREETERWINDOW_H
