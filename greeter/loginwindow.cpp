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
      enableBiometricAuth(-1),
      authMode(UNKNOWN),
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

void LoginWindow::initUI()
{
    setFixedWidth(500);

    m_userWidget = new QWidget(this);
    m_userWidget->setObjectName(QStringLiteral("userWidget"));

    /* 头像 */
    m_faceLabel = new QLabel(m_userWidget);
    m_faceLabel->setObjectName(QStringLiteral("faceLabel"));
    m_faceLabel->setFocusPolicy(Qt::NoFocus);

    /* 返回按钮 */
    m_backButton = new QPushButton(m_userWidget);
    m_backButton->setObjectName(QStringLiteral("backButton"));
    m_backButton->setFocusPolicy(Qt::NoFocus);
    connect(m_backButton, &QPushButton::clicked,
            this, &LoginWindow::onBackButtonClicked);

    /* 用户名 */
    m_nameLabel = new QLabel(m_userWidget);
    m_nameLabel->setObjectName(QStringLiteral("login_nameLabel"));
    m_nameLabel->setFocusPolicy(Qt::NoFocus);
    m_nameLabel->setAlignment(Qt::AlignCenter);

    /* 是否已登录 */
    m_isLoginLabel = new QLabel(m_userWidget);
    m_isLoginLabel->setObjectName(QStringLiteral("isLoginLabel"));
    m_isLoginLabel->setFocusPolicy(Qt::NoFocus);
    m_isLoginLabel->setAlignment(Qt::AlignCenter);

    /* 密码框所在窗口 */
    m_passwdWidget = new QWidget(this);
    m_passwdWidget->setObjectName(QStringLiteral("passwordWidget"));

    /* 密码框 */
    m_passwordEdit = new IconEdit(m_passwdWidget);
    m_passwordEdit->setObjectName(QStringLiteral("passwordEdit"));   
    m_passwordEdit->setIcon(QIcon(":/resource/login-button.png"));
    m_passwordEdit->setFocusPolicy(Qt::StrongFocus);
    m_passwordEdit->installEventFilter(this);
    m_passwordEdit->hide(); //收到请求密码的prompt才显示出来
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)),
            this, SLOT(onLogin(const QString&)));

    /* 密码认证信息显示列表 */
    m_messageLabel = new QLabel(m_passwdWidget);
    m_messageLabel->setObjectName(QStringLiteral("messageLabel"));
    m_messageLabel->setAlignment(Qt::AlignCenter);
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

void LoginWindow::setChildrenGeometry()
{
    // 用户信息显示位置
    m_userWidget->setGeometry(0, (height() - 240 - 150) / 2,
                              width(), 240);

    m_faceLabel->setGeometry((width() - 128) / 2, 0, 128, 128);
    m_backButton->setGeometry(m_faceLabel->geometry().left() - 30 - 32,
                              m_faceLabel->geometry().top() + (m_faceLabel->height() - 32) / 2,
                              32, 32);
    m_nameLabel->setGeometry(0, m_faceLabel->geometry().bottom() + 25,
                             width(), 30);
    m_isLoginLabel->setGeometry(0, m_nameLabel->geometry().bottom() + 10,
                                width(), 15);

    // 密码框和提示信息显示位置
    m_passwdWidget->setGeometry(0, m_userWidget->geometry().bottom(), width(), 150);
    m_passwordEdit->setGeometry((m_passwdWidget->width() - 400)/2, 0, 400, 40);
    m_messageLabel->setGeometry((m_passwdWidget->width() - 300)/2,
                                m_passwordEdit->geometry().bottom() + 25,
                                300, 20);


    setBiometricWidgetGeometry();
    setBiometricButtonWidgetGeometry();
}

/**
 * @brief LoginWindow::reset
 * 复原UI
 */
void LoginWindow::reset()
{
    m_nameLabel->clear();
    m_isLoginLabel->clear();
    m_passwordEdit->clear();
    m_passwordEdit->setType(QLineEdit::Password);
    m_passwordEdit->hide();
    m_backButton->show();
    m_userWidget->show();
    if(m_biometricAuthWidget)
        m_biometricAuthWidget->hide();
    if(m_biometricDevicesWidget)
        m_biometricDevicesWidget->hide();
    if(m_buttonsWidget)
        m_buttonsWidget->show();
    clearMessage();
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
    //如果头像文件不存在，则使用默认头像
    if(faceFile.exists())
        faceImage = scaledPixmap(128, 128, facePath);
    else
        faceImage = scaledPixmap(128, 128, ":/resource/default_face.png");

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

    return true;
}

/**
 * @brief 手动输入的用户名不在用户列表中（uid < 1000或者用户不存在）
 * @param userName
 */
void LoginWindow::setUserNotInView(const QString &userName)
{
    m_name = userName;
    setUserName(userName);
}

void LoginWindow::startAuthentication()
{
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
    m_backButton->setEnabled(false);
}

void LoginWindow::stopWaiting()
{
    m_passwordEdit->stopWaiting();
    m_backButton->setEnabled(true);
}



void LoginWindow::onLogin(const QString &str)
{
    clearMessage();

    if(m_name == "*guest")
    {
        m_greeter->authenticateAsGuest();
    }
    else if(m_name == "*login")
    {   //用户输入用户名
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
            performPasswordAuth();
        }
        else
        {
            performBiometricAuth();
        }
    }
    else
    {
        m_passwordEdit->stopWaiting();
        if(!text.isEmpty())
            m_passwordEdit->show();

        m_passwordEdit->setFocus();
        if(type != QLightDM::Greeter::PromptTypeSecret)
            m_passwordEdit->setType(QLineEdit::Normal);
        else
            m_passwordEdit->setType(QLineEdit::Password);

        if(text == "Password: ")
            text = tr("Password: ");
        if(text == "login:") {
            text = tr("login:");
            m_name = "*login";
            m_nameLabel->setText(tr("login"));
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

    m_messageLabel->setText(text);
}

void LoginWindow::onAuthenticationComplete()
{
    m_passwordEdit->stopWaiting();
    if(m_greeter->isAuthenticated()) {
        // 认证成功，启动session
        qDebug()<< "authentication success";
        Configuration::instance()->saveLastLoginUser(m_name);
        m_greeter->startSession();
    } else {
        // 认证失败，重新开始用户的认证
        qDebug() << "authentication failed";
        onShowMessage(tr("Incorrect password, please input again"), QLightDM::Greeter::MessageTypeError);
        m_passwordEdit->clear();

        //如果用户输入了不存在的用户名导致的认证失败，让用户重新输入用户名
        if(isManual)
            m_name = "*login";
        else
            authMode = PASSWORD;
        startAuthentication();
    }
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
        performPasswordAuth();
        return;
    }

    //初始化enableBiometriAuth
    if(enableBiometricAuth < 0)
    {
        enableBiometricAuth = m_biometricProxy->GetDevCount();
    }

    //没有可用设备，不启用生物识别认证
    if(enableBiometricAuth < 1)
    {
        qWarning() << "No available devices";
        performPasswordAuth();
        return;
    }

    initBiometricButtonWidget();

    //获取默认设备
    if(m_deviceName.isEmpty())
    {
        m_deviceName = m_biometricProxy->GetDefaultDevice(m_name);
    }

    qDebug() << (m_deviceInfo ? m_deviceInfo->shortName : "");

    //如果默认设备为空的话，第一次不启动生物识别认证
    if(m_deviceName.isEmpty() && !m_deviceInfo)
    {
        performPasswordAuth();
        return;
    }

    //初始化生物识别认证UI
    initBiometricWidget();
    clearMessage();

    if(!m_deviceInfo)
    {
        m_deviceInfo = m_biometricDevicesWidget->findDeviceByName(m_deviceName);
    }
    if(!m_deviceInfo)
    {
        performPasswordAuth();
        return;
    }

    authMode = BIOMETRIC;

    m_biometricAuthWidget->show();
    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);

    m_passwdWidget->hide();
    m_biometricButton->hide();
    m_passwordButton->setVisible(true);
    m_otherDeviceButton->setVisible(enableBiometricAuth > 1);
    m_retryButton->setVisible(false);
}

void LoginWindow::performPasswordAuth()
{
    m_greeter->respond(BIOMETRIC_IGNORE);

    if(m_biometricAuthWidget)
    {
        m_biometricAuthWidget->hide();
    }
    if(m_buttonsWidget)
    {
        m_biometricButton->setVisible(true);
        m_passwordButton->setVisible(false);
        m_otherDeviceButton->setVisible(false);
        m_retryButton->setVisible(false);
    }
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
        return;
    }

    m_buttonsWidget = new QWidget(this);
    m_buttonsWidget->setObjectName(QStringLiteral("buttonsWidget"));
    m_buttonsWidget->setFixedHeight(25);

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    m_biometricButton = new QPushButton(m_buttonsWidget);
    m_biometricButton->setObjectName(QStringLiteral("biometricButton"));
    m_biometricButton->setText(tr("Biometric Authentication"));
    m_biometricButton->setSizePolicy(sizePolicy);
    m_biometricButton->setVisible(false);
    m_biometricButton->setCursor(Qt::PointingHandCursor);
    QFontMetrics fm(m_biometricButton->font(), m_biometricButton);
    int width = fm.width(m_biometricButton->text());
    m_biometricButton->setMaximumWidth(width + 20);
    connect(m_biometricButton, &QPushButton::clicked,
            this, &LoginWindow::onBiometricButtonClicked);

    m_passwordButton = new QPushButton(m_buttonsWidget);
    m_passwordButton->setObjectName(QStringLiteral("passwordButton"));
    m_passwordButton->setText(tr("Password Authentication"));
    fm = QFontMetrics(m_passwordButton->font(), m_passwordButton);
    width = fm.width(m_passwordButton->text());
    m_passwordButton->setMaximumWidth(std::max(width + 20, 110));
    m_passwordButton->setSizePolicy(sizePolicy);
    m_passwordButton->setVisible(false);
    m_passwordButton->setCursor(Qt::PointingHandCursor);
    connect(m_passwordButton, &QPushButton::clicked,
            this, &LoginWindow::onPasswordButtonClicked);

    m_otherDeviceButton = new QPushButton(m_buttonsWidget);
    m_otherDeviceButton->setObjectName(QStringLiteral("otherDeviceButton"));
    m_otherDeviceButton->setText(tr("Other Devices"));
    m_otherDeviceButton->setSizePolicy(sizePolicy);
    m_otherDeviceButton->setMaximumWidth(std::max(width + 20, 110));
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
        m_biometricAuthWidget->setGeometry(0, m_userWidget->geometry().bottom(),
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
        m_buttonsWidget->setGeometry(0, height() - m_buttonsWidget->height() - 20,
                                     width(), m_buttonsWidget->height());
    }
}

void LoginWindow::onDeviceChanged(const DeviceInfoPtr &deviceInfo)
{
    qDebug() << "device changed: " << *deviceInfo;
    m_deviceInfo = deviceInfo;

    if(authMode != BIOMETRIC)
        startAuthentication();
    else
        m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);

    m_backButton->show();
    m_userWidget->show();
    m_passwdWidget->hide();
    m_biometricAuthWidget->show();
    m_buttonsWidget->show();
    m_biometricDevicesWidget->hide();
    m_biometricButton->hide();
    m_passwordButton->show();
    m_otherDeviceButton->show();
    m_retryButton->hide();
}

void LoginWindow::onBiometricAuthComplete(bool result)
{
    if(!result)
    {
        m_retryButton->setVisible(!m_biometricAuthWidget->isHidden());
    }
    else
    {
        m_greeter->respond(BIOMETRIC_SUCCESS);
    }
}

void LoginWindow::onBiometricButtonClicked()
{
    //当前没有设备，让用户选择设备
    if(!m_deviceInfo)
    {
        m_otherDeviceButton->click();

    }
    else
    {
        authMode = BIOMETRIC;
        startAuthentication();
    }

}

void LoginWindow::onPasswordButtonClicked()
{
    performPasswordAuth();

    m_biometricAuthWidget->stopAuth();
}

void LoginWindow::onOtherDevicesButtonClicked()
{
    m_biometricAuthWidget->stopAuth();

    m_backButton->hide();
    m_userWidget->hide();
    m_passwdWidget->hide();
    m_biometricAuthWidget->hide();
    m_buttonsWidget->hide();
    m_biometricDevicesWidget->show();
}

void LoginWindow::onRetryButtonClicked()
{
    m_biometricAuthWidget->startAuth(m_deviceInfo, m_uid);
    m_retryButton->setVisible(false);
}
