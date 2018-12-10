/* greeterwindow.cpp
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
#include "greeterwindow.h"
#include <QMenu>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QResizeEvent>
#include <QWindow>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>
#include <QLightDM/SessionsModel>
#include "globalv.h"
#include "loginwindow.h"
#include "usersview.h"
#include "usersmodel.h"
#include "powerwindow.h"
#include "common/configuration.h"
#include "mainwindow.h"
#include "virtualkeyboard.h"
#include "languagewidget.h"
#include "language.h"

float scale;
int fontSize;
GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent),
      m_userWnd(nullptr),
      m_loginWnd(nullptr),
      m_sessionWnd(nullptr),
      m_powerWnd(nullptr),
      m_sessionLB(nullptr),
      m_virtualKeyboard(nullptr),
      m_languageWnd(nullptr),
      m_greeter(new GreeterWrapper()),
      m_usersModel(new UsersModel(m_greeter->hideUsersHint())),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions)),
      m_configuration(Configuration::instance()),
      m_languageHasChanged(false),
      m_sessionHasChanged(false)
{
    if(m_greeter->hasGuestAccountHint()){    //允许游客登录
        qDebug() << "allow guest";
        m_usersModel->setShowGuest(true);
    }
    if(m_greeter->showManualLoginHint()) {    //允许手动输入用户名
        qDebug() << "allow manual login";
        m_usersModel->setShowManualLogin(true);
    }

    for(int i = 0; i < m_usersModel->rowCount(); i++)
        qDebug() << "load user " << m_usersModel->index(i).
                    data(QLightDM::UsersModel::NameRole).toString();

    for(int i = 0; i < m_sessionsModel->rowCount(); i++)
        qDebug() << "load session " << m_sessionsModel->index(i, 0).
                    data(QLightDM::SessionsModel::KeyRole).toString();

    connect(m_greeter, SIGNAL(autologinTimerExpired()),this, SLOT(timedAutologin()));
    connect(m_greeter, SIGNAL(authenticationSucess()), this, SLOT(hide()));
    connect(m_greeter, SIGNAL(startSessionFailed()), this, SLOT(show()));
    installEventFilter(this);
}

void GreeterWindow::initUI()
{
    //电源对话框打开按钮
    m_powerLB = new QPushButton(this);
    m_powerLB->setObjectName(QStringLiteral("powerButton"));
    m_powerLB->setIcon(QPixmap(":/resource/power.png"));
    m_powerLB->setIconSize(QSize(39, 39));
    m_powerLB->setFocusPolicy(Qt::NoFocus);
    m_powerLB->setFixedSize(39, 39);
    connect(m_powerLB, &QPushButton::clicked, this, &GreeterWindow::showPowerWnd);

    //虚拟键盘启动按钮
    m_keyboardLB = new QPushButton(this);
    m_keyboardLB->setObjectName(QStringLiteral("keyboardButton"));
    m_keyboardLB->setIcon(QPixmap(":/resource/keyboard.png"));
    m_keyboardLB->setIconSize(QSize(39, 39));
    m_keyboardLB->setFocusPolicy(Qt::NoFocus);
    m_keyboardLB->setFixedSize(39, 39);
    connect(m_keyboardLB, &QPushButton::clicked,
            this, &GreeterWindow::showVirtualKeyboard);

    //桌面环境选择按钮
    if(m_sessionsModel->rowCount() > 1)
    {
        m_sessionLB = new QPushButton(this);
        m_sessionLB->setObjectName((QStringLiteral("sessionButton")));
        m_sessionLB->setIconSize(QSize(39, 39));
        m_sessionLB->setFocusPolicy(Qt::NoFocus);
        m_sessionLB->setFixedSize(39, 39);
        connect(m_sessionLB, &QPushButton::clicked, this, &GreeterWindow::showSessionWnd);
    }

    //语言选择按钮
    m_languageLB = new QPushButton(this);
    m_languageLB->setObjectName(QStringLiteral("languageButton"));
    m_languageLB->setFocusPolicy(Qt::NoFocus);
    m_languageLB->setFont(QFont("Ubuntu", 16));
    m_languageLB->setFixedHeight(39);
    QString defaultLanguage = qgetenv("LANG").constData();
    onLanguageChanged(getLanguage(defaultLanguage));
    connect(m_languageLB, &QPushButton::clicked, this, &GreeterWindow::showLanguageWnd);  

    //登录窗口
    m_loginWnd = new LoginWindow(m_greeter, this);
    if(m_usersModel->rowCount() > 1)    //如果显示了用户选择窗口，则先隐藏登录窗口
        m_loginWnd->hide();
    connect(m_loginWnd, SIGNAL(back()), this, SLOT(onBacktoUsers()));
    connect(m_loginWnd, &LoginWindow::userChangedByManual,
            this, &GreeterWindow::onUserChangedByManual);

    // 如果只用一个用户的话，直接进入登录界面，否则显示用户列表窗口
    if(m_usersModel->rowCount() > 1) {
        m_userWnd = new UsersView(this);
        connect(m_userWnd, &UsersView::currentUserChanged, this, &GreeterWindow::onCurrentUserChanged);
        connect(m_userWnd, &UsersView::userSelected, this, &GreeterWindow::onUserSelected);

        m_userWnd->setModel(m_usersModel);

        //显示lightdm传过来的被选中的用户 -- SwitchToUser()
        QString selectedUser = m_greeter->selectUserHint();
        if(!selectedUser.isEmpty())
        {
            qDebug() << "SelectUserHint: " << selectedUser;
            m_userWnd->setCurrentUser(selectedUser);
        }

        // SwitchToGuest()
        if(m_greeter->selectGuestHint())
        {
            qDebug() << "SelectGuest";
            m_userWnd->setCurrentUser("*guest");
        }
    } else {
        setBackground(m_usersModel->index(0, 0));
    }
}

/**
 * @brief GreeterWindow::resizeEvent
 * @param event
 * GreeterWindow的大小发生改变
 */
void GreeterWindow::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    //重新计算缩放比例
    scale = QString::number(size.width() / 1920.0, 'f', 1).toFloat();
    scale = scale > 0.5 ? scale : (width() >= 800 ? 0.5 : scale);

    //字体大小
    fontSize = scale > 0.5 ? 10 : 8;

    qDebug() << "GreeterWindow resize to " << size;
    qDebug() << "scale: " << scale;

    if(m_userWnd){
        m_userWnd->resize(USERSVIEW_WIDTH, USERSVIEW_HEIGHT);
        QRect userRect((width()-m_userWnd->width())/2,
                       (height()-m_userWnd->height())/2,
                       m_userWnd->width(), m_userWnd->height());
        m_userWnd->setGeometry(userRect);
    }
    if(m_loginWnd){
        QRect loginRect((width()-m_loginWnd->width())/2, 0,
                        m_loginWnd->width(), height());
        m_loginWnd->setGeometry(loginRect);
    }

    if(m_languageWnd)
    {
        QRect languageWndRect((rect().width()-m_languageWnd->width())/2,
                              (height()-m_languageWnd->height())/2,
                              m_languageWnd->width(), m_languageWnd->height());
        m_languageWnd->setGeometry(languageWndRect);
    }

    //电源按钮位置
    int x = m_powerLB->geometry().width() + 20;
    int y = 20;
    m_powerLB->move(this->width() - x, y);

    //虚拟键盘按钮位置
    x += (20 + m_keyboardLB->width());
    m_keyboardLB->move(this->width() - x, y);

    //桌面环境选择按钮位置
    if(m_sessionLB)
    {
        x += (20 + m_sessionLB->width());
        m_sessionLB->move(this->width() - x, y);
    }

    //语言选择按钮位置
    x += (20 + m_languageLB->width());
    m_languageLB->move(this->width() - x, y);

    //虚拟键盘位置
    setVirkeyboardPos();
}

void GreeterWindow::setVirkeyboardPos()
{
    if(m_virtualKeyboard)
    {
        m_virtualKeyboard->setGeometry((width() - m_virtualKeyboard->width()) / 2,
                                       height() - m_virtualKeyboard->height(),
                                       m_virtualKeyboard->width(), m_virtualKeyboard->height());
    }
}

void GreeterWindow::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_L:
        if(e->modifiers() & Qt::ControlModifier)
            m_languageLB->click();
    break;
    case Qt::Key_K:
        if(e->modifiers() & Qt::ControlModifier)
            m_keyboardLB->click();
    break;
    case Qt::Key_P:
        if(e->modifiers() & Qt::ControlModifier)
            m_powerLB->click();
    break;
    case Qt::Key_Escape:
        if(m_powerWnd && !m_powerWnd->isHidden())
            m_powerWnd->close();
        else if(m_loginWnd && !m_loginWnd->isHidden()){
            m_loginWnd->reset();
            switchWnd(0);
        }
    break;
    }
    QWidget::keyReleaseEvent(e);
}

void GreeterWindow::setBackground(const QModelIndex &index)
{
    QString backgroundPath = index.data(QLightDM::UsersModel::BackgroundPathRole).toString();
    QSharedPointer<Background> background(new Background);
    background->type = BACKGROUND_IMAGE;
    background->image = backgroundPath;
    static_cast<MainWindow*>(parentWidget())->setBackground(background);
}

void GreeterWindow::onUserSelected(const QModelIndex &index)
{
    qDebug() << index.data(QLightDM::UsersModel::NameRole).toString() << "selected";
    m_loginWnd->setUserIndex(index);

    switchWnd(1);
}

void GreeterWindow::onCurrentUserChanged(const QModelIndex &index)
{
    setBackground(index);

    if(!m_languageHasChanged)
    {
        //获取用户的session语言
        QString language;
        QString realName = index.data(QLightDM::UsersModel::NameRole).toString();
        if(realName == "*guest" || realName == "*login")
            return;
        QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                             "org.freedesktop.Accounts",QDBusConnection::systemBus());
        QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserByName", realName);
        if(!userPath.isValid())
            qWarning() << "Get UserPath error:" << userPath.error();
        else {
            QDBusInterface userIface("org.freedesktop.Accounts", userPath.value().path(),
                                     "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
            QDBusReply<QDBusVariant> languageReply = userIface.call("Get", "org.freedesktop.Accounts.User", "Language");
            if(!languageReply.isValid())
                qWarning() << "Get User's language error" << languageReply.error();
            else {
                language = languageReply.value().variant().toString();
                onLanguageChanged(getLanguage(language));
            }
        }
    }

    if(!m_sessionHasChanged && m_sessionsModel->rowCount() > 1)
    {
        QString session = index.data(QLightDM::UsersModel::SessionRole).toString();
        onSessionChanged(session);
    }
}

void GreeterWindow::onUserChangedByManual(const QString &userName)
{
    m_userWnd->setCurrentUser(userName);
}

void GreeterWindow::onBacktoUsers()
{
    switchWnd(0);
}

void GreeterWindow::onBacktoLogin()
{
    switchWnd(1);
}

/**
 * @brief GreeterWindow::switchWnd
 * @param index （0：用户列表窗口；1：登录窗口；2：session选择窗口）
 * 切换窗口
 */
void GreeterWindow::switchWnd(int index)
{
    if(m_userWnd)
        m_userWnd->hide();
    if(m_loginWnd)
        m_loginWnd->hide();

    switch (index) {
    case 0:
        m_userWnd->show();
        break;
    case 1:
        m_loginWnd->show();
        break;
    default:
        break;
    }
}

/**
 * @brief GreeterWindow::showPowerWnd
 * 显示电源对话框
 */
void GreeterWindow::showPowerWnd()
{
    //如果已经打开了电源对话框则关闭
    if(m_powerWnd && !m_powerWnd->isHidden()){
        m_powerWnd->close();
        return;
    }
    //判断是否已经有用户登录
    bool hasOpenSessions = false;
    for(int i = 0; i < m_usersModel->rowCount(); i++) {
        hasOpenSessions = m_usersModel->index(i, 0).data(QLightDM::UsersModel::LoggedInRole).toBool();
        if(hasOpenSessions)
            break;
    }
    m_powerWnd = new PowerWindow(hasOpenSessions, this);
    m_powerWnd->setObjectName(QStringLiteral("powerWnd"));
    m_powerWnd->show();
}

/**
 * @brief GreeterWindow::showVirtualKeyboard
 * 显示或隐藏虚拟键盘
 */
void GreeterWindow::showVirtualKeyboard()
{
    if(!m_virtualKeyboard)
    {
        m_virtualKeyboard = new VirtualKeyboard(this);
        connect(m_virtualKeyboard, &VirtualKeyboard::aboutToClose,
                m_virtualKeyboard, &VirtualKeyboard::close);
    }
    m_virtualKeyboard->setVisible(!m_virtualKeyboard->isVisible());
    setVirkeyboardPos();
}

/**
 * @brief GreeterWindow::timedAutologin
 * 处理延时自动登录
 */
void GreeterWindow::timedAutologin()
{
    if(m_greeter->isAuthenticated()) {
        if(!m_greeter->authenticationUser().isEmpty())
            m_greeter->startSession();
        else if(m_greeter->autologinGuestHint())
            m_greeter->startSession();
        else if(!m_greeter->autologinUserHint().isEmpty())
            m_greeter->authenticate(m_greeter->autologinUserHint());
    }
    else
        m_greeter->authenticateAutologin();
}

/**
 * @brief GreeterWindow::showLanguageWnd
 * 显示语言选择窗口
 */
void GreeterWindow::showLanguageWnd()
{
    m_languageWnd = new LanguageWidget(this);
    m_languageWnd->setObjectName("languageWnd");

    connect(m_languageWnd, &LanguageWidget::languageChanged,
            this, &GreeterWindow::onLanguageChanged);

    m_languageHasChanged = true;

    m_languageWnd->show();
    m_languageWnd->setCurrentLanguage(m_greeter->lang());
}

void GreeterWindow::setWindowPos(QWidget *widget, Qt::Alignment align)
{
    QRect rect;
    rect.setSize(widget->size());

    if(align & Qt::AlignHCenter)
    {
        rect.setLeft((width() - widget->width())/2);
    }
    if(align & Qt::AlignVCenter)
    {
        rect.setTop((height() - widget->height())/2);
    }
    if(align & Qt::AlignBottom)
    {
        rect.setTop(height() - widget->height());
    }
    widget->setGeometry(rect);
}

void GreeterWindow::onLanguageChanged(const Language &language)
{
    if(m_greeter->lang() == language.code)
    {
        return;
    }

    m_greeter->setLang(language.code);

    m_languageLB->setText(language.name);

    const QFont &font = m_languageLB->font();
    QFontMetrics fm(font);
    int pixelWidth = fm.width(language.name);
    m_languageLB->setFixedWidth(pixelWidth + 4);

    int x;
    if(m_sessionLB)
    {
        x = m_sessionLB->geometry().left();
    }
    else
    {
        x = m_keyboardLB->geometry().left();
    }
    m_languageLB->move(x - 20 - m_languageLB->width(), 20);

    if(m_userWnd && !m_userWnd->isHidden())
    {
        m_userWnd->setFocus();
    }
}

void GreeterWindow::showSessionWnd()
{
    m_sessionWnd = new SessionWindow(m_sessionsModel, this);
    connect(m_sessionWnd, &SessionWindow::sessionChanged,
            this, &GreeterWindow::onSessionChanged);

    m_sessionHasChanged = true;

    m_sessionWnd->setCurrentSession(m_greeter->session());
    m_sessionWnd->show();
}

void GreeterWindow::onSessionChanged(const QString &session)
{
    qDebug() << "session changed: " << session;
    QString sessionTmp(session);
    if(session.isEmpty() || !sessionIsValid(session))
    {   //session不存在
        QString defaultSession = m_greeter->defaultSessionHint();

        //默认session存在
        if(defaultSession != session && sessionIsValid(defaultSession))
        {
            sessionTmp = defaultSession;
        }
        //列表中的第一个session
        else if(m_sessionsModel && m_sessionsModel->rowCount() > 0)
        {
            sessionTmp = m_sessionsModel->index(0, 0)
                    .data(QLightDM::SessionsModel::KeyRole).toString();
        }
        else
        {
            sessionTmp = "";
        }
    }

    QString sessionPrefix = sessionTmp.left(sessionTmp.indexOf('-'));
    QString sessionIcon = IMAGE_DIR + QString("badges/%1_badge.png")
            .arg(sessionPrefix.toLower());
    QFile iconFile(sessionIcon);
    if(!iconFile.exists()){
        sessionIcon = IMAGE_DIR + QString("badges/unknown_badge.png");
    }
    qDebug() << sessionIcon;
    m_sessionLB->setIcon(QIcon(sessionIcon));
    m_greeter->setSession(sessionTmp);

    if(m_userWnd && !m_userWnd->isHidden())
    {
        m_userWnd->setFocus();
    }
}

bool GreeterWindow::sessionIsValid(const QString &session)
{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++)
    {
        QString sessionKey = m_sessionsModel->index(i, 0)
                .data(QLightDM::SessionsModel::KeyRole).toString();
        if(sessionKey == session)
            return true;
    }
    return false;
}
