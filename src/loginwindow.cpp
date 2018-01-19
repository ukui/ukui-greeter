#include "loginwindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QLightDM/SessionsModel>
#include "globalv.h"

LoginWindow::LoginWindow(QSharedPointer<GreeterWrapper> greeter, QWidget *parent)
    : QWidget(parent), m_greeter(greeter),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions, this))
{    
    initUI();
    connect(m_greeter.data(), SIGNAL(showMessage(QString,QLightDM::Greeter::MessageType)),
            this, SLOT(showMessage_cb(QString,QLightDM::Greeter::MessageType)));
    connect(m_greeter.data(), SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this, SLOT(showPrompt_cb(QString,QLightDM::Greeter::PromptType)));
    connect(m_greeter.data(), SIGNAL(authenticationComplete()),
            this, SLOT(authenticationComplete_cb()));
    connect(m_greeter.data(), SIGNAL(autologinTimerExpired()),
            this, SLOT(autologinTimerExpired_cb()));
    connect(m_greeter.data(), SIGNAL(reset()), this, SLOT(reset_cb()));
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

    m_sessionLabel = new QLabel(this);
    m_sessionLabel->setObjectName(QStringLiteral("m_sessionLabel"));
    m_sessionLabel->setGeometry(QRect(width()-22, 0, 22, 22));
    m_sessionLabel->installEventFilter(this);

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
    QRect pwdRect(220, 90, 300, 40);
    m_passwordEdit->setGeometry(pwdRect);
    m_passwordEdit->resize(QSize(300, 40));
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)), this, SLOT(login_cb(const QString&)));
}

bool LoginWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_backLabel) {
        if(event->type() == QEvent::MouseButtonPress) {
            QPixmap back_active(":/resource/arrow_left_active.png");
            back_active = back_active.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            m_backLabel->setPixmap(back_active);
            return true;
        }
        if(event->type() == QEvent::MouseButtonRelease) {
            QPixmap back_image(":/resource/arrow_left.png");
            back_image = back_image.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            m_backLabel->setPixmap(back_image);
            //清空当前连接
            m_nameLabel->setText("");
            m_isLoginLabel->setText("");
            m_messageLabel->setText("");
            m_passwordEdit->setType(QLineEdit::Password);
            emit back(1);
            return true;
        }
    } else if(obj == m_sessionLabel) {
        if(event->type() == QEvent::MouseButtonRelease) {
            emit selectSession(m_session);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void LoginWindow::setUsersModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull())
        return;
    m_usersModel = model;
}

bool LoginWindow::setUserIndex(const QModelIndex& index)
{
    if(!index.isValid()){
        return false;
    }
    //设置用户名
    QString name = index.data(Qt::DisplayRole).toString();
    m_nameLabel->setText(name);

    //设置头像
    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    QFile faceFile(facePath);
    QPixmap face;
    if(faceFile.exists())
        face.load(facePath);
    else
        face.load(":/resource/default_face.png");
    face = face.scaled(128, 128, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_faceLabel->setPixmap(face);

    //显示是否已经登录
    bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
    m_isLoginLabel->setText(islogin ? tr("logged in") : "");

    //显示session图标
    if(!m_sessionsModel.isNull() && m_sessionsModel->rowCount() > 1) {
        setSession(index.data(QLightDM::UsersModel::SessionRole).toString());
    }

    //用户认证
    if(name == tr("Guest")) {                       //游客登录
        //m_greeter->authenticateAsGuest();
    }
    else if(name == tr("Login")) {                  //手动输入用户名
        m_passwordEdit->setPrompt(tr("Username"));
        m_passwordEdit->setType(QLineEdit::Normal);
    }
    else {
        m_greeter->authenticate(name);
    }

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

void LoginWindow::setSession(QString sessionName)
{
    QString sessionIcon;
    //有些用户没有设置session，为空，如果系统有Ubuntu session，则设为默认session；
    //否则，则选择第一个session为默认session
    if(sessionName.isEmpty()) {
        for(int i = 0; i < m_sessionsModel->rowCount(); i++) {
            QModelIndex index = m_sessionsModel->index(i, 0);
            if(index.data(Qt::DisplayRole).toString().toLower() == "ubuntu") {
                sessionName = "ubuntu";
                break;
            }
        }
        if(sessionName.isEmpty()) {
            sessionName = m_sessionsModel->index(0, 0).data().toString();
        }
    }
    m_session = sessionName;
    if(sessionName.toLower() == "ubuntu") {
        sessionIcon = ":/resource/ubuntu_badge.png";
    } else if (sessionName.toLower() == "gnome") {
        sessionIcon = ":/resource/ubuntu_badge.png";
    } else if (sessionName.toLower() == "kde") {
        sessionIcon = ":/resource/ubuntu_badge.png";
    } else {
        sessionIcon = ":/resource/unknown_badge.png";
    }
    m_sessionLabel->setPixmap(scaledPixmap(22, 22, sessionIcon));
}

void LoginWindow::onSessionChanged(const QString &sessionName)
{
    qDebug() << "select session: " << sessionName;
    m_session = sessionName;
    setSession(m_session);
}

void LoginWindow::login_cb(const QString &str)
{
    qDebug()<< "login: " << str;
    QString name = m_nameLabel->text();
    if(name == tr("Login")) {
        m_nameLabel->setText(str);
        m_passwordEdit->setText("");
        m_passwordEdit->setType(QLineEdit::Password);
        m_greeter->authenticate(str);
    }
    else {
        m_greeter.data()->respond(str);
    }
    m_passwordEdit->setText("");
}

void LoginWindow::showPrompt_cb(QString text, QLightDM::Greeter::PromptType type)
{
    qDebug()<< "prompt: "<< text;
    m_passwordEdit->setPrompt(text);
    update();   //不更新的话，第一次不会显示prompt
}

void LoginWindow::showMessage_cb(QString text, QLightDM::Greeter::MessageType type)
{
    qDebug()<< "message: "<< text;
    addMessage(text);
}

void LoginWindow::authenticationComplete_cb()
{
    if(m_greeter.data()->isAuthenticated())
    {
        qDebug()<< "authentication success";
        m_greeter.data()->startSessionSync("ubuntu");
        exit(0);
    }
    else
    {
        qDebug() << "authentication failed";
        m_greeter.data()->authenticate(m_nameLabel->text());
        addMessage(tr("password error, please input again"));
        m_passwordEdit->clear();
    }
}

void LoginWindow::autologinTimerExpired_cb()
{

}

void LoginWindow::reset_cb()
{

}

void LoginWindow::addMessage(const QString &text)
{
    m_messageLabel->setText(text);
}
