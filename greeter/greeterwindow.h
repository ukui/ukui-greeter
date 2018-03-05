/* greeterwindow.h
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
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
