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
#include <QApplication>
#include <QProcess>
#include <QLayout>
#include <QTranslator>
#include <QStandardPaths>
#include <QPainter>
#include <QScrollBar>
#include <QResizeEvent>
#include <QWindow>
#include <QtDBus/QDBusInterface>
#include <QtConcurrent/QtConcurrentRun>
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

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <libintl.h>

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
      m_greeter(new GreeterWrapper()),
      m_usersModel(new UsersModel(m_greeter->hideUsersHint())),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions)),
      m_configuration(Configuration::instance()),
      m_sessionHasChanged(false)
{
    scale = 1.0;
    if(m_greeter->hasGuestAccountHint()){    //允许游客登录
        qDebug() << "allow guest";
        m_usersModel->setShowGuest(true);
    }
    if(m_greeter->showManualLoginHint()) {    //允许手动输入用户名
        qDebug() << "allow manual login";
        m_usersModel->setShowManualLogin(true);
    }

    m_greeter->setSession(m_greeter->defaultSessionHint());

    connect(m_greeter, SIGNAL(autologinTimerExpired()),this, SLOT(timedAutologin()));
    connect(m_greeter, SIGNAL(authenticationComplete()),this, SLOT(onAuthenticationComplete1()));
    installEventFilter(this);
}

bool GreeterWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress && obj == this){
        if(m_powerWnd && m_powerWnd->isVisible()){
            m_powerWnd->close();
            switchWnd(1);
        }
    }
    return false;
}

void GreeterWindow::initUI()
{
    installEventFilter(this);

    widgetTime = new QWidget(this);
    QVBoxLayout *widgetlayout = new QVBoxLayout(widgetTime);
    lblDate = new QLabel(this);
    lblTime = new QLabel(this);
    //电源对话框打开按钮
    m_powerLB = new QPushButton(this);

    //虚拟键盘启动按钮
    m_keyboardLB = new QPushButton(this);

    //切换用户按钮
    m_btnSwitchUser = new QPushButton(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{
                lblTime->setText(local.toString(QDateTime::currentDateTime(),"hh:mm"));
                lblDate->setText(local.toString(QDate::currentDate(),"yyyy/MM/dd ddd"));
    });
    timer->start(1000);
    QtConcurrent::run([=](){
        local = QLocale::system().language();
        QDateTime dateTime = QDateTime::currentDateTime();
        QString strFormat = "dd.MM.yyyy, ddd MMMM d yy, hh:mm:ss.zzz, h:m:s ap";
        QString strDateTime = local.toString(dateTime, strFormat);

        widgetlayout->addWidget(lblTime);
        widgetlayout->addWidget(lblDate);

        lblTime->setText(local.toString(QDateTime::currentDateTime(),"hh:mm"));
        lblTime->setStyleSheet("QLabel{color:white; font-size: 50px;}");

        lblDate->setText(local.toString(QDate::currentDate(),"yyyy/MM/dd ddd"));
        lblDate->setStyleSheet("QLabel{color:white; font-size: 16px;}");

        widgetTime->adjustSize();

        m_powerLB->setObjectName(QStringLiteral("powerButton"));
        m_powerLB->setIcon(QPixmap(":/images/power.png"));
        m_powerLB->setIconSize(QSize(30, 30));
        m_powerLB->setFocusPolicy(Qt::NoFocus);
        m_powerLB->setFixedSize(48, 48);
        m_powerLB->setCursor(Qt::PointingHandCursor);
        m_powerLB->installEventFilter(this);
        connect(m_powerLB, &QPushButton::clicked, this, &GreeterWindow::showPowerWnd);

        m_keyboardLB->setObjectName(QStringLiteral("keyboardButton"));
        m_keyboardLB->setIcon(QPixmap(":/images/keyboard.png"));
        m_keyboardLB->setIconSize(QSize(30, 30));
        m_keyboardLB->setFocusPolicy(Qt::NoFocus);
        m_keyboardLB->setFixedSize(48, 48);
        m_keyboardLB->setCursor(Qt::PointingHandCursor);
        m_keyboardLB->installEventFilter(this);
        if(m_usersModel->rowCount() < 2){
            m_btnSwitchUser->hide();
        }
	connect(m_keyboardLB, &QPushButton::clicked,
                this, &GreeterWindow::showVirtualKeyboard);

        m_btnSwitchUser->setObjectName(QStringLiteral("btnSwitchUser"));
        m_btnSwitchUser->setIcon(QIcon(":/images/switchUser.png"));
        m_btnSwitchUser->setIconSize(QSize(24, 24));
        m_btnSwitchUser->setFocusPolicy(Qt::NoFocus);
        m_btnSwitchUser->setFixedSize(48, 48);
        m_btnSwitchUser->setCursor(Qt::PointingHandCursor);
        m_btnSwitchUser->installEventFilter(this);
        connect(m_btnSwitchUser, &QPushButton::clicked,
                this, [&]{
            if(m_powerWnd && m_powerWnd->isVisible())
                m_powerWnd->close();

            if(scrollArea->isVisible())
                switchWnd(1);
            else
                switchWnd(0);
        });
    });

    //桌面环境选择按钮
    if(m_sessionsModel->rowCount() > 1)
    {
        m_sessionLB = new QPushButton(this);
        m_sessionLB->setObjectName((QStringLiteral("sessionButton")));
        m_sessionLB->setIconSize(QSize(24, 24));
        m_sessionLB->setFocusPolicy(Qt::NoFocus);
        m_sessionLB->setFixedSize(48, 48);
        m_sessionLB->setCursor(Qt::PointingHandCursor);
        m_sessionLB->installEventFilter(this);
        m_sessionLB->setIcon(QIcon(IMAGE_DIR + QString("badges/unknown_badge.svg")));
        connect(m_sessionLB, &QPushButton::clicked, this, &GreeterWindow::showSessionWnd);
    }

    //用户列表
    scrollArea = new QScrollArea(this);
    scrollArea->setAttribute(Qt::WA_TranslucentBackground);
    scrollArea->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    scrollArea->setStyleSheet("QScrollArea {background-color:transparent;}");
    scrollArea->viewport()->setStyleSheet("background-color:transparent;");
    scrollArea->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    scrollArea->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    scrollContents = new QWidget(scrollArea);
    scrollArea->setWidget(scrollContents);

    m_userWnd = new UsersView(scrollContents);
    connect(m_userWnd, &UsersView::currentUserChanged, this, &GreeterWindow::onCurrentUserChanged);
    connect(m_userWnd, &UsersView::userSelected, this, &GreeterWindow::onUserSelected);
    scrollArea->hide();

    //登录窗口
    m_loginWnd = new LoginWindow(m_greeter, this);
    m_loginWnd->hide();
    connect(m_loginWnd, &LoginWindow::userChangedByManual,
            this, &GreeterWindow::onUserChangedByManual);

    connect(m_userWnd, &UsersView::userNotFound, m_loginWnd, &LoginWindow::setUserNotInView);

    m_userWnd->setModel(m_usersModel);
    setFocusProxy(m_loginWnd);
    QtConcurrent::run([=](){
        //显示lightdm传过来的被选中的用户且自动进入认证界面 -- SwitchToUser()
        QString selectedUser = m_greeter->selectUserHint();

        if(!selectedUser.isEmpty())
        {
            qDebug() << "SelectUserHint: " << selectedUser;
            m_userWnd->setCurrentUser(selectedUser, true);
        }
        // SwitchToGuest()且自动进入认证界面
        else if(m_greeter->selectGuestHint())
        {
            qDebug() << "SelectGuest";
            m_userWnd->setCurrentUser("*guest", true);
        }
        //如果没有可显示出来的用户，则开启手动输入用户名登录
        else if(m_usersModel->rowCount() == 0)
        {
            m_usersModel->setShowManualLogin(true);
            QString userName = m_usersModel->index(0, 0).data(QLightDM::UsersModel::NameRole).toString();
            m_userWnd->setCurrentUser(userName, true);
        }
        //如果只有一个用户，直接进入认证界面
        else if(m_usersModel->rowCount() == 1)
        {
            QString userName = m_usersModel->index(0, 0).data(QLightDM::UsersModel::NameRole).toString();
            m_userWnd->setCurrentUser(userName, true);
        }
        else
        {
            //选中上一次登录的用户，但不进入认证界面
            QString lastLoginUser = Configuration::instance()->getLastLoginUser();
            m_userWnd->setCurrentUser(lastLoginUser);
        }
        setWindowOpacity(0.5);
    });

}

void GreeterWindow::setRootWindow()
{
    m_greeter->setrootWindow();
}

void GreeterWindow::switchWnd(int index)
{
    if(m_userWnd)
        scrollArea->hide();
    if(m_loginWnd)
        m_loginWnd->hide();

    repaint();

    switch (index) {
    case 0:
        if(m_userWnd)
            scrollArea->show();
        break;
    case 1:
        if(m_loginWnd)
            m_loginWnd->show();
        break;
    default:
        break;
    }
}

QString GreeterWindow::guessBackground()
{
    QString selectedUser = m_greeter->selectUserHint();

    if(!selectedUser.isEmpty())
    {
        return m_configuration->getDefaultBackgroundName();
    }
    else if(m_greeter->selectGuestHint())
    {
        return m_configuration->getDefaultBackgroundName();
    }
    else if(m_usersModel->rowCount() == 0)
    {
        return m_configuration->getDefaultBackgroundName();
    }
    else
    {
        QString userName;
        if(m_usersModel->rowCount() == 1)
            userName = m_usersModel->index(0, 0).data(QLightDM::UsersModel::NameRole).toString();
        else
            userName = Configuration::instance()->getLastLoginUser();

        struct passwd *user = NULL;
        user = getpwnam(userName.toLatin1().data());

        if(user){
            QString   backgroundPath = getAccountBackground(user->pw_uid);
            if(backgroundPath.isEmpty())
                backgroundPath = m_configuration->getDefaultBackgroundName();

            QString userConfigurePath = m_greeter->getEnsureShareDir(userName) + "/ukui-greeter.conf";
            QFile backgroundFile(userConfigurePath);
            if(backgroundFile.exists()){
                QSettings settings(userConfigurePath,QSettings::IniFormat);
                settings.beginGroup("greeter");
                if(settings.contains("backgroundPath")){
                    QString filepath = settings.value("backgroundPath").toString();
                    if(!filepath.isEmpty() && isPicture(filepath)){
                        return filepath;
                    }
                }
            }
	    return backgroundPath;
        }
        return m_configuration->getDefaultBackgroundName();
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
    
    if(scale > 1)
        scale = 1;
    fontSize = scale > 0.5 ? 10 : 8;

    qDebug() << "GreeterWindow resize to " << size;
    qDebug() << "scale: " << scale;
//对距离进行缩放
    widgetTime->move((width()-widgetTime->geometry().width())/2, 59*scale); //距离顶部59*scale的距离

    if(scrollArea){
        QRect userRect(0.05*width(),
                       0.2*height(),
                       width()*0.9, height()*0.6);
        scrollArea->setGeometry(userRect);
    }

    if(scrollContents){
        scrollContents->setFixedWidth(scrollArea->width()-20);
    }

    if(m_userWnd){
        m_userWnd->resize(m_userWnd->getSize());
        if(m_userWnd->height() + 2*30*scale  < scrollArea->height()){
            m_userWnd->move((scrollArea->width()-m_userWnd->width())/2,(scrollArea->height() - m_userWnd->height())/2);
        }else{
            m_userWnd->move((scrollArea->width()-m_userWnd->width())/2,30*scale);
        }
        scrollContents->setFixedHeight(m_userWnd->height() + m_userWnd->y() );
    }

    if(m_loginWnd){
        //m_loginWnd->setFixedHeight(height());
        m_loginWnd->setChildrenGeometry();
        m_loginWnd->setGeometry((width()-m_loginWnd->geometry().width())/2,height()/3, \
                                m_loginWnd->width(), height()*2/3);
    }
    
    if(m_powerWnd){
    	m_powerWnd->move((width()-m_powerWnd->width())/2,(height() - m_powerWnd->height())/2);
    }

    //电源按钮位置,根据x，y的值进行计算，初始值为到右下角的距离

    int x = 39;
    int y = 66;

    if(m_powerLB){
        x+=m_powerLB->width();
        m_powerLB->move(this->width() - x,height() - y);
    }

    //虚拟键盘按钮位置
    if(m_keyboardLB){
        x += (m_keyboardLB->width() + 10); //10为间隔
        m_keyboardLB->move(this->width() - x, height() - y);
    }

    //切换用户按钮位置
    if(m_btnSwitchUser){
        x += (m_btnSwitchUser->width() + 10); //10为间隔
        m_btnSwitchUser->move(this->width() - x, height() - y);
    }

    //桌面环境选择按钮位置
    if(m_sessionLB)
    {
        x += (m_sessionLB->width() + 10); //10为间隔
        m_sessionLB->move(this->width() - x, height() - y);
    }

    //虚拟键盘位置
    setVirkeyboardPos();

}

void GreeterWindow::setVirkeyboardPos()
{
    if(m_virtualKeyboard)
    {
	    //设置屏幕键盘的高度为窗口高度的三分之一
       m_virtualKeyboard->setGeometry(0,
                                       height() - height()/3,
                                       width(), height()/3);
    }
}

void GreeterWindow::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_K:
        if(e->modifiers() & Qt::ControlModifier)
            m_keyboardLB->click();
        break;
    case Qt::Key_P:
        if(e->modifiers() & Qt::ControlModifier)
            m_powerLB->click();
        break;
    case Qt::Key_Escape:
        if(m_powerWnd && !m_powerWnd->isHidden()){
            setWindowVisible();
        }
        break;
    }
    QWidget::keyReleaseEvent(e);
}

QString GreeterWindow::getAccountBackground(uid_t uid)
{

    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());

    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserById", (qint64)uid);
    if(!userPath.isValid())
        qWarning() << "Get UserPath error:" << userPath.error();
    else {
        QDBusInterface userIface("org.freedesktop.Accounts", userPath.value().path(),
                                 "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> backgroundReply = userIface.call("Get", "org.freedesktop.Accounts.User", "BackgroundFile");
        if(backgroundReply.isValid())
            return  backgroundReply.value().variant().toString();
    }
    return "";
}

void GreeterWindow::setBackground(const QModelIndex &index)
{
    QString backgroundPath;
    QString accountBackground; //记录来自accountsservice的背景图片

    backgroundPath = index.data(QLightDM::UsersModel::BackgroundPathRole).toString();
    accountBackground = backgroundPath;

    if(backgroundPath.isEmpty())
    {
        uid_t uid = index.data(QLightDM::UsersModel::UidRole).toUInt();
        backgroundPath = getAccountBackground(uid);
        accountBackground = backgroundPath;
        if(backgroundPath.isEmpty())
            backgroundPath = m_configuration->getDefaultBackgroundName();
    }

    //登录后绘制桌面背景而不是登录背景
    m_greeter->setrootWindowBackground(rootWinPicture,0,backgroundPath);

    //读取/var/lib/lightdm-date/用户名/ukui-greeter.conf,
    //判断是否设置了该用户的登陆界面的背景图片.
    QString userConfigurePath = m_greeter->getEnsureShareDir(index.data(QLightDM::UsersModel::NameRole).toString()) + "/ukui-greeter.conf";
    QFile backgroundFile(userConfigurePath);
    if(backgroundFile.exists()){
        QSettings settings(userConfigurePath,QSettings::IniFormat);
        settings.beginGroup("greeter");
        if(settings.contains("backgroundPath")){
            QString filepath = settings.value("backgroundPath").toString();
            if(!filepath.isEmpty() && isPicture(filepath)){
                backgroundPath = filepath;
            }
        }
        if(accountBackground.isEmpty() && settings.contains("color")){
            QString drawBackgroundColor = settings.value("color").toString();
       	    if(drawBackgroundColor.length() == 7 && drawBackgroundColor.startsWith("#")){
            drawBackgroundColor = drawBackgroundColor.remove(0,1);
                bool ok;
                int color = drawBackgroundColor.toUInt(&ok,16);
                m_greeter->setrootWindowBackground(rootWinColor,color,NULL);
            }
        }
        settings.endGroup();
    }else{
    	QSettings settingsbak(userConfigurePath,QSettings::IniFormat);
        settingsbak.beginGroup("greeter");
        settingsbak.setValue("backgroundPath","/usr/share/backgrounds/warty-final-ubuntukylin.jpg");
        settingsbak.endGroup();
        settingsbak.sync();
        QFile file(userConfigurePath);
        file.setPermissions(QFile::WriteOther | QFile::ReadOther);
    }

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

void GreeterWindow::updateLanguage(QString userName)
{
    QString language;
    QString formatsLocale;
    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());
    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserByName", userName);
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
            if(!language.isEmpty()){
                    onLanguageChanged(language);
             }
        }

        QDBusReply<QDBusVariant> fomatsLocalReply = userIface.call("Get", "org.freedesktop.Accounts.User", "FormatsLocale");
        if(!fomatsLocalReply.isValid())
            qWarning() << "Get User's language error" << languageReply.error();
        else {
            formatsLocale = fomatsLocalReply.value().variant().toString();
            if(!formatsLocale.isEmpty()){
                if(formatsLocale.startsWith("zh")){
                    local = QLocale::Chinese;
            QString date = local.toString(QDate::currentDate(),"yyyy/MM/dd ddd");
                    lblDate->setText(date);
                }
                else{
                    local = QLocale::English;
                    QString date = local.toString(QDate::currentDate(),"yyyy/MM/dd ddd");
                    lblDate->setText(date);
        }
            }
        }

    }
}

void GreeterWindow::updateSession(QString userName)
{
    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());
    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserByName", userName);
    if(!userPath.isValid())
        qWarning() << "Get UserPath error:" << userPath.error();
    else {
        QDBusInterface userIface("org.freedesktop.Accounts", userPath.value().path(),
                                 "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> sessionReply = userIface.call("Get", "org.freedesktop.Accounts.User", "XSession");
        if(!sessionReply.isValid())
            qWarning() << "Get User's session error" << sessionReply.error();
        else {
            QString session = sessionReply.value().variant().toString();
             onSessionChanged(session);
        }
    }
}

void GreeterWindow::onCurrentUserChanged(const QModelIndex &index)
{
    for(int i = 0;i<m_userWnd->userlist.count();i++)
    {
        UserEntry *entry = m_userWnd->userlist.at(i);
          if(entry->userIndex().data(QLightDM::UsersModel::NameRole).toString() == "*login")
               entry->setUserName(tr("Login"));
    }

    setBackground(index);


    if(m_greeter->getdrawBackgroundIsStarted()){
        if(future.isStarted()&& future.isRunning()){
            future.waitForFinished();
        }else{
            future = QtConcurrent::run([=](){
                m_greeter->setrootWindow();
            });
        }
    }else{
        future = QtConcurrent::run([=](){
            m_greeter->setrootWindow();
        });
    }

    QString realName = index.data(QLightDM::UsersModel::NameRole).toString();
    bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();

    if(islogin || realName == "*login" || realName == "*guest"){
        if(m_sessionLB){
            m_sessionLB->hide();
        }
    }
    else{
        if(m_sessionLB){
            m_sessionLB->show();
        }
    }

    if(realName == "*guest" || realName == "*login")
        return;

    updateLanguage(realName);
    if(!m_sessionHasChanged && m_sessionsModel->rowCount() > 1)
    {
        QString session = index.data(QLightDM::UsersModel::SessionRole).toString();
        onSessionChanged(session);
    }

    update();
}

void GreeterWindow::onUserChangedByManual(const QString &userName)
{
    m_userWnd->setCurrentUser(userName, true);

    updateLanguage(userName);
    updateSession(userName);
}

void GreeterWindow::setWindowVisible()
{
    if(m_powerWnd && m_powerWnd->isVisible())
        m_powerWnd->close();

    switchWnd(1);
    update();
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
        switchWnd(1);
        update();
        return;
    }

    scrollArea->hide();
    m_loginWnd->hide();

    m_powerWnd = new PowerManager(this);
    m_powerWnd->adjustSize();
//    m_powerWnd->setGeometry((width()-ITEM_WIDTH*5)/2,
//                              (height()-ITEM_HEIGHT)/2,
//                              ITEM_WIDTH*5,ITEM_HEIGHT);
    m_powerWnd->setFixedSize(m_powerWnd->windowSize());
    m_powerWnd->move((width()-m_powerWnd->width())/2,(height() - m_powerWnd->height())/2);

//    connect(m_powerWnd,SIGNAL(switchToUser())
//            ,this,SLOT(setWindowVisible()));
    connect(m_powerWnd, &PowerManager::switchToUser, this,
            [this]() {
        if(m_powerWnd && m_powerWnd->isVisible())
            m_powerWnd->close();

        switchWnd(0);
    });

    m_powerWnd->setObjectName(QStringLiteral("powerWnd"));
    m_powerWnd->show();
    update();
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

void GreeterWindow::onLanguageChanged(QString languageCode)
{

    if(languageCode == "")
        return ;

    if(m_greeter->lang() == languageCode)
    {
        return;
    }

    qApp->removeTranslator(m_configuration->m_trans);
    delete m_configuration->m_trans;
    m_configuration->m_trans = new QTranslator();
    QString qmFile;

    if(languageCode.startsWith("zh")){
        qmFile = QM_DIR + QString("%1.qm").arg("zh_CN");
        setenv("LANGUAGE","zh_CN",1);
        setlocale(LC_ALL,"zh_CN.utf8");
    }
    else{
        qmFile = QM_DIR + QString("%1.qm").arg(languageCode);
        setenv("LANGUAGE",languageCode.toLatin1().data(),1);
        setlocale(LC_ALL,"");
    }
    m_configuration->m_trans->load(qmFile);
    qApp->installTranslator(m_configuration->m_trans);

    m_greeter->setLang(languageCode);

}

void GreeterWindow::showSessionWnd()
{
    if(!m_sessionWnd){
        m_sessionWnd = new SessionWindow(m_sessionsModel, this);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
        m_sessionWnd->setStyleSheet("QMenu::item{padding: 2px 10px 2px 10px;}");
#else
        m_sessionWnd->setStyleSheet("QMenu::item{padding: 2px 10px 2px 30px;}");
#endif

        connect(m_sessionWnd, &SessionWindow::sessionChanged, \
                this, &GreeterWindow::onSessionChanged);
    }

    m_sessionHasChanged = true;

    if(m_sessionWnd->isVisible()){
        m_sessionWnd->close();
        m_sessionWnd = nullptr;
        setFocus();
    }
    else{
        m_sessionWnd->move(m_sessionLB->x(),m_sessionLB->y()-m_sessionWnd->height() - 3);
        m_sessionWnd->show();
        m_sessionWnd->setFocus();
    }
    update();
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
    if(sessionTmp == "ukui-wayland")
	    sessionPrefix = "ukui_wayland";
    QString sessionIcon = IMAGE_DIR + QString("badges/%1_badge.svg")
            .arg(sessionPrefix.toLower());
    QFile iconFile(sessionIcon);
    if(!iconFile.exists()){
        sessionIcon = IMAGE_DIR + QString("badges/unknown_badge.svg");
    }
    qDebug() << sessionIcon;

    if(m_sessionLB)
        m_sessionLB->setIcon(QIcon(sessionIcon));

    m_greeter->setSession(sessionTmp);

    if(m_userWnd && !m_userWnd->isHidden())
    {
        m_userWnd->setFocus();
    }

    if(m_sessionWnd){
        m_sessionWnd->close();
        m_sessionWnd = nullptr;
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

void GreeterWindow::onAuthenticationComplete1()
{
    for(int i = 0;i<m_userWnd->userlist.count();i++)
    {
        UserEntry *entry = m_userWnd->userlist.at(i);
          if(entry->userIndex().data(QLightDM::UsersModel::NameRole).toString() == "*login")
               entry->setUserName(tr("Login"));
    }
}
