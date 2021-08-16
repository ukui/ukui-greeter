/* loginwindow.cpp
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
#include "loginwindow.h"
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QtConcurrent/QtConcurrentRun>
#include <QKeyEvent>
#include <QGridLayout>
#include <QAbstractItemModel>
#include <QPainter>
#include <QPainterPath>

#include "greeterwrapper.h"
#include "iconedit.h"
#include "globalv.h"
#include "configuration.h"
#include "biometricproxy.h"
#include "biometricauthwidget.h"
#include "biometricdeviceswidget.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <libintl.h>
#include <locale.h>
#include "pam-tally.h"

#define _(string) gettext(string)

LoginWindow::LoginWindow(GreeterWrapper *greeter, QWidget *parent)
    : QWidget(parent),
      m_timer(nullptr),
      isLockingFlg(false),
      m_greeter(greeter)
{    
    initUI();

    pam_tally_init();  //
    connect(m_greeter, SIGNAL(showMessage(QString,QLightDM::Greeter::MessageType)),
            this, SLOT(onShowMessage(QString,QLightDM::Greeter::MessageType)));
    connect(m_greeter, SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this, SLOT(onShowPrompt(QString,QLightDM::Greeter::PromptType)));
    connect(m_greeter, SIGNAL(authenticationComplete()),
            this, SLOT(onAuthenticationComplete()));

}

QPixmap LoginWindow::PixmapToRound(const QPixmap &src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    QPixmap pixmapa(src);
    QPixmap pixmap(radius*2,radius*2);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, radius*2, radius*2);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, radius*2, radius*2, pixmapa);
    return pixmap;
}

void LoginWindow::initUI()
{
    setFixedWidth(500);

    m_userWidget = new QWidget(this);
    m_userWidget->setObjectName(QStringLiteral("userWidget"));

    /* 头像 */
    const QString SheetStyle = QString("border-radius: %1px;  border:0px   solid white;").arg(90*scale);
    m_faceLabel = new QLabel(m_userWidget);
    m_faceLabel->setObjectName(QStringLiteral("faceLabel"));
    m_faceLabel->setFocusPolicy(Qt::NoFocus);
    m_faceLabel->setStyleSheet(SheetStyle);
    m_faceLabel->setAlignment(Qt::AlignCenter);

    /* 用户名 */
    m_nameLabel = new QLabel(m_userWidget);
    m_nameLabel->setObjectName(QStringLiteral("login_nameLabel"));
    m_nameLabel->setFocusPolicy(Qt::NoFocus);
    m_nameLabel->setAlignment(Qt::AlignCenter);

    /* 密码框所在窗口 */
    m_passwdWidget = new QWidget(this);

    /* 密码框 */
    m_passwordEdit = new IconEdit(m_passwdWidget);

    /* 密码认证信息显示列表 */
    m_messageLabel = new QLabel(m_passwdWidget);

    m_messageButton = new QPushButton(m_passwdWidget);

    
    QtConcurrent::run([=](){
        m_passwdWidget->setObjectName(QStringLiteral("passwordWidget"));
        m_passwordEdit->setObjectName(QStringLiteral("passwordEdit"));
        m_messageLabel->setObjectName(QStringLiteral("messageLabel"));
        m_messageLabel->setAlignment(Qt::AlignCenter);
        m_messageButton->setObjectName(QStringLiteral("messageButton"));
        m_messageButton->hide();

        m_passwordEdit->setIcon(QIcon(":/images/login-button.png"));
        m_passwordEdit->setFocusPolicy(Qt::StrongFocus);
        m_passwordEdit->installEventFilter(this);
        m_passwordEdit->hide(); //收到请求密码的prompt才显示出来
        connect(m_passwordEdit, SIGNAL(clicked(const QString&)),
                this, SLOT(onLogin(const QString&)));

        setFocusProxy(m_passwordEdit);

    });

    isinput_passwd = false;
}

void LoginWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if(!m_passwordEdit->isHidden())
    {
        m_passwordEdit->setFocus();
    }
}

void LoginWindow::resizeEvent(QResizeEvent *)
{
    setChildrenGeometry();
}

void LoginWindow::keyReleaseEvent(QKeyEvent *event)
{
    //    qDebug() << event->key();
    if(event->key() == Qt::Key_Escape)
    {
        //退出设备选择
        if(m_biometricDevicesWidget && m_biometricDevicesWidget->isVisible())
        {
            if(authMode == BIOMETRIC)
            {
                showBiometricAuthWidget();
            }
            else
            {
                showPasswordAuthWidget();
            }
            return;
        }
    }
    QWidget::keyReleaseEvent(event);
}

void LoginWindow::refreshTranslate()
{
    if(m_biometricButton){
        m_biometricButton->setText(tr("Biometric Authentication"));
    }
    if(m_passwordButton){
        m_passwordButton->setText(tr("Password Authentication"));
    }
    if(m_otherDeviceButton){
        m_otherDeviceButton->setText(tr("Other Devices"));
    }
    if(m_retryButton){
        m_retryButton->setText(tr("Retry"));
    }

    if(m_nameLabel){
        if(m_nameLabel->text() == "Login" || m_nameLabel->text() == "登录")
            m_nameLabel->text() == tr("Login");
    }
}

void LoginWindow::setChildrenGeometry()
{
    // 用户信息显示位置
    m_userWidget->setGeometry(0, 0,
                              width(), 292*scale);

    const QString SheetStyle = QString("border-radius: %1px;  border:0px   solid white;").arg(90*scale);
    m_faceLabel->setStyleSheet(SheetStyle);

    setFace(m_face);
    m_faceLabel->setGeometry((width() - 180*scale) / 2 , 0, 180*scale, 180*scale);
    m_nameLabel->setGeometry(0, m_faceLabel->geometry().bottom() + 25*scale,
                             width(), 40);

    if(scale < 0.5)
        setFixedWidth(360);
    else
        setFixedWidth(600);


    m_passwdWidget->setGeometry(0, m_nameLabel->geometry().bottom() + 45*scale, width(), 150);
    m_passwordEdit->setGeometry((m_passwdWidget->width() - 300)/2, 0, 300, 34);
    m_messageLabel->setGeometry((m_passwdWidget->width() - 600)/2,
                                m_passwordEdit->geometry().bottom() + 25,
                                600, 25);

    m_messageButton->setGeometry((m_passwdWidget->width() - 200)/2, 0, 200, 40);

    setBiometricWidgetGeometry();
    setBiometricButtonWidgetGeometry();

}

/**
 * @brief LoginWindow::reset
 * 复原UI
 */
void LoginWindow::reset()
{
    m_passwordEdit->clear();
    m_passwordEdit->setType(QLineEdit::Password);
    m_messageButton->hide();
    clearMessage();
    showPasswordAuthWidget();
    m_deviceCount = -1;
    authMode = UNKNOWN;
    m_deviceInfo = DeviceInfoPtr();
    m_deviceName = "";
}

void LoginWindow::clearMessage()
{
    m_messageLabel->clear();
    m_preStrMessage = "";
    m_preStrMessageType = -1;
}

/**
 * @brief LoginWindow::setUserName
 * @param userName 用户名
 *
 * 设置用户名
 */
void LoginWindow::setUserName(const QString& userName)
{
    m_nameLabel->setText(userName);
}

/**
 * @brief LoginWindow::userName
 * @return 当前的用户名
 *
 * 获取当前用户名
 */
QString LoginWindow::getUserName()
{
    if(m_nameLabel->text() == tr("Login"))
        return "Login";
    return m_nameLabel->text();
}

/**
 * @brief LoginWindow::getRealName
 * @return
 * 获取用户真名
 */
QString LoginWindow::getRealName()
{
    return m_name;
}

/**
 * @brief LoginWindow::setFace
 * @param faceFile 用户头像文件
 *
 * 设置用户头像
 */
void LoginWindow::setFace(const QString& facePath)
{
    QFile faceFile(facePath);
    QPixmap faceImage;
    m_faceLabel->setFixedSize(180*scale,180*scale);
    //如果头像文件不存在，则使用默认头像
    if(faceFile.exists())
        faceImage = scaledPixmap(180*scale, 180*scale, facePath);
    else
        faceImage = scaledPixmap(180*scale, 180*scale, ":/images/default_face.png");

    faceImage =  PixmapToRound(faceImage,90*scale);
    m_faceLabel->setAlignment(Qt::AlignCenter);
    m_faceLabel->setPixmap(faceImage);

    m_face = facePath;
}

/**
 * @brief LoginWindow::setLoggedIn
 * @param isLoggedIn
 *
 * 设置当前用户是否已已经登录
 */
void LoginWindow::setLoggedIn(bool isLoggedIn)
{
    //m_isLoginLabel->setText(isLoggedIn ? tr("logged in") : "");
}

/**
 * @brief LoginWindow::setPrompt
 * @param text
 *
 * 设置密码框的提示信息
 */
void LoginWindow::setPrompt(const QString& text)
{
    m_passwordEdit->setPrompt(text);
}

/**
 * @brief LoginWindow::password
 * @return
 *
 * 获取密码
 */
QString LoginWindow::getPassword()
{
    return m_passwordEdit->text();
}


bool LoginWindow::setUserIndex(const QModelIndex& index)
{
    isinput_passwd = false;

    if(!index.isValid()){
        return false;
    }

    if(m_biometricAuthWidget){
        m_biometricAuthWidget->stopAuth();
        if(m_bioTimer && m_bioTimer->isActive())
            m_bioTimer->stop();
    }

    //设置用户名
    QString name = index.data(QLightDM::UsersModel::RealNameRole).toString();
    m_name = index.data(QLightDM::UsersModel::NameRole).toString();
    setUserName(name.isEmpty() ? m_name : name);


    //设置头像
    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    setFace(facePath);

    //显示是否已经登录
    bool isLoggedIn = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
    setLoggedIn(isLoggedIn);

    //设置生物识别设备窗口的uid
    m_uid = index.data(QLightDM::UsersModel::UidRole).toInt();
    setChildrenGeometry();

    startAuthentication();
    reset();

    //设置用户名

    return true;
}

/**
 * @brief 手动输入的用户名不在用户列表中（uid < 1000或者用户不存在）
 * @param userName
 */
void LoginWindow::setUserNotInView(const QString &userName)
{
    isloginauth = false;
    m_name = userName;
    setUserName(userName);
}

void LoginWindow::startAuthentication()
{
    prompted = false;
    unacknowledged_messages = false;
    isloginauth = false;
    //用户认证
    if(m_name == "*guest")
    {                       //游客登录
        qDebug() << "guest login";
        m_passwordEdit->show();
        setPrompt(tr("login"));
    }
    else if(m_name == "*login")
    { //手动输入用户名
        m_greeter->authenticate("");
    }
    else
    {
        qDebug() << "login: " << m_name;
        m_greeter->authenticate(m_name);
    }
}

/**
 * @brief LoginWindow::startWaiting
 *
 * 等待认证结果
 */
void LoginWindow::startWaiting()
{
    m_passwordEdit->startWaiting();   //等待认证结果期间不能再输入密码
}

void LoginWindow::stopWaiting()
{
    m_passwordEdit->stopWaiting();
}

void LoginWindow::onLogin(const QString &str)
{
    clearMessage();
    unacknowledged_messages=false;
    isloginauth = false;
    qDebug()<<m_name;
    if(m_name == "*guest")
    {
        m_greeter->authenticateAsGuest();
    }
    else if(m_name == "*login")
    {   //用户输入用户名
        isloginauth = true;
        isManual = true;
        m_name_is_login = true;

        m_greeter->respond(str);

        manualLoginName = str;

        if (!isinput_passwd){
            Q_EMIT userChangedByManual(str);
            isinput_passwd = true;
        }
    }
    else
    {  //发送密码
        m_greeter->respond(str);
        startWaiting();
        m_passwordEdit->setEnabled(false);
    }
}

void LoginWindow::onShowPrompt(QString text, QLightDM::Greeter::PromptType type)
{
    qDebug()<< "prompt: "<< text;
    if(text == BIOMETRIC_PAM)
    {
        if(authMode == PASSWORD)
        {
            skipBiometricAuth();
        }
        else
        {
            performBiometricAuth();
        }
    }else if(text == BIOMETRIC_PAM_DOUBLE)
    {
        if(isBioSuccess){
            m_greeter->respond(BIOMETRIC_SUCCESS);
            return ;
        }
        useDoubleAuth = true;
        if(authMode == PASSWORD)
        {
            skipBiometricAuth();
        }
        else
        {
            performBiometricAuth();
        }
    }
    else
    {
        showPasswordAuthWidget();
        m_name_is_login = false;
        qDebug()<<"m_name_is_login = false";
        stopWaiting();
        if(!text.isEmpty()){
            m_passwordEdit->setEnabled(true);
            m_passwordEdit->show();
        }
        m_passwordEdit->setFocus();
        if(type != QLightDM::Greeter::PromptTypeSecret){
            isauto_switch = true;
            m_passwordEdit->setType(QLineEdit::Normal);
        }
        else if(isauto_switch)
        {
            m_passwordEdit->setType(QLineEdit::Password);
            isauto_switch = false;
        }

        prompted = true;
        unacknowledged_messages = false;
        m_preStrMessage = "";
        qDebug()<<"unacknowledged_messages = false";
        if(text == "Password: "||text == "密码："){
            if(useDoubleAuth){
                if(!m_failMap.contains(m_uid) || m_failMap[m_uid] < maxFailedTimes)
                    onShowMessage(tr("Please enter your password or enroll your fingerprint "), QLightDM::Greeter::MessageTypeInfo);
            }
            text = tr("Password: ");
        }
        if(text == "login:") {
            text = tr("Username");
            m_name = "*login";
            //m_nameLabel->setText(tr("login"));
            m_name_is_login = true;
            qDebug()<<"m_name_is_login = true";
        }
        m_passwordEdit->clear();
        setPrompt(text);
    }
}

void LoginWindow::onShowMessage(QString text, QLightDM::Greeter::MessageType type)
{
    Q_UNUSED(type);
    QString strDisplay = "";//显示的pam返回字符串
    qDebug()<< "message: "<< text<<"type = "<<type;

    //账户锁定倒计时
    if (text.indexOf("account locked") != -1 || text.indexOf("账户已锁定") != -1 
        || text.indexOf("Account locked") != -1 || text.indexOf("永久锁定") != -1)
    {
        if(!m_timer){
            m_timer = new QTimer(this);
            m_timer->setInterval(800);
            connect(m_timer, &QTimer::timeout, this, &LoginWindow::unlock_countdown);
        }
        m_timer->start();
        return;
    }
    else if (text.indexOf("No password received, please input password") != -1){//空密码
        m_messageLabel->setText(tr("Password cannot be empty"));
        m_messageLabel->setToolTip(tr("Password cannot be empty"));
        return;
    }

    if(text == "密码为空，请输入密码" && qgetenv("LANGUAGE") == "en_US")
        text = "No password received, please input password";

    unacknowledged_messages = true;
    if (!m_preStrMessage.isEmpty() && m_preStrMessageType != type && m_preStrMessageType != -1)
    {
        text = m_preStrMessage + "," + text;
    }
    
    // text = m_preStrMessage + "\n" + text;
    qDebug()<<"unacknowledged_messages = true";

    // std::string texttmp = text.toStdString();
    // const char* ch = texttmp.c_str();

    // char str[1024];
    // sprintf(str,"%s",_(ch));
    

    strDisplay = text;
    //设置完整字符串的tooltip
    m_messageLabel->setToolTip(text);
    QFontMetrics font(m_messageLabel->font());
    int font_size = font.width(strDisplay);
    //当字符宽度大于75，不再显示完整信息
    if(font_size > 75)
    {
        //返回字符串末尾带省略号的字符串
        strDisplay = font.elidedText(text, Qt::ElideRight, 600);
    }
    m_messageLabel->setText(strDisplay);

    m_preStrMessage = text;
    m_preStrMessageType = type;
    stopWaiting();
}

void LoginWindow::onAuthenticationComplete()
{
    stopWaiting();

    if(m_biometricAuthWidget){
        m_biometricAuthWidget->stopAuth();
    }

    if(m_greeter->isAuthenticated()) {
        // 认证成功，启动session
        qDebug()<< "authentication success";
        if((prompted && !unacknowledged_messages )||direct_login){
            direct_login = false;
            Configuration::instance()->saveLastLoginUser(m_name);
            m_greeter->startSession();
        }
        else
        {
            qDebug()<<"prompted  = "<<prompted<<"  unacknowledged_messages = "<<unacknowledged_messages;
            prompted = true;
            show_authenticated ();
        }
    } else {
        // 认证失败，重新开始用户的认证
        qDebug() << "authentication failed";
        if (prompted)
        {
            m_passwordEdit->clear();

            //如果用户输入了不存在的用户名导致的认证失败，让用户重新输入用户名
            if(isManual){
                if(m_name_is_login){
                    m_name_is_login = false;
                    m_name = "*login";
                }
                // qDebug() << "authentication failed--isManual";
                if(!unacknowledged_messages && !isLockingFlg)
                    onShowMessage(tr("Authentication failure, Please try again"), QLightDM::Greeter::MessageTypeError);
            }
            else{
                if(!unacknowledged_messages && !isLockingFlg)
                    onShowMessage(tr("Authentication failure, Please try again"), QLightDM::Greeter::MessageTypeError);
                if(useDoubleAuth){
                    authMode = BIOMETRIC;
                    //qDebug() << "authentication failed--useDoubleAuth";
                }
                else{
                    //qDebug() << "authentication failed--else";
                    authMode = PASSWORD;
                }
                    
            }
            isinput_passwd = false;
            isManual = false;
            startAuthentication();
        }
        else
        {
            show_authenticated (false);
        }
    }
}

void LoginWindow::show_authenticated(bool successful)
{
    m_passwordEdit->hide();
    m_messageButton->show();
    m_messageButton->setFocus();
    m_messageButton->setDefault(true);

    connect(m_messageButton, &QPushButton::clicked,
            this, &LoginWindow::onMessageButtonClicked);
    if(successful)
    {
        isretry = false;
        m_messageButton->setText(tr("Login"));
    }
    else
    {
        isretry = true;
        m_messageButton->setText(tr("Retry"));
    }
}

void LoginWindow::onMessageButtonClicked()
{
    m_messageButton->setDefault(false);
    if(!isretry)
    {
        m_messageButton->hide();
        Configuration::instance()->saveLastLoginUser(m_name);
        m_greeter->startSession();
    }
    else
    {
        m_messageButton->hide();
        authMode = PASSWORD;

        m_messageLabel->setText("");
        startAuthentication();
    }

}

void LoginWindow::restartBioAuth()
{
    startBioAuth();
}

void LoginWindow::onPrepareForSleep(bool sleep)
{
    ///系统休眠时，会关闭总线，导致设备不可用，发生错误
    ///在系统休眠之前停止认证，在系统唤醒后重新开始认证
    if(sleep)
    {
        if(useDoubleAuth){
            if(m_biometricAuthWidget){
                manualStopBio = true;
                m_biometricAuthWidget->stopAuth();
                if(m_bioTimer && m_bioTimer->isActive())
                    m_bioTimer->stop();
            }
        }
    }
    else
    {
        if(useDoubleAuth){
            manualStopBio = false;
            restartBioAuth();
        }
    }
}

void LoginWindow::setDirLogin()
{
    direct_login = true;
}

void LoginWindow::pamBioSuccess()
{
    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);
    onShowMessage(tr("Please enter your password or enroll your fingerprint "), QLightDM::Greeter::MessageTypeInfo);
    m_bioTimer->stop();
}

void LoginWindow::startBioAuth()
{
    if(!m_bioTimer){
        m_bioTimer = new QTimer(this);
        connect(m_bioTimer, SIGNAL(timeout()), this, SLOT(pamBioSuccess()));
    }
    m_bioTimer->start(1000);
}

void LoginWindow::waitBiometricServiceStatus()
{
    qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~` get biometric status";
    QDBusInterface iface("org.freedesktop.systemd1", "/org/freedesktop/systemd1",
                         "org.freedesktop.systemd1.Manager",QDBusConnection::systemBus());

    QDBusReply<QDBusObjectPath> bioPath = iface.call("GetUnit","biometric-authentication.service");
    if(!bioPath.isValid()){
        return ;
    }

    QDBusInterface bioface("org.freedesktop.systemd1", bioPath.value().path(),
                           "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    QDBusReply<QDBusVariant> sessionReply = bioface.call("Get", "org.freedesktop.systemd1.Unit", "UnitFileState");
    if(!sessionReply.isValid())
        qWarning() << sessionReply.error();
    else {
        QString res = sessionReply.value().variant().toString();
        if(res == "disable")
            return;
    }

    qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   get activeState";
    int times = 0;
    while(times<20){
        QDBusReply<QDBusVariant> sessionReply = bioface.call("Get", "org.freedesktop.systemd1.Unit", "ActiveState");
        if(!sessionReply.isValid()){
            qWarning() << sessionReply.error();
            return ;
        }
        else {
            QString res = sessionReply.value().variant().toString();
            if(res == "activating"){
                times ++;
                usleep(100000);
            }else{
                break;
            }
        }
    }
    qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ times = "<<times;
}

void LoginWindow::performBiometricAuth()
{
    if(!m_biometricProxy)
    {
        waitBiometricServiceStatus();
        m_biometricProxy = new BiometricProxy(this);
        maxFailedTimes = GetFailedTimes();
        isHiddenSwitchButton = GetHiddenSwitchButton();
    }

    //已经错误超过3次
    if(m_failMap.contains(m_uid) && m_failMap[m_uid] >=maxFailedTimes)
    {
        useDoubleAuth = false;
        skipBiometricAuth();
        return;
    }
    //服务没启动，或者打开DBus连接出错
    if(!m_biometricProxy->isValid())
    {
        qWarning() << "An error occurs when connect to the biometric DBus";
        useDoubleAuth = false;
        skipBiometricAuth();
        return;
    }

    //初始化设备数量
    if(m_deviceCount < 0)
    {
        m_deviceCount = m_biometricProxy->GetUserDevCount(m_uid);
    }

    //没有可用设备，不启用生物识别认证
    if(m_deviceCount < 1)
    {
        useDoubleAuth = false;
        qWarning() << "No available devices";
        skipBiometricAuth();
        return;
    }

    //初始化生物识别认证UI
    if(!useDoubleAuth)
        initBiometricButtonWidget();
    initBiometricWidget();

    if(!useDoubleAuth)
        clearMessage();

    //获取默认设备
    if(m_deviceName.isEmpty())
    {
        if(m_name == "*login")
            m_deviceName = GetDefaultDevice(manualLoginName);
        else
            m_deviceName = GetDefaultDevice(m_name);
    }
    qDebug() << m_deviceName;
    //    qDebug() << (m_deviceInfo ? m_deviceInfo->shortName : "");

    //如果默认设备为空的话，第一次不启动生物识别认证
    if(m_deviceName.isEmpty() && !m_deviceInfo)
    {
        qDebug() << "No default device";
        if(m_deviceCount == 1 && useFirstDevice == true){
            DeviceList deviceList = m_biometricProxy->GetDevList();
            m_deviceInfo = deviceList.at(0);
        }else{
            useDoubleAuth = false;
            skipBiometricAuth();
            return;
        }
    }
    //第一次，获取默认设备的设备信息，之后使用的则是从设备选择窗口传出的设备信息
    if(!m_deviceInfo)
    {
        m_deviceInfo = m_biometricDevicesWidget->findDeviceByName(m_deviceName);
    }

    if(!m_deviceInfo)
    {
        qDebug() << "Device Not found: " << m_deviceName;
        useDoubleAuth = false;
        skipBiometricAuth();
        return;
    }

    if(useDoubleAuth){
        startBioAuth();
        skipBiometricAuth();
        return ;
    }

    authMode = BIOMETRIC;

    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);

    showBiometricAuthWidget();
}

void LoginWindow::skipBiometricAuth()
{
    m_greeter->respond(BIOMETRIC_IGNORE);

    showPasswordAuthWidget();

    m_passwdWidget->show();
}

void LoginWindow::initBiometricWidget()
{
    if(m_biometricAuthWidget)
    {
        m_biometricDevicesWidget->setUser(m_uid);
        return;
    }

    m_biometricAuthWidget = new BiometricAuthWidget(m_biometricProxy, this);
    connect(m_biometricAuthWidget, &BiometricAuthWidget::authComplete,
            this, &LoginWindow::onBiometricAuthComplete);
    m_biometricDevicesWidget = new BiometricDevicesWidget(m_biometricProxy,m_uid, this);
    connect(m_biometricDevicesWidget, &BiometricDevicesWidget::deviceChanged,
            this, &LoginWindow::onDeviceChanged);

    setBiometricWidgetGeometry();
}

void LoginWindow::initBiometricButtonWidget()
{
    if(m_buttonsWidget)
    {
        m_buttonsWidget->setVisible(true);
        return;
    }

    m_buttonsWidget = new QWidget(this);
    m_buttonsWidget->setObjectName(QStringLiteral("buttonsWidget"));
    m_buttonsWidget->setFixedHeight(38);

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    m_biometricButton = new QPushButton(m_buttonsWidget);
    m_biometricButton->setObjectName(QStringLiteral("biometricButton"));
    m_biometricButton->setIcon(QIcon(":/images/bio-login.png"));
    m_biometricButton->setText(tr("Biometric Authentication"));
    m_biometricButton->setSizePolicy(sizePolicy);
    m_biometricButton->setVisible(false);
    m_biometricButton->setCursor(Qt::PointingHandCursor);
    QFontMetrics fm(m_biometricButton->font(), m_biometricButton);
    int width = fm.width(m_biometricButton->text());
    m_biometricButton->setMaximumWidth(std::max(294, 190));
    connect(m_biometricButton, &QPushButton::clicked,
            this, &LoginWindow::onBiometricButtonClicked);

    m_passwordButton = new QPushButton(m_buttonsWidget);
    m_passwordButton->setObjectName(QStringLiteral("passwordButton"));
    m_passwordButton->setIcon(QIcon(":/images/password-login.png"));
    m_passwordButton->setText(tr("Password Authentication"));
    fm = QFontMetrics(m_passwordButton->font(), m_passwordButton);
    width = fm.width(m_passwordButton->text());
    m_passwordButton->setMaximumWidth(std::max(294, 140));
    m_passwordButton->setSizePolicy(sizePolicy);
    m_passwordButton->setVisible(false);
    m_passwordButton->setCursor(Qt::PointingHandCursor);
    connect(m_passwordButton, &QPushButton::clicked,
            this, &LoginWindow::onPasswordButtonClicked);

    m_otherDeviceButton = new QPushButton(m_buttonsWidget);
    m_otherDeviceButton->setObjectName(QStringLiteral("otherDeviceButton"));
    m_otherDeviceButton->setText(tr("Other Devices"));
    m_otherDeviceButton->setSizePolicy(sizePolicy);
    m_otherDeviceButton->setMaximumWidth(std::max(width + 50, 140));
    m_otherDeviceButton->setVisible(false);
    m_otherDeviceButton->setCursor(Qt::PointingHandCursor);
    connect(m_otherDeviceButton, &QPushButton::clicked,
            this, &LoginWindow::onOtherDevicesButtonClicked);

    m_retryButton = new QPushButton(m_buttonsWidget);
    m_retryButton->setObjectName(QStringLiteral("retryButton"));
    m_retryButton->setText(tr("Retry"));
    m_retryButton->setSizePolicy(sizePolicy);
    m_retryButton->setMaximumWidth(110);
    m_retryButton->setVisible(false);
    m_retryButton->setCursor(Qt::PointingHandCursor);
    connect(m_retryButton, &QPushButton::clicked,
            this, &LoginWindow::onRetryButtonClicked);

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_biometricButton, 0, 0);
    layout->addWidget(m_passwordButton, 0, 1);
    layout->addWidget(m_otherDeviceButton, 0, 2);
    layout->addWidget(m_retryButton, 0, 3);

    m_buttonsWidget->setLayout(layout);
    m_buttonsWidget->show();

    setBiometricButtonWidgetGeometry();
}

void LoginWindow::setBiometricWidgetGeometry()
{

    //生物识别
    if(m_biometricAuthWidget)
    {
        if(scale > 0 && scale < 1)
            m_biometricAuthWidget->setMinImage(scale);

        m_biometricAuthWidget->setGeometry(0, m_userWidget->geometry().bottom(),
                                           width(), m_biometricAuthWidget->height());
    }
    if(m_biometricDevicesWidget)
    {
        m_biometricDevicesWidget->setGeometry((width() - m_biometricDevicesWidget->width()) / 2,
                                              0,
                                              m_biometricDevicesWidget->width(),
                                              m_biometricDevicesWidget->height());
    }
}

void LoginWindow::setBiometricButtonWidgetGeometry()
{
    if(m_buttonsWidget)
    {
        if(scale > 0.5)
            m_buttonsWidget->setGeometry(0, height() - 100*scale - m_buttonsWidget->height(),
                                         width(), m_buttonsWidget->height());
        else
            m_buttonsWidget->setGeometry(0, height() - 20 - m_buttonsWidget->height(),
                                         width(), m_buttonsWidget->height());
    }
}

void LoginWindow::onDeviceChanged(const DeviceInfoPtr &deviceInfo)
{
    qDebug() << "device changed: " << *deviceInfo;
    m_deviceInfo = deviceInfo;

    if(authMode != BIOMETRIC){
        authMode = BIOMETRIC;
        startAuthentication();
    }
    else
        m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);

    showBiometricAuthWidget();
}

void LoginWindow::onBiometricAuthComplete(bool result)
{
    if(!result)
    {
        if(useDoubleAuth){
            if(manualStopBio){
                manualStopBio = false;
                return;
            }

            if(m_failMap.contains(m_uid)){
                m_failMap[m_uid] = m_failMap[m_uid] + 1;
            }else{
                m_failMap[m_uid] = 1;
            }

            if(m_failMap[m_uid] >= maxFailedTimes){
                onShowMessage(tr("Too many unsuccessful attempts,please enter password."),QLightDM::Greeter::MessageTypeInfo);
                useDoubleAuth = false;
                return ;
            }

            onShowMessage(tr("Authentication failure, Please try again"), QLightDM::Greeter::MessageTypeInfo);
            if(!isBioSuccess)
                startBioAuth();
        }
        else{
            m_retryButton->setVisible(!m_biometricAuthWidget->isHidden());
        }
    }
    else
    {
        if(useDoubleAuth){
            //onShowMessage("验证成功!", QLightDM::Greeter::MessageTypeInfo);
            setDirLogin();
            isBioSuccess = true;
            onBiometricButtonClicked();
        }else{
            setDirLogin();
            m_greeter->respond(BIOMETRIC_SUCCESS);
        }
    }
}

void LoginWindow::onBiometricButtonClicked()
{
    //当前没有设备，让用户选择设备
    if(!m_deviceInfo)
    {
        if(m_deviceCount == 1)
        {
            if(m_biometricDevicesWidget)
                m_biometricDevicesWidget->onOKButtonClicked();
        }
        else
        {
            m_otherDeviceButton->click();
        }
    }
    else
    {
        authMode = BIOMETRIC;
        startAuthentication();
    }
}

void LoginWindow::onPasswordButtonClicked()
{
    skipBiometricAuth();
}

void LoginWindow::onOtherDevicesButtonClicked()
{
    m_biometricAuthWidget->stopAuth();

    showBiometricDeviceWidget();
}

void LoginWindow::onRetryButtonClicked()
{
    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);
    m_retryButton->setVisible(false);
}


void LoginWindow::showPasswordAuthWidget()
{ 
    m_passwdWidget->setVisible(true);
    if(m_biometricAuthWidget)
    {
        m_biometricAuthWidget->setVisible(false);
        m_biometricDevicesWidget->setVisible(false);

        m_biometricAuthWidget->stopAuth();
    }

    if(m_buttonsWidget)
    {
        if(m_deviceCount > 0)
        {
            m_buttonsWidget->setVisible(true);
            if(!isHiddenSwitchButton)
                m_biometricButton->setVisible(true);
            m_passwordButton->setVisible(false);
            m_otherDeviceButton->setVisible(false);
            m_retryButton->setVisible(false);
        }
        else
        {
            qDebug() << "hide buttons widget";
            m_buttonsWidget->setVisible(false);
        }
    }
}

void LoginWindow::showBiometricAuthWidget()
{
    m_userWidget->setVisible(true);
    m_passwdWidget->setVisible(false);
    m_messageButton->hide();
    if(m_biometricAuthWidget)
    {
        m_biometricAuthWidget->setVisible(true);
        m_biometricDevicesWidget->setVisible(false);
    }

    if(m_buttonsWidget && m_deviceCount > 0)
    {
        m_buttonsWidget->setVisible(true);
        m_biometricButton->setVisible(false);
        if(!isHiddenSwitchButton)
            m_passwordButton->setVisible(true);
        m_otherDeviceButton->setVisible(m_deviceCount > 1);
    }
    else
    {
        m_buttonsWidget->setVisible(false);
    }
}

void LoginWindow::unlock_countdown()
{
    int failed_count = 0;
    int time_left = 0;
    int deny = 0;
    int fail_time =0;
    int unlock_time = 0;
    
    
    if (NULL == m_name.toLatin1().data())
    {
        qDebug() << "get user info failed." ;
        return;
    }
    
    struct passwd* user = NULL;
    user = getpwnam(m_name.toLatin1().data());

    pam_tally_unlock_time_left(user->pw_uid, &failed_count, &time_left, &deny,&fail_time,&unlock_time);

    // qDebug() << "failed_count:" << failed_count << "time_left:" <<time_left <<"deny:"<<deny<<"fail_time:"<< fail_time<<"unlock_time:" << unlock_time;
    if(time_left/60 > 0)//请多少分钟后重试
    {
        char ch[100]={0};
        int nMinute = time_left/60 + 1;
        m_messageLabel->setText(tr("Please try again in %1 minutes.").arg(nMinute));
        m_messageLabel->setToolTip(tr("Please try again in %1 minutes.").arg(nMinute));
        m_passwordEdit->clearText();
        m_passwordEdit->setDisabled(true);
        isLockingFlg = true;
        return ;
    }
    else if(time_left > 0)//请多少秒后重试
    {
        char ch[100]={0};
        m_messageLabel->setText(tr("Please try again in %1 seconds.").arg(time_left%60));
        m_messageLabel->setToolTip(tr("Please try again in %1 seconds.").arg(time_left%60));
        m_passwordEdit->clearText();
        m_passwordEdit->setDisabled(true);
        isLockingFlg = true;
        return ;
    }
    else if (failed_count == 0xFFFF)//账号被永久锁定
    {
        m_messageLabel->setText(tr("Account locked permanently."));
        m_messageLabel->setToolTip(tr("Account locked permanently."));
        m_passwordEdit->clearText();
        m_passwordEdit->setDisabled(true);
        isLockingFlg = true;
        return ;
    }
    else
    {
        if(m_passwordEdit){
            m_passwordEdit->setDisabled(false);
            m_passwordEdit->setFocus();
        }
        //    qDebug() << "清理 1 clearMessage";
        if (isLockingFlg)
        {
            onShowMessage(tr("Authentication failure, Please try again"), QLightDM::Greeter::MessageTypeInfo);
            isLockingFlg = false;
        }
            
        m_timer->stop();
    }
    return ;
}

void LoginWindow::root_unlock_countdown()
{
    int failed_count = 0;
    int time_left = 0;
    int deny = 0;
    if (NULL == m_name.toLatin1().data())
    {
        qDebug() << "get user info failed." ;
        return;
    }
    
    struct passwd* user = NULL;
    user = getpwnam(m_name.toLatin1().data());
    pam_tally_root_unlock_time_left(user->pw_uid, &failed_count, &time_left, &deny);
    if(time_left/60 > 0)//请多少分钟后重试
    {
        char ch[100]={0};
        int nMinute = time_left/60 + 1;
        m_messageLabel->setText(tr("Please try again in %1 minutes.").arg(nMinute));
        m_messageLabel->setToolTip(tr("Please try again in %1 minutes.").arg(nMinute));
        m_passwordEdit->clearText();
        m_passwordEdit->setDisabled(true);
        isLockingFlg = true;
        return ;
    }
    else if(time_left > 0)//请多少秒后重试
    {
        char ch[100]={0};
        m_messageLabel->setText(tr("Please try again in %1 seconds.").arg(time_left%60));
        m_messageLabel->setToolTip(tr("Please try again in %1 seconds.").arg(time_left%60));
        m_passwordEdit->clearText();
        m_passwordEdit->setDisabled(true);
        isLockingFlg = true;
        return ;
    }
    else if (failed_count == 0xFFFF)//账号被永久锁定
    {
        m_messageLabel->setText(tr("Account locked permanently."));
        m_messageLabel->setToolTip(tr("Account locked permanently."));
        m_passwordEdit->clearText();
        m_passwordEdit->setDisabled(true);
        isLockingFlg = true;
        return ;
    }
    else
    {
        if(m_passwordEdit){
            m_passwordEdit->setDisabled(false);
            m_passwordEdit->setFocus();
        }
        if (isLockingFlg)
        {
            onShowMessage(tr("Authentication failure, Please try again"), QLightDM::Greeter::MessageTypeInfo);
            isLockingFlg = false;
        }
        m_timer->stop();
    }
    return ;
}

void LoginWindow::showBiometricDeviceWidget()
{
    m_userWidget->setVisible(false);
    m_passwdWidget->setVisible(false);
    m_messageButton->hide();
    if(m_biometricAuthWidget)
    {
        m_biometricAuthWidget->setVisible(false);
        m_biometricDevicesWidget->setVisible(true);
    }

    if(m_buttonsWidget)
    {
        m_buttonsWidget->setVisible(false);
        m_biometricButton->setVisible(false);
        m_passwordButton->setVisible(false);
        m_otherDeviceButton->setVisible(false);
        m_retryButton->setVisible(false);
    }
}
