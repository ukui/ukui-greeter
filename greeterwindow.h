#ifndef GREETERWINDOW_H
#define GREETERWINDOW_H

#include <QWidget>
#include <QStackedLayout>
#include <QStackedWidget>
#include "userwindow.h"
#include "loginwindow.h"

class GreeterWindow : public QWidget
{
    Q_OBJECT

public:
    GreeterWindow(QWidget *parent = 0);
    ~GreeterWindow();

private slots:
    void onLoggedIn(const QString &);

private:
    UserWindow      *m_userWnd;
    LoginWindow     *m_loginWnd;
    QStackedLayout  *m_layout;
    QStackedWidget  *m_stackedWnd;
    QWidget         *m_firstWnd;
    QWidget         *m_secondWnd;
};

#endif // GREETERWINDOW_H
