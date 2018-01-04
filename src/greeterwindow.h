#ifndef GREETERWINDOW_H
#define GREETERWINDOW_H

#include <QWidget>
#include <QStackedLayout>
#include <QStackedWidget>
#include "userwindow.h"
#include "loginwindow.h"
#include "usersmodel.h"

class GreeterWindow : public QWidget
{
    Q_OBJECT

public:
    GreeterWindow(QWidget *parent = 0);
    ~GreeterWindow();

private:
    void initUI();

private slots:
    void onLoggedIn(const QString &);
    void onBack();
private:
    UserWindow      *m_userWnd;
    LoginWindow     *m_loginWnd;
    QStackedLayout  *m_layout;
//    QStackedWidget  *m_stackedWnd;
    QWidget         *m_firstWnd;
    QWidget         *m_secondWnd;

    UsersModel *m_model;
};

#endif // GREETERWINDOW_H
