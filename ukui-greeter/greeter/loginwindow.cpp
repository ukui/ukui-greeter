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


LoginWindow::LoginWindow(GreeterWrapper *greeter, QWidget *parent)
    : QWidget(parent),
      m_greeter(greeter),
      isManual(false),
      authMode(UNKNOWN),
      m_deviceCount(-1),
      m_featureCount(-1),
      m_biometricProxy(nullptr),
      m_biometricAuthWidget(nullptr),
      m_biometricDevicesWidget(nullptr),
      m_buttonsWidget(nullptr)
{    
    initUI();

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

//    m_userWidget = new QWidget(this);
//    m_userWidget->setObjectName(QStringLiteral("userWidget"));

    /* 头像 */
//    m_faceLabel = new QLabel(m_userWidget);
//    m_faceLabel->setObjectName(QStringLiteral("faceLabel"));
//    m_faceLabel->setFocusPolicy(Qt::NoFocus);

    /* 返回按钮 */
//    m_backButton = new QPushButton(m_userWidget);
//    m_backButton->setObjectName(QStringLiteral("backButton"));
//    m_backButton->setFocusPolicy(Qt::NoFocus);
//    m_backButton->show();
//    connect(m_backButton, &QPushButton::clicked,
//            this, &LoginWindow::onBackButtonClicked);

//    /* 用户名 */
//    m_nameLabel = new QLabel(m_userWidget);
//    m_nameLabel->setObjectName(QStringLiteral("login_nameLabel"));
//    m_nameLabel->setFocusPolicy(Qt::NoFocus);
//    m_nameLabel->setAlignment(Qt::AlignCenter);

//    /* 是否已登录 */
//    m_isLoginLabel = new QLabel(m_userWidget);
//    m_isLoginLabel->setObjectName(QStringLiteral("isLoginLabel"));
//    m_isLoginLabel->setFocusPolicy(Qt::NoFocus);
//    m_isLoginLabel->setAlignment(Qt::AlignCenter);

    /* 密码框所在窗口 */
    m_passwdWidget = new QWidget(this);
    m_passwdWidget->setObjectName(QStringLiteral("passwordWidget"));

    /* 密码框 */
    m_passwordEdit = new IconEdit(m_passwdWidget);
    m_passwordEdit->setObjectName(QStringLiteral("passwordEdit"));   
    m_passwordEdit->setIcon(QIcon(":/images/login-button.png"));
    m_passwordEdit->setFocusPolicy(Qt::StrongFocus);
    m_passwordEdit->installEventFilter(this);
    m_passwordEdit->hide(); //收到请求密码的prompt才显示出来
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)),
            this, SLOT(onLogin(const QString&)));

    /* 密码认证信息显示列表 */
    m_messageLabel = new QLabel(m_passwdWidget);
    m_messageLabel->setObjectName(QStringLiteral("messageLabel"));
    m_messageLabel->setAlignment(Qt::AlignCenter);

    m_messageButton = new QPushButton(m_passwdWidget);
    m_messageButton->setObjectName(QStringLiteral("messageButton"));
    m_messageButton->hide();

    setFocusProxy(m_passwordEdit);

    isloginauth = false;
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

#include <QKeyEvent>
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
        //返回用户列表
        else if(m_biometricAuthWidget && m_biometricAuthWidget->isVisible())
        {
            m_biometricAuthWidget->stopAuth();
            Q_EMIT back();
            return;
        }
    }
    QWidget::keyReleaseEvent(event);
}

void LoginWindow::setChildrenGeometry()
{
    // 用户信息显示位置
//    m_userWidget->setGeometry(0, (height() - 240 - 150) / 2,
//                              width(), 240);

//    m_faceLabel->setGeometry((width() - 130) / 2, 0, 130, 130);
//    m_backButton->setGeometry(m_faceLabel->geometry().left() - 30 - 32,
//                              m_faceLabel->geometry().top() + (m_faceLabel->height() - 32) / 2,
//                              32, 32);
//    m_nameLabel->setGeometry(0, m_faceLabel->geometry().bottom() + 25,
//                             width(), 30);
//    m_isLoginLabel->setGeometry(0, m_nameLabel->geometry().bottom() + 10,
//                                width(), 15);

    // 密码框和提示信息显示位置

    if(scale < 0.5)
        setFixedWidth(300);

    m_passwdWidget->setGeometry(0, 0, width(), 150);
    m_passwordEdit->setGeometry((m_passwdWidget->width() - 300)/2, 0, 300, 34);
    m_messageLabel->setGeometry((m_passwdWidget->width() - 600)/2,
                                m_passwordEdit->geometry().bottom() + 25,
                                600, 20);

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
//    m_nameLabel->clear();
//    m_isLoginLabel->clear();
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
}

/**
 * @brief LoginWindow::backToUsers
 * 返回到用户列表窗口
 */
void LoginWindow::onBackButtonClicked()
{
    reset();
    Q_EMIT back();
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
    m_faceLabel->setFixedSize(128,128);
    //如果头像文件不存在，则使用默认头像
    if(faceFile.exists())
        faceImage = scaledPixmap(128, 128, facePath);
    else
        faceImage = scaledPixmap(128, 128, ":/images/default_face.png");

    faceImage =  PixmapToRound(faceImage,62);
    m_faceLabel->setAlignment(Qt::AlignCenter);
    m_faceLabel->setPixmap(faceImage);
}

/**
 * @brief LoginWindow::setLoggedIn
 * @param isLoggedIn
 *
 * 设置当前用户是否已已经登录
 */
void LoginWindow::setLoggedIn(bool isLoggedIn)
{
    m_isLoginLabel->setText(isLoggedIn ? tr("logged in") : "");
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
    if(!index.isValid()){
        return false;
    }
    //先清空设置
    reset();

    //设置用户名
    QString name = index.data(QLightDM::UsersModel::RealNameRole).toString();
    m_name = index.data(QLightDM::UsersModel::NameRole).toString();
   // setUserName(name.isEmpty() ? m_name : name);

    //设置头像
//    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
//    setFace(facePath);

    //显示是否已经登录
//    bool isLoggedIn = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
//    setLoggedIn(isLoggedIn);

    //设置生物识别设备窗口的uid
    m_uid = index.data(QLightDM::UsersModel::UidRole).toInt();

    setChildrenGeometry();

    if(!isloginauth)
    	startAuthentication();

    isloginauth = false;

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
        Q_EMIT userChangedByManual(str);
        m_greeter->respond(str);
    }
    else
    {  //发送密码
        m_greeter->respond(str);
        startWaiting();
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
    }
    else
    {
        m_name_is_login = false;
        qDebug()<<"m_name_is_login = false";
        stopWaiting();
        if(!text.isEmpty())
            m_passwordEdit->show();

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
        qDebug()<<"unacknowledged_messages = false";
        if(text == "Password: ")
            text = tr("Password: ");
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
    qDebug()<< "message: "<< text;

//    if(type == QLightDM::Greeter::MessageTypeError)
//    {
//        m_messageLabel->setStyleSheet("#messageLabel{color: rgb(255, 0, 0, 180);}");
//    }
//    else
//    {
//        m_messageLabel->setStyleSheet("#messageLabel{color: black;}");
//    }
    unacknowledged_messages = true;
    qDebug()<<"unacknowledged_messages = true";
    m_messageLabel->setText(text);
    stopWaiting();
}

void LoginWindow::onAuthenticationComplete()
{
    stopWaiting();
    if(m_greeter->isAuthenticated()) {
        // 认证成功，启动session
        qDebug()<< "authentication success";
        if(prompted && !unacknowledged_messages ||direct_login){
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
                        onShowMessage(tr("Incorrect user name, please input again"), QLightDM::Greeter::MessageTypeError);
                        m_name_is_login = false;
                        m_name = "*login";
                    }
                    else{
                        onShowMessage(tr("Incorrect password, please input again"), QLightDM::Greeter::MessageTypeError);
                    }
                       // m_name = "*login";
            }       
            else{
                onShowMessage(tr("Incorrect password, please input again"), QLightDM::Greeter::MessageTypeError);
                authMode = PASSWORD;
            }
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
      //  m_greeter->startSession();
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
      //  m_passwordEdit->show();
      //  m_passwordEdit->clear();

        authMode = PASSWORD;


        m_messageLabel->setText("");
        startAuthentication();
    }

}

void LoginWindow::setDirLogin()
{
    direct_login = true;
}

void LoginWindow::performBiometricAuth()
{
    if(!m_biometricProxy)
    {
        m_biometricProxy = new BiometricProxy(this);
    }

    //服务没启动，或者打开DBus连接出错
    if(!m_biometricProxy->isValid())
    {
        qWarning() << "An error occurs when connect to the biometric DBus";
        skipBiometricAuth();
        return;
    }

    //初始化设备数量
    if(m_deviceCount < 0)
    {
        m_deviceCount = m_biometricProxy->GetDevCount();
    }

    //没有可用设备，不启用生物识别认证
    if(m_deviceCount < 1)
    {
        qWarning() << "No available devices";
        skipBiometricAuth();
        return;
    }

    //初始化用户对应特征数量    
    m_featureCount = m_biometricProxy->GetFeatureCount(m_uid);
	
    qDebug()<<"m_featureCount = "<<m_featureCount;
    //没有可用特征，不启用生物识别认证    
    if(m_featureCount < 1)
    {
        skipBiometricAuth();
        return;
    }

    //初始化生物识别认证UI
    initBiometricButtonWidget();
    initBiometricWidget();
    clearMessage();

    //获取默认设备
    if(m_deviceName.isEmpty())
    {
        m_deviceName = GetDefaultDevice(m_name);
    }
    qDebug() << m_deviceName;
//    qDebug() << (m_deviceInfo ? m_deviceInfo->shortName : "");

    //如果默认设备为空的话，第一次不启动生物识别认证
    if(m_deviceName.isEmpty() && !m_deviceInfo)
    {
        qDebug() << "No default device";
        skipBiometricAuth();
        return;
    }
    //第一次，获取默认设备的设备信息，之后使用的则是从设备选择窗口传出的设备信息
    if(!m_deviceInfo)
    {
        m_deviceInfo = m_biometricDevicesWidget->findDeviceByName(m_deviceName);
    }
    if(!m_deviceInfo)
    {
        qDebug() << "Device Not found: " << m_deviceName;
        skipBiometricAuth();
        return;
    }

    authMode = BIOMETRIC;

    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);

//    m_biometricAuthWidget->show();
//    m_passwdWidget->hide();
//    m_biometricButton->hide();
//    m_passwordButton->setVisible(true);
//    m_otherDeviceButton->setVisible(m_deviceCount > 1);
//    m_retryButton->setVisible(false);
    showBiometricAuthWidget();
}

void LoginWindow::skipBiometricAuth()
{
    m_greeter->respond(BIOMETRIC_IGNORE);

    showPasswordAuthWidget();

//    if(m_biometricAuthWidget)
//    {
//        m_biometricAuthWidget->hide();
//    }
//    if(m_buttonsWidget)
//    {
//        m_biometricButton->setVisible(true);
//        m_passwordButton->setVisible(false);
//        m_otherDeviceButton->setVisible(false);
//       m_retryButton->setVisible(false);
//    }
    m_passwdWidget->show();
}

void LoginWindow::initBiometricWidget()
{
    if(m_biometricAuthWidget)
    {
        return;
    }

    m_biometricAuthWidget = new BiometricAuthWidget(m_biometricProxy, this);
    connect(m_biometricAuthWidget, &BiometricAuthWidget::authComplete,
            this, &LoginWindow::onBiometricAuthComplete);
    m_biometricDevicesWidget = new BiometricDevicesWidget(m_biometricProxy, this);
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
    m_biometricButton->setMaximumWidth(std::max(width + 20, 190));
    connect(m_biometricButton, &QPushButton::clicked,
            this, &LoginWindow::onBiometricButtonClicked);

    m_passwordButton = new QPushButton(m_buttonsWidget);
    m_passwordButton->setObjectName(QStringLiteral("passwordButton"));
    m_passwordButton->setIcon(QIcon(":/images/password-login.png"));
    m_passwordButton->setText(tr("Password Authentication"));
    fm = QFontMetrics(m_passwordButton->font(), m_passwordButton);
    width = fm.width(m_passwordButton->text());
    m_passwordButton->setMaximumWidth(std::max(width + 20, 140));
    m_passwordButton->setSizePolicy(sizePolicy);
    m_passwordButton->setVisible(false);
    m_passwordButton->setCursor(Qt::PointingHandCursor);
    connect(m_passwordButton, &QPushButton::clicked,
            this, &LoginWindow::onPasswordButtonClicked);

    m_otherDeviceButton = new QPushButton(m_buttonsWidget);
    m_otherDeviceButton->setObjectName(QStringLiteral("otherDeviceButton"));
    m_otherDeviceButton->setText(tr("Other Devices"));
    m_otherDeviceButton->setSizePolicy(sizePolicy);
    m_otherDeviceButton->setMaximumWidth(std::max(width + 20, 140));
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
        m_biometricAuthWidget->setGeometry(0, 0,
                                           width(), m_biometricAuthWidget->height());
    }
    if(m_biometricDevicesWidget)
    {
        m_biometricDevicesWidget->setGeometry((width() - m_biometricDevicesWidget->width()) / 2,
                                              (height() - m_biometricDevicesWidget->height()) / 2,
                                              m_biometricDevicesWidget->width(),
                                              m_biometricDevicesWidget->height());
    }
}

void LoginWindow::setBiometricButtonWidgetGeometry()
{
    if(m_buttonsWidget)
    {
        m_buttonsWidget->setGeometry(0, height() - 100,
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

//    m_backButton->show();
//    m_userWidget->show();
//    m_passwdWidget->hide();
//    m_biometricAuthWidget->show();
//    m_buttonsWidget->show();
//    m_biometricDevicesWidget->hide();
//    m_biometricButton->hide();
//    m_passwordButton->show();
//    m_otherDeviceButton->show();
//    m_retryButton->hide();
    showBiometricAuthWidget();
}

void LoginWindow::onBiometricAuthComplete(bool result)
{
    if(!result)
    {
        m_retryButton->setVisible(!m_biometricAuthWidget->isHidden());
    }
    else
    {
        setDirLogin();
        m_greeter->respond(BIOMETRIC_SUCCESS);
    }
}

void LoginWindow::onBiometricButtonClicked()
{
    //当前没有设备，让用户选择设备
    if(!m_deviceInfo)
    {
//        if(m_deviceCount == 1)
//        {
//            DeviceList deviceList = m_biometricProxy->GetDevList();
//            m_deviceInfo = deviceList.at(0);
//            if(!m_deviceInfo)
//            {
//                m_otherDeviceButton->click();
//            }
//            else
//            {
//                authMode = BIOMETRIC;
//                startAuthentication();
//            }
//        }
//        else
//        {
            m_otherDeviceButton->click();
//        }
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

//    m_biometricAuthWidget->stopAuth();
}

void LoginWindow::onOtherDevicesButtonClicked()
{
    m_biometricAuthWidget->stopAuth();

//    m_backButton->hide();
//    m_userWidget->hide();
//    m_passwdWidget->hide();
//    m_biometricAuthWidget->hide();
//    m_buttonsWidget->hide();
//    m_biometricDevicesWidget->show();
    showBiometricDeviceWidget();
}

void LoginWindow::onRetryButtonClicked()
{
    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);
    m_retryButton->setVisible(false);
}


void LoginWindow::showPasswordAuthWidget()
{ 
    //m_userWidget->setVisible(true);
    m_passwdWidget->setVisible(true);

    if(m_biometricAuthWidget)
    {
        m_biometricAuthWidget->setVisible(false);
        m_biometricDevicesWidget->setVisible(false);

        m_biometricAuthWidget->stopAuth();
    }

    if(m_buttonsWidget)
    {
        if(m_deviceCount > 0 && m_featureCount > 0)
        {
            m_buttonsWidget->setVisible(true);
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
    emit bioDeviceIsChoosed(true);
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
        m_passwordButton->setVisible(true);
        m_otherDeviceButton->setVisible(m_deviceCount > 1);
        m_retryButton->setVisible(!m_biometricAuthWidget->isAuthenticating());
    }
    else
    {
        m_buttonsWidget->setVisible(false);
    }
}

void LoginWindow::showBiometricDeviceWidget()
{
    emit bioDeviceIsChoosed(false);

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
