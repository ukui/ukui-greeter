#include "loginwindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include "globalv.h"

LoginWindow::LoginWindow(QSharedPointer<GreeterWrapper> greeter, QWidget *parent)
    : QWidget(parent), m_greeter(greeter)
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
    this->resize(600, 180);

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
    if(obj == m_backLabel)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QPixmap back_active(":/resource/arrow_left_active.png");
            back_active = back_active.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            m_backLabel->setPixmap(back_active);
            return true;
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QPixmap back_image(":/resource/arrow_left.png");
            back_image = back_image.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            m_backLabel->setPixmap(back_image);
            //清空当前连接
            m_nameLabel->setText("");
            m_isLoginLabel->setText("");
            m_messageLabel->setText("");
            m_passwordEdit->setType(QLineEdit::Password);
            emit back();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void LoginWindow::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull())
        return;
    m_model = model;
}

bool LoginWindow::setIndex(const QModelIndex& index)
{
    if(!index.isValid()){
        return false;
    }
    QString name = index.data(Qt::DisplayRole).toString();
    m_nameLabel->setText(name);

    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    QFile faceFile(facePath);
    QPixmap face;
    if(faceFile.exists())
        face.load(facePath);
    else
        face.load(":/resource/default_face.png");
    face = face.scaled(128, 128, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_faceLabel->setPixmap(face);

    bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
    m_isLoginLabel->setText(islogin ? tr("logged in") : "");

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
    qDebug() << "authentication complete";
    if(m_greeter.data()->isAuthenticated())
    {
        qDebug()<< "authentication success";
        m_greeter.data()->startSessionSync("ubuntu");
        exit(0);
    }
    else
    {
        m_greeter.data()->authenticate(m_nameLabel->text());
        addMessage("password error, please input again");
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
