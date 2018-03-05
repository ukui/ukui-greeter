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
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QProcess>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QStandardPaths>
#include <QScreen>
#include <QLightDM/SessionsModel>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "globalv.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "usersmodel.h"
#include "powerwindow.h"

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent),
      m_userWnd(nullptr),
      m_loginWnd(nullptr),
      m_sessionWnd(nullptr),
      m_powerWnd(nullptr),
      m_languageMenu(nullptr),
      m_board(nullptr),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions)),
      m_greeter(new GreeterWrapper())
{
    m_usersModel = QSharedPointer<UsersModel>(new UsersModel(m_greeter->hideUsersHint()));
    if(m_greeter->hasGuestAccountHint()){    //允许游客登录
        qDebug() << "allow guest";
        m_usersModel->setShowGuest(true);
    }
    if(m_greeter->showManualLoginHint()) {    //允许手动输入用户名
        qDebug() << "allow manual login";
        m_usersModel->setShowManualLogin(true);
    }

    connect(m_greeter.data(), SIGNAL(autologinTimerExpired()),this, SLOT(timedAutologin()));
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
        m_userWnd = new UserWindow(this);
        m_userWnd->setModel(m_usersModel);
        QRect userRect((rect().width()-m_userWnd->width())/2, (rect().height()-m_userWnd->height())/2,
                       m_userWnd->width(), m_userWnd->height());
        m_userWnd->setGeometry(userRect);
        connect(m_userWnd, SIGNAL(selectedChanged(QModelIndex)), this, SLOT(onSelectedUserChanged(QModelIndex)));
    }

    //登录窗口
    m_loginWnd = new LoginWindow(m_greeter, this);
    m_loginWnd->setUsersModel(m_usersModel);
    QRect loginRect((rect().width()-m_loginWnd->width())/2, (rect().height()-m_loginWnd->height())/2,
                    m_loginWnd->width(), m_loginWnd->height());
    m_loginWnd->setGeometry(loginRect);
    if(m_usersModel->rowCount() > 1)    //如果显示了用户选择窗口，则先隐藏登录窗口
        m_loginWnd->hide();
    connect(m_loginWnd, SIGNAL(back()), this, SLOT(onBacktoUsers()));
    connect(m_loginWnd, SIGNAL(selectSession(QString)), this, SLOT(onSelectSession(QString)));
    connect(m_loginWnd, SIGNAL(authenticationSuccess()), this, SLOT(startTransparent()));

    //语言选择
    m_languageLB = new QLabel(this);
    m_languageLB->setGeometry(this->width() - 180, 20, 39, 39);
    m_languageLB->setAlignment(Qt::AlignCenter);
    m_languageLB->setFont(QFont("Ubuntu", 16));
    QPalette plt;
    plt.setColor(QPalette::WindowText, QColor(255, 255, 255, 200));
    m_languageLB->setPalette(plt);
    m_languageLB->installEventFilter(this);
    m_languageLB->setStyleSheet("QLabel::hover{background-color:rgb(255, 255, 255, 50)}");
    QString defaultLanguage = qgetenv("LANG").constData();
    if(defaultLanguage.contains("zh_CN")) {
        m_languageLB->setText(tr("Zh"));
        m_greeter->setLang("zh_CN");
    } else {
        m_languageLB->setText(tr("En"));
        m_greeter->setLang("en_US");
    }

    //虚拟键盘
    m_keyboardLB = new QLabel(this);
    m_keyboardLB->setGeometry(this->width() - 120, 20, 39, 39);
    m_keyboardLB->setPixmap(QPixmap(":/resource/keyboard.png"));
    m_keyboardLB->installEventFilter(this);
    m_keyboardLB->setStyleSheet("QLabel::hover{background-color:rgb(255, 255, 255, 50)}");

    //电源
    m_powerLB = new QLabel(this);
    m_powerLB->setGeometry(this->width() - 60, 20, 39, 39);
    m_powerLB->setPixmap(QPixmap(":/resource/power.png"));
    m_powerLB->installEventFilter(this);
    m_powerLB->setStyleSheet("QLabel::hover{background-color:rgb(255, 255, 255, 50)}");
}

bool GreeterWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_keyboardLB) {
        //虚拟键盘
        if(event->type() == QEvent::MouseButtonRelease) {
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                showBoard();
                return true;
            }
        }
    } else if(obj == m_powerLB) {
        //电源对话框
        if(event->type() == QEvent::MouseButtonRelease) {
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                showPowerWnd();
                return true;
            }
        }
    } else if(obj == m_languageLB) {
        //选择语言
        if(event->type() == QEvent::MouseButtonRelease) {
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                showLanguageMenu();
                return true;
            }
        }
    } else if(obj == m_languageMenu) {
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
    //打开了电源对话框，点击对话框之外的地方关闭对话框
    if(m_powerWnd && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        if(!m_powerWnd->geometry().contains(pos)) {
            m_powerWnd->close();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

//void GreeterWindow::keyReleaseEvent(QKeyEvent *e)
//{
//    if(e->key() == Qt::Key_Escape){
//        qDebug() << "escape";
//    }
//}


void GreeterWindow::onSelectedUserChanged(const QModelIndex &index)
{

    QString name = index.data().toString();
    if(name == tr("Guest")){
        //直接登录
    }
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
    m_userWnd->hide();
    m_loginWnd->hide();
    if(m_sessionWnd) {
        m_sessionWnd->hide();
    }

    switch (index) {
    case 0:
        m_userWnd->show();
        break;
    case 1:
        m_loginWnd->show();
        break;
    case 2:
        m_sessionWnd->show();
        m_sessionWnd->setFocus();
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
    //创建一个黑色透明背景的窗口
    m_blackbgWnd = new QWidget(this);
    m_blackbgWnd->setAttribute(Qt::WA_DeleteOnClose);
    QPalette plt;
    plt.setColor(QPalette::Background, QColor(0, 0, 0, 150));
    m_blackbgWnd->setAutoFillBackground(true);
    m_blackbgWnd->setPalette(plt);
    m_blackbgWnd->setGeometry(0, 0, width(), height());
    m_blackbgWnd->show();

    //判断是否已经有用户登录
    bool hasOpenSessions = false;
    for(int i = 0; i < m_usersModel->rowCount(); i++) {
        hasOpenSessions = m_usersModel->index(i, 0).data(QLightDM::UsersModel::LoggedInRole).toBool();
        if(hasOpenSessions)
            break;
    }
    m_powerWnd = new PowerWindow(hasOpenSessions, this);
    connect(m_powerWnd, SIGNAL(aboutToClose()), m_blackbgWnd, SLOT(close()));
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
        QAction *m_en = new QAction(tr("English"), this);
        m_en->setCheckable(true);
        QAction *m_zh = new QAction(tr("Chinese"), this);
        m_zh->setCheckable(true);
        QActionGroup *actgroup = new QActionGroup(this);
        actgroup->addAction(m_en);
        actgroup->addAction(m_zh);
        actgroup->setExclusive(true);
        m_languageMenu = new QMenu(this);
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
        QString program(QStandardPaths::findExecutable("onboard"));
        QStringList arg{"--xid"};
        m_board->setArguments(arg);
        m_board->start(program);
    }

    if(m_board->state() == QProcess::Running){
        QDBusInterface iface("org.onboard.Onboard", "/org/onboard/Onboard/Keyboard",
                             "org.onboard.Onboard.Keyboard", QDBusConnection::sessionBus());
        if(!iface.isValid()){
            qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        }
        iface.call("ToggleVisible");
    }

    //设置虚拟键盘窗口的鼠标指针
    XDefineCursor(QX11Info::display(), QX11Info::appRootWindow(), XCreateFontCursor(QX11Info::display(), XC_arrow));
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
#include <QThread>
void GreeterWindow::setRootImage()
{
    qDebug() << "setRootImage";
    XDefineCursor(QX11Info::display(), QX11Info::appRootWindow(), XCreateFontCursor(QX11Info::display(), XC_circle));
//    QPixmap pix = QApplication::primaryScreen()->grabWindow(winId());
//    QProcess process;
//    process.start(QStandardPaths::findExecutable("kylin-greeter-rootimage"), QIODevice::WriteOnly);
//    pix.save(&process, "xpm"); //write pixmap to rootimage
//    process.closeWriteChannel();
//    if(process.waitForFinished())
        m_greeter->startSession();
}

#include <QPropertyAnimation>
void GreeterWindow::startTransparent()
{
    qDebug() << "startTransparent";
    QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
    connect(animation, &QPropertyAnimation::finished, this, &GreeterWindow::setRootImage);
    animation->setDuration(1);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);

    animation->start();
}
#include <QGraphicsOpacityEffect>
void GreeterWindow::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    QGraphicsOpacityEffect  *pGraphicsOpacityEffect = new QGraphicsOpacityEffect;
    pGraphicsOpacityEffect->setOpacity(m_opacity);
    this->setGraphicsEffect(pGraphicsOpacityEffect);
}
