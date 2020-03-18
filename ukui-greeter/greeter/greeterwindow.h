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
#include "loginwindow.h"
#include "sessionwindow.h"
#include "usersmodel.h"
#include "greeterwrapper.h"

class UsersView;
class PowerManager;
class QProcess;
class QGraphicsOpacityEffect;
class Configuration;
class VirtualKeyboard;
class LanguageWidget;
class Language;
class GreeterWindow : public QWidget
{
    Q_OBJECT
public:
    GreeterWindow(QWidget *parent = 0);
    void initUI();

protected:
    void resizeEvent(QResizeEvent *);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void onUserSelected(const QModelIndex &);
    void onCurrentUserChanged(const QModelIndex&);
    void timedAutologin();
    void showPowerWnd();
    void showVirtualKeyboard();
    void onLanguageChanged(const Language &language);
    void showSessionWnd();
    void onSessionChanged(const QString &session);
    void onUserChangedByManual(const QString &userName);
    void showLanguageWnd();
    void setWindowVisible();

private:
    void switchWnd(int);
    void setBackground(const QModelIndex &index);
    void setVirkeyboardPos();
    void setWindowPos(QWidget *widget, Qt::Alignment align = Qt::AlignCenter);
    bool sessionIsValid(const QString &session);
    void updateLanguage(QString userName);
    void updateSession(QString userName);
    void setUserWindowVisible(bool visible);

    UsersView               *m_userWnd;
    LoginWindow             *m_loginWnd;
    SessionWindow           *m_sessionWnd;
    PowerManager             *m_powerWnd;
    QPushButton             *m_keyboardLB;
    QPushButton             *m_powerLB;
    QPushButton             *m_sessionLB;
    QPushButton             *m_languageLB;
    VirtualKeyboard         *m_virtualKeyboard;
    LanguageWidget          *m_languageWnd;

    GreeterWrapper          *m_greeter;
    UsersModel              *m_usersModel;
    QAbstractItemModel      *m_sessionsModel;
    Configuration           *m_configuration;
    bool                     m_languageHasChanged;
    bool                     m_sessionHasChanged;

    QTimer 		    *timer;
    QWidget	    	    *widgetTime;
    QLabel		    *lblDate;
    QLabel		    *lblTime;
};

#endif // GREETERWINDOW_H
