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
#include <QLightDM/SessionsModel>
#include "globalv.h"
#include "loginwindow.h"
#include "usersview.h"
#include "usersmodel.h"
#include "powerwindow.h"
#include <X11/Xlib.h>

float scale;
int fontSize;
GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent),
      m_userWnd(nullptr),
      m_loginWnd(nullptr),
      m_sessionWnd(nullptr),
      m_powerWnd(nullptr),
      m_languageMenu(nullptr),
      m_board(nullptr),
      m_greeter(new GreeterWrapper()),
      m_usersModel(new UsersModel(m_greeter->hideUsersHint())),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions))
{
    if(m_greeter->hasGuestAccountHint()){    //允许游客登录
        qDebug() << "allow guest";
        m_usersModel->setShowGuest(true);
    }
    if(m_greeter->showManualLoginHint()) {    //允许手动输入用户名
        qDebug() << "allow manual login";
        m_usersModel->setShowManualLogin(true);
    }

    connect(m_greeter, SIGNAL(autologinTimerExpired()),this, SLOT(timedAutologin()));
    connect(m_greeter, SIGNAL(authenticationSucess()), this, SLOT(hide()));
    connect(m_greeter, SIGNAL(startSessionFailed()), this, SLOT(show()));
    installEventFilter(this);
}

GreeterWindow::~GreeterWindow()
{
    //先kill虚拟键盘进程，再销毁process对象
    if(m_board && m_board->state() == QProcess::Running){
        m_board->kill();
        m_board->waitForFinished(100);
    }
    if(m_board){
        delete(m_board);
    }
}

void GreeterWindow::initUI()
{
    // 如果只用一个用户的话，直接进入登录界面，否则显示用户列表窗口
    if(m_usersModel->rowCount() > 1) {
        m_userWnd = new UsersView(this);
        m_userWnd->setModel(m_usersModel);
        connect(m_userWnd, &UsersView::userSelected, this, &GreeterWindow::onUserSelected);
    }

    //登录窗口
    m_loginWnd = new LoginWindow(m_greeter, this);
    m_loginWnd->setUsersModel(m_usersModel);
    m_loginWnd->setSessionsModel(m_sessionsModel);
    if(m_usersModel->rowCount() > 1)    //如果显示了用户选择窗口，则先隐藏登录窗口
        m_loginWnd->hide();
    connect(m_loginWnd, SIGNAL(back()), this, SLOT(onBacktoUsers()));
    connect(m_loginWnd, SIGNAL(selectSession(QString)), this, SLOT(onSelectSession(QString)));

    //语言选择按钮
    m_languageLB = new QPushButton(this);
    m_languageLB->setObjectName(QStringLiteral("languageButton"));
    m_languageLB->setMenu(m_languageMenu);
    m_languageLB->setFocusPolicy(Qt::NoFocus);
    m_languageLB->setFont(QFont("Ubuntu", 16));
    QString defaultLanguage = qgetenv("LANG").constData();
    if(defaultLanguage.contains("zh_CN")) {
        m_languageLB->setText(tr("Zh"));
        m_greeter->setLang("zh_CN");
    } else {
        m_languageLB->setText(tr("En"));
        m_greeter->setLang("en_US");
    }
    connect(m_languageLB, &QPushButton::clicked, this, &GreeterWindow::showLanguageMenu);

    //虚拟键盘启动按钮
    m_keyboardLB = new QPushButton(this);
    m_keyboardLB->setObjectName(QStringLiteral("keyboardButton"));
    m_keyboardLB->setIcon(QPixmap(":/resource/keyboard.png"));
    m_keyboardLB->setIconSize(QSize(39, 39));
    m_keyboardLB->setFocusPolicy(Qt::NoFocus);
    connect(m_keyboardLB, &QPushButton::clicked, this, &GreeterWindow::showBoard);

    //电源对话框打开按钮
    m_powerLB = new QPushButton(this);
    m_powerLB->setObjectName(QStringLiteral("powerButton"));
    m_powerLB->setIcon(QPixmap(":/resource/power.png"));
    m_powerLB->setIconSize(QSize(39, 39));
    m_powerLB->setFocusPolicy(Qt::NoFocus);
    connect(m_powerLB, &QPushButton::clicked, this, &GreeterWindow::showPowerWnd);
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
        QRect userRect((rect().width()-m_userWnd->width())/2,
                       (rect().height()-m_userWnd->height())/2,
                       m_userWnd->width(), m_userWnd->height());
        m_userWnd->setGeometry(userRect);
    }
    if(m_loginWnd){
        QRect loginRect(geometry().left() + (rect().width()-m_loginWnd->width())/2,
                        geometry().top() + (rect().height()-m_loginWnd->height())/2,
                        m_loginWnd->width(), m_loginWnd->height());
        m_loginWnd->setGeometry(loginRect);
    }

    if(m_sessionWnd){
        QRect sessionRect((rect().width()-m_sessionWnd->width())/2,
                          (rect().height()-m_sessionWnd->height())/2,
                          m_sessionWnd->width(), m_sessionWnd->height());
        m_sessionWnd->setGeometry(sessionRect);
    }

    m_languageLB->setGeometry(this->width() - 180, 20, 39, 39);
    m_keyboardLB->setGeometry(this->width() - 120, 20, 39, 39);
    m_powerLB->setGeometry(this->width() - 60, 20, 39, 39);
}

bool GreeterWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_languageMenu) {
        if(event->type() == QEvent::Close) {
            //当菜单关闭时需要重绘label，否则指针悬浮在label上产生的背景不会消失
            repaint(m_languageLB->geometry());
            return true;
        } else if(event->type() == QEvent::MouseButtonRelease) {
            //打开电源对话框后再打开语言选择菜单，单击菜单项接收不到triggered信号，
            //需要双击菜单项才会生效，所以这里直接调用trigger函数
            QAction *action = m_languageMenu->activeAction();
            if(action)
                action->trigger();
            m_languageMenu->close();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
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
        else if(m_sessionWnd && !m_sessionWnd->isHidden())
            switchWnd(1);
    break;
    }
    QWidget::keyReleaseEvent(e);
}


void GreeterWindow::onUserSelected(const QModelIndex &index)
{
    m_loginWnd->setUserIndex(index);

    switchWnd(1);
}

void GreeterWindow::onBacktoUsers()
{
    switchWnd(0);
}

void GreeterWindow::onBacktoLogin()
{
    switchWnd(1);
}

void GreeterWindow::onSelectSession(const QString &sessionName)
{
    qDebug() << "selection session";
    if(!m_sessionWnd) {
        m_sessionWnd = new SessionWindow(m_greeter->defaultSessionHint(), this);
        m_sessionWnd->setGeometry((rect().width()-m_sessionWnd->width())/2,
                                  (rect().height()-m_sessionWnd->height())/2,
                                  m_sessionWnd->width(), m_sessionWnd->height());
        connect(m_sessionWnd, SIGNAL(sessionSelected(QString)), m_loginWnd, SLOT(onSessionSelected(QString)));
        connect(m_sessionWnd, SIGNAL(back()), this, SLOT(onBacktoLogin()));
    }
    m_sessionWnd->setSession(sessionName);

    switchWnd(2);
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
    if(m_sessionWnd)
        m_sessionWnd->hide();

    switch (index) {
    case 0:
        m_userWnd->show();
        break;
    case 1:
        m_loginWnd->show();
        break;
    case 2:
        m_sessionWnd->show();
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
 * @brief GreeterWindow::showLanguageMenu
 * 显示语言选择菜单
 */
void GreeterWindow::showLanguageMenu()
{
    if(!m_languageMenu) {
        // 目前只允许language设置为中文或者英文
        m_languageMenu = new QMenu(this);

        QAction *m_en = new QAction(tr("English"), m_languageMenu);
        m_en->setCheckable(true);
        QAction *m_zh = new QAction(tr("Chinese"), m_languageMenu);
        m_zh->setCheckable(true);
        QActionGroup *actgroup = new QActionGroup(m_languageMenu);
        actgroup->addAction(m_en);
        actgroup->addAction(m_zh);
        actgroup->setExclusive(true);

        m_languageMenu->addActions(actgroup->actions());
        m_languageMenu->installEventFilter(this);
        connect(m_languageMenu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuItemClicked(QAction*)));

        // 默认选项
        if(m_greeter->lang() == "zh_CN")
            m_zh->setChecked(true);
        else
            m_en->setChecked(true);
    }
    m_languageMenu->popup(mapToGlobal(m_languageLB->geometry().bottomLeft()));
}

/**
 * @brief GreeterWindow::onMenuItemClicked
 * @param action
 * 选择语言
 */
void GreeterWindow::onMenuItemClicked(QAction *action)
{
    QString text = action->text();
    if(text == tr("English")){
        m_languageLB->setText(tr("En"));
        m_greeter->setLang("en_US");
    } else if(text == tr("Chinese")) {
        m_languageLB->setText(tr("Zh"));
        m_greeter->setLang("zh_CN");
    }
}

/**
 * @brief GreeterWindow::showBoard
 * 显示或隐藏虚拟键盘
 */
void GreeterWindow::showBoard()
{
    if(m_board == nullptr){
        m_board = new QProcess();
        m_board->start("/bin/sh -c onboard --xid");
        connect(m_board, &QProcess::readyReadStandardOutput, this, [&]{
            QByteArray output = m_board->readAllStandardOutput();
            unsigned xid = output.trimmed().toInt();
            qDebug() << "onBoard xid: " << xid;
            QWindow *boardWind = QWindow::fromWinId(xid);
            m_boardWidget = QWidget::createWindowContainer(boardWind, this, Qt::ForeignWindow);
            m_boardWidget->setFixedSize(width(), 200);
            m_boardWidget->move(0, height() - 200);
            m_boardWidget->show();
        });
    }

    if(m_board->state() == QProcess::Running){
        QDBusInterface iface("org.onboard.Onboard", "/org/onboard/Onboard/Keyboard",
                             "org.onboard.Onboard.Keyboard", QDBusConnection::sessionBus());
        if(!iface.isValid()){
            qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        }
        iface.call("ToggleVisible");
    }
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
