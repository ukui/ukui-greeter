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
#include <QAbstractItemModel>


#include "greeterwrapper.h"
#include "iconedit.h"
#include "globalv.h"
#include "configuration.h"

LoginWindow::LoginWindow(GreeterWrapper *greeter, QWidget *parent)
    : QWidget(parent),
      m_greeter(greeter),
      m_timer(new QTimer(this)),
      isManual(false),
      isPasswordError(false)
{    
    initUI();

    connect(m_greeter, SIGNAL(showMessage(QString,QLightDM::Greeter::MessageType)),
            this, SLOT(onShowMessage(QString,QLightDM::Greeter::MessageType)));
    connect(m_greeter, SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this, SLOT(onShowPrompt(QString,QLightDM::Greeter::PromptType)));
    connect(m_greeter, SIGNAL(authenticationComplete()),
            this, SLOT(onAuthenticationComplete()));

    m_timer->setInterval(100);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePixmap()));
}

void LoginWindow::initUI()
{
    setFixedWidth(400);

    m_userWidget = new QWidget(this);
    m_userWidget->setObjectName(QStringLiteral("userWidget"));

    /* 头像 */
    m_faceLabel = new QLabel(m_userWidget);
    m_faceLabel->setObjectName(QStringLiteral("faceLabel"));
    m_faceLabel->setFocusPolicy(Qt::NoFocus);

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
    m_passwordEdit->setFocusPolicy(Qt::StrongFocus);
    m_passwordEdit->installEventFilter(this);
    m_passwordEdit->hide(); //收到请求密码的prompt才显示出来
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)),
            this, SLOT(onLogin(const QString&)));

    /* 密码认证信息显示列表 */
    m_messageList = new QListWidget(m_passwdWidget);
    m_messageList->setObjectName(QStringLiteral("messageListWidget"));
    m_messageList->setFocusPolicy(Qt::NoFocus);
    m_messageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_messageList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_messageList->setSelectionMode(QAbstractItemView::NoSelection);

    /* 返回按钮 */
    m_backButton = new QPushButton(this);
    m_backButton->setObjectName(QStringLiteral("backButton"));
    m_backButton->setFocusPolicy(Qt::NoFocus);
    connect(m_backButton, &QPushButton::clicked,
            this, &LoginWindow::onBackButtonClicked);
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
    int userWidgetHeight = 175;
    if(!m_isLoginLabel->text().isEmpty())
    {
        userWidgetHeight += 20;
    }
    m_userWidget->setGeometry(0, (height() - userWidgetHeight - 150) / 2,
                              width(), userWidgetHeight);

    m_faceLabel->setGeometry((width() - 128) / 2, 0, 128, 128);
    m_nameLabel->setGeometry(0, m_faceLabel->geometry().bottom() + 5,
                             width(), 40);
    m_isLoginLabel->setGeometry(0, m_nameLabel->geometry().bottom(),
                                width(), 20);

    // 密码框和提示信息显示位置
    m_passwdWidget->setFixedSize(width(), 150);
    m_passwordEdit->setGeometry((m_passwdWidget->width() - 400)/2, 0, 400, 40);
    m_messageList->setGeometry((m_passwdWidget->width() - 300)/2,
                               m_passwordEdit->geometry().bottom()+3,
                               300, 100);

    m_backButton->setGeometry(0, m_userWidget->y(), 32, 32);
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
    m_passwordEdit->setWaiting(false);
    clearMessage();
}

void LoginWindow::clearMessage()
{
    m_messageList->clear();
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

void LoginWindow::startAuthentication()
{
    //用户认证
    if(m_name == "*guest")
    {                       //游客登录
        qDebug() << "guest login";
        m_passwordEdit->show();
        m_passwordEdit->showIconButton("*");
        m_passwordEdit->setWaiting(true);
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
    m_passwordEdit->setWaiting(true);   //等待认证结果期间不能再输入密码
    m_backButton->setEnabled(false);
    m_timer->start();
}

void LoginWindow::stopWaiting()
{
    m_timer->stop();
    m_passwordEdit->setWaiting(false);
    m_passwordEdit->setIcon(tr("Login"));
    m_backButton->setEnabled(true);
}

void LoginWindow::updatePixmap()
{
    QMatrix matrix;
    matrix.rotate(90.0);
    m_waiting = m_waiting.transformed(matrix, Qt::FastTransformation);
    m_passwordEdit->setIcon(QIcon(m_waiting));
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
    }
    else
    {  //发送密码
        m_greeter->respond(str);
        m_waiting.load(":/resource/waiting.png");
        m_passwordEdit->showIconButton("*");  //当没有输入密码登录时，也显示等待提示
        m_passwordEdit->setIcon(QIcon(m_waiting));
        startWaiting();
    }
    m_passwordEdit->setText("");
}

void LoginWindow::onShowPrompt(QString text, QLightDM::Greeter::PromptType type)
{
    qDebug()<< "prompt: "<< text;

    if(m_timer->isActive())
        stopWaiting();
    if(!text.isEmpty())
        m_passwordEdit->show();

    m_passwdWidget->move(0, m_userWidget->geometry().bottom());

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

void LoginWindow::onShowMessage(QString text, QLightDM::Greeter::MessageType type)
{
    qDebug()<< "message: "<< text;

    QListWidgetItem *item = new QListWidgetItem(text, m_messageList);
    if(type == QLightDM::Greeter::MessageTypeError)
    {
        item->setForeground(QColor(255, 0, 0, 180));
    }
    item->setTextAlignment(Qt::AlignCenter);

    m_messageList->addItem(item);
}

void LoginWindow::onAuthenticationComplete()
{
    stopWaiting();
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
            isPasswordError = true;
        startAuthentication();
    }
}

