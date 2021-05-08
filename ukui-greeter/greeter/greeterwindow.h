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
#include <QTranslator>
#include <QLocale>
#include "loginwindow.h"
#include "sessionwindow.h"
#include "usersmodel.h"
#include "greeterwrapper.h"
#include "backgroundwindow.h"

class UsersView;
class PowerManager;
class QProcess;
class QGraphicsOpacityEffect;
class Configuration;
class VirtualKeyboard;
class LanguageWidget;
class Language;

extern QPixmap* blurPixmap(QPixmap *pixmap);

class GreeterWindow : public QWidget
{
    Q_OBJECT
public:
    GreeterWindow(QWidget *parent = 0);
    ~GreeterWindow();
    void initUI();

signals:
    void signalBackgroundChanged(QSharedPointer<Background> &);

protected:
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void changeEvent(QEvent *event);

    void paintEvent(QPaintEvent *);
private slots:
    void onAuthenticationComplete1();
    void onUserSelected(const QModelIndex &);
    void onCurrentUserChanged(const QModelIndex&);
    void timedAutologin();
    void showPowerWnd();
    void showVirtualKeyboard();
    void onLanguageChanged(QString languageCode);
    void showSessionWnd();
    void onSessionChanged(const QString &session);
    void onUserChangedByManual(const QString &userName);
    void setWindowVisible();

    void onTransition();

private:
    void switchWnd(int);
    void setBackground(const QModelIndex &index);
    void setVirkeyboardPos();
    void setWindowPos(QWidget *widget, Qt::Alignment align = Qt::AlignCenter);
    bool sessionIsValid(const QString &session);
    void updateLanguage(QString userName);
    void updateSession(QString userName);
    void setUserWindowVisible(bool visible);
    void refreshTranslate();
    QString getAccountBackground(uid_t uid);

    void drawBackground(QSharedPointer<Background> &, const QRect &, float alpha = 1.0);
    QPixmap * getBackground(const QString &path, const QRect &rect);
    void setBackground(QSharedPointer<Background> &);
    void initBackground();
    void stopTransition();
    void startTransition(QSharedPointer<Background> &, QSharedPointer<Background> &);


    UsersView               *m_userWnd;
    LoginWindow             *m_loginWnd;
    SessionWindow           *m_sessionWnd;
    PowerManager             *m_powerWnd;
    QPushButton             *m_keyboardLB;
    QPushButton             *m_powerLB;
    QPushButton             *m_sessionLB;
    VirtualKeyboard         *m_virtualKeyboard;

    GreeterWrapper          *m_greeter;
    UsersModel              *m_usersModel;
    QAbstractItemModel      *m_sessionsModel;
    Configuration           *m_configuration;
    bool                     m_sessionHasChanged;

    QTimer 		    *timer;
    QWidget	    	    *widgetTime;
    QLabel		    *lblDate;
    QLabel		    *lblTime;
    QLocale         local;

    QTimer          *m_timer;
    Transition       m_transition;
    QMap<QPair<QString, QString>, QPixmap*>   m_backgrounds;
    QSharedPointer<Background> m_background;
    BackgroundMode   m_backgroundMode;

};

#endif // GREETERWINDOW_H
