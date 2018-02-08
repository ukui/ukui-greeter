#include "loginwindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>
#include <QScreen>
#include <QProcess>
#include <QFocusEvent>
#include <QLightDM/SessionsModel>
#include "globalv.h"

LoginWindow::LoginWindow(QSharedPointer<GreeterWrapper> greeter, QWidget *parent)
    : QWidget(parent), m_greeter(greeter),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions, this)),
      m_config(new QSettings(configFile, QSettings::IniFormat)),
      m_timer(new QTimer(this))
{    
    initUI();
    connect(m_greeter.data(), SIGNAL(showMessage(QString,QLightDM::Greeter::MessageType)),
            this, SLOT(onShowMessage(QString,QLightDM::Greeter::MessageType)));
    connect(m_greeter.data(), SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this, SLOT(onShowPrompt(QString,QLightDM::Greeter::PromptType)));
    connect(m_greeter.data(), SIGNAL(authenticationComplete()),
            this, SLOT(onAuthenticationComplete()));

    m_timer->setInterval(100);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePixmap()));
}

void LoginWindow::initUI()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("this"));
    this->resize(520, 135);

    m_backLabel = new QLabel(this);
    m_backLabel->setObjectName(QStringLiteral("m_backLabel"));
    m_backLabel->setGeometry(QRect(0, 0, 32, 32));
    QPixmap back(":/resource/arrow_left.png");
    back = back.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_backLabel->setPixmap(back);
    m_backLabel->installEventFilter(this);

    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QStringLiteral("m_faceLabel"));
    m_faceLabel->setGeometry(QRect(60, 0, 132, 132));
    m_faceLabel->setStyleSheet("QLabel{ border: 2px solid white}");

    m_sessionLabel = new QSvgWidget(this);
    m_sessionLabel->setObjectName(QStringLiteral("m_sessionLabel"));
    m_sessionLabel->setGeometry(QRect(width()-22, 0, 22, 22));
    m_sessionLabel->installEventFilter(this);
    m_sessionLabel->hide();

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::white);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QStringLiteral("m_nameLabel"));
    QRect nameRect(220, 0, 200, 25);
    m_nameLabel->setGeometry(nameRect);
    m_nameLabel->setPalette(plt);
    m_nameLabel->setFont(QFont("ubuntu", fontSize+2));

    m_isLoginLabel = new QLabel(this);
    m_isLoginLabel->setObjectName(QStringLiteral("m_isLoginLabel"));
    QRect loginRect(220, nameRect.bottom()+5, 200, 30);
    m_isLoginLabel->setGeometry(loginRect);
    m_isLoginLabel->setPalette(plt);
    m_isLoginLabel->setFont(QFont("ubuntu", fontSize));

    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName(QStringLiteral("m_messageLabel"));
    m_messageLabel->setGeometry(QRect(220, 60, 300, 20));
    plt.setColor(QPalette::WindowText, Qt::red);
    m_messageLabel->setPalette(plt);

    m_passwordEdit = new IconEdit(QIcon(":/resource/arrow_right.png"), this);
    m_passwordEdit->setObjectName("m_passwordEdit");
    QRect pwdRect(220, 90, 300, 40);
    m_passwordEdit->setGeometry(pwdRect);
    m_passwordEdit->resize(QSize(300, 40));
    m_passwordEdit->installEventFilter(this);
    m_passwordEdit->setStyleSheet("QWidget{background-color:rgb(255, 255, 255, 150)}");
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)), this, SLOT(onLogin(const QString&)));
}

bool LoginWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_backLabel) {
        if(!m_backLabel->isEnabled())
            return true;
        if(event->type() == QEvent::MouseButtonPress) {
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left_active.png"));
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease) {
            //返回到用户列表窗口
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left.png"));
                //清空当前用户信息
                m_nameLabel->setText("");
                m_isLoginLabel->setText("");
                m_messageLabel->setText("");
                m_passwordEdit->clear();
                m_passwordEdit->setType(QLineEdit::Password);
                emit back();
                return true;
            }
        }
    } else if(obj == m_sessionLabel) {
        if(!m_sessionLabel->isEnabled())
            return true;
        if(event->type() == QEvent::MouseButtonRelease) {
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                emit selectSession(m_session);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void LoginWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    m_passwordEdit->setFocus();
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
//    m_passwordEdit->setPrompt(text);
}


void LoginWindow::addMessage(const QString &text)
{
    m_messageLabel->setText(text);
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

/**
 * @brief LoginWindow::setSession
 *
 * 设置session图标
 */
void LoginWindow::setSession(QString session)
{
    QString sessionIcon;

    if(session.isEmpty() || sessionIndex(session) == -1) {
        /* default */
        QString defaultSession = m_greeter->defaultSessionHint();
        if(defaultSession != session && sessionIndex(defaultSession) != -1) {
            session = defaultSession;
        }
        /* first session in session list */
        else if(m_sessionsModel && m_sessionsModel->rowCount() > 0) {
            session = m_sessionsModel->index(0, 0).data().toString();
        }
        else {
            session = "";
        }
    }
    m_session = session;

    sessionIcon = IMAGE_DIR + QString("badges/%1_badge-symbolic.svg").arg(session.toLower());
    m_sessionLabel->load(sessionIcon);
}

/**
 * @brief LoginWindow::session
 * @return
 *
 * 获取session标识
 */
QString LoginWindow::getSession()
{
    QString sessionKey;
    for(int i = 0; i < m_sessionsModel->rowCount(); i++) {
        QString session = m_sessionsModel->index(i, 0).data(Qt::DisplayRole).toString();
        if(m_session == session) {
            sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
            break;
        }
    }
    return sessionKey;
}

void LoginWindow::setUsersModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull())
        return;
    m_usersModel = model;

    if(m_usersModel->rowCount() == 1) {
        m_backLabel->hide();
        setUserIndex(m_usersModel->index(0,0));
    }
}

bool LoginWindow::setUserIndex(const QModelIndex& index)
{
    if(!index.isValid()){
        return false;
    }
    //设置用户名
    QString name = index.data(Qt::DisplayRole).toString();
    setUserName(name);

    //设置头像
    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    setFace(facePath);

    //显示是否已经登录
    bool isLoggedIn = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
    setLoggedIn(isLoggedIn);

    //显示session图标
    if(!m_sessionsModel.isNull() && m_sessionsModel->rowCount() > 1) {
        m_sessionLabel->show();
        setSession(index.data(QLightDM::UsersModel::SessionRole).toString());
    }

    startAuthentication(name);

    return true;
}

void LoginWindow::setSessionsModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull()){
        return ;
    }
    m_sessionsModel = model;
    //如果session有多个，则显示session图标，默认显示用户上次登录的session
    //如果当前还没有设置用户，则默认显示第一个session
    if(m_sessionsModel->rowCount() > 1) {
        if(!m_usersModel.isNull() && !m_nameLabel->text().isEmpty()) {
            for(int i = 0; i < m_usersModel->rowCount(); i++){
                QModelIndex index = m_usersModel->index(i, 0);
                if(index.data(Qt::DisplayRole).toString() == m_nameLabel->text()){
                    setSession(index.data(QLightDM::UsersModel::SessionRole).toString());
                    return;
                }
            }
        }
        setSession(m_sessionsModel->index(0, 0).data().toString());
    }
}

bool LoginWindow::setSessionIndex(const QModelIndex &index)
{
    //显示选择的session（如果有多个session则显示，否则不显示）
    if(!index.isValid()) {
        return false;
    }
    setSession(index.data(Qt::DisplayRole).toString());
    return true;
}

int LoginWindow::sessionIndex(const QString &session)
{
    if(!m_sessionsModel){
        return -1;
    }
    for(int i = 0; i < m_sessionsModel->rowCount(); i++){
        QString sessionName = m_sessionsModel->index(i, 0).data().toString();
        if(session.toLower() == sessionName.toLower()) {
            return i;
        }
    }
    return -1;
}

void LoginWindow::onSessionSelected(const QString &session)
{
    qDebug() << "select session: " << session;
    if(!session.isEmpty() && m_session != session) {
        m_session = session;

        //查找session的标识
        QString sessionName, sessionKey;
        for(int i = 0; i < m_sessionsModel->rowCount(); i++){
            sessionName = m_sessionsModel->index(i, 0).data().toString();
            if(sessionName == session){
                sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
                m_greeter->setSession(sessionKey);
                this->setSession(m_session);
                return;
            }
        }
    }
}

void LoginWindow::startAuthentication(const QString &username)
{
    //用户认证
    if(username == tr("Guest")) {                       //游客登录
        qDebug() << "guest login";
        m_greeter->authenticateAsGuest();
    }
    else if(username == tr("Login")) {                  //手动输入用户名
        m_passwordEdit->setPrompt(tr("Username"));
        m_passwordEdit->setType(QLineEdit::Normal);
    }
    else {
        qDebug() << "login: " << username;
        m_greeter->authenticate(username);
    }
}

/**
 * @brief LoginWindow::startWaiting
 *
 * 等待认证结果
 */
void LoginWindow::startWaiting()
{
//    m_passwordEdit->setWaiting(true);
    m_backLabel->setEnabled(false);
    m_sessionLabel->setEnabled(false);
    m_timer->start();
}

void LoginWindow::stopWaiting()
{
    m_timer->stop();
    m_passwordEdit->setWaiting(false);
    m_passwordEdit->setIcon(":/resource/arrow_right.png");
    m_backLabel->setEnabled(true);
    m_sessionLabel->setEnabled(true);
    m_passwordEdit->showIcon("");
}

void LoginWindow::updatePixmap()
{
    QMatrix matrix;
    matrix.rotate(90.0);
    m_waiting = m_waiting.transformed(matrix, Qt::FastTransformation);
    m_passwordEdit->setIcon(QIcon(m_waiting));
}

void LoginWindow::saveLastLoginUser()
{
    m_config->setValue("lastLoginUser", m_nameLabel->text());
    m_config->sync();
    qDebug() << m_config->fileName();
}

void LoginWindow::onLogin(const QString &str)
{
    m_messageLabel->clear();
    QString name = m_nameLabel->text();
    if(name == tr("Login")) {   //认证用户
        m_nameLabel->setText(str);
        m_passwordEdit->setText("");
        m_passwordEdit->setType(QLineEdit::Password);
        m_greeter->authenticate(str);
        qDebug() << "login: " << name;
    }
    else {  //发送密码
        m_greeter->respond(str);
        m_waiting.load(":/resource/waiting.png");
        m_passwordEdit->showIcon("*");  //当没有输入密码登录时，显示等待提示
        m_passwordEdit->setIcon(QIcon(m_waiting));
        startWaiting();
    }
    m_passwordEdit->setText("");
}

void LoginWindow::onShowPrompt(QString text, QLightDM::Greeter::PromptType type)
{
    qDebug()<< "prompt: "<< text;
    if(type != QLightDM::Greeter::PromptTypeSecret)
        m_passwordEdit->setType(QLineEdit::Normal);
    else
        m_passwordEdit->setType(QLineEdit::Password);

    m_passwordEdit->setPrompt(text);
}

void LoginWindow::onShowMessage(QString text, QLightDM::Greeter::MessageType type)
{
    qDebug()<< "message: "<< text;
    addMessage(text);
}

void LoginWindow::onAuthenticationComplete()
{
    stopWaiting();
    if(m_greeter->isAuthenticated()) {
        qDebug()<< "authentication success";
        m_greeter->startSession();
    } else {
        qDebug() << "authentication failed";
        addMessage(tr("Incorrect password, please input again"));
        m_passwordEdit->clear();
        m_greeter->authenticate(m_nameLabel->text());
    }
}
