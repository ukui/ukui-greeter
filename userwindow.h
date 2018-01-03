#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLightDM/UsersModel>
#include "pagelistview.h"

class UserWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UserWindow(QWidget *parent = 0);

    void initUI();

protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void onSwitchPage(int);
    void onLoggedIn(const QString &);

signals:
    void loggedIn(const QString &name);

private:
//    QLightDM::UsersModel *m_model;
//    QVBoxLayout *m_vlayout;
//    QHBoxLayout *m_hlayout;
    QLabel      *m_pageIndicator;
    QLabel      *m_prevLabel;
    QLabel      *m_nextLabel;
    PageListView *m_userList;
};

#endif // USERWINDOW_H
