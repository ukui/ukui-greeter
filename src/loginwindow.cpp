#include "loginwindow.h"
#include <QMouseEvent>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), m_greeter(new QLightDM::Greeter(this))
{    
    initUI();

    connect(m_greeter, SIGNAL(showMessage(QString,QLightDM::Greeter::MessageType)),
            this, SLOT(showMessage_cb(QString,QLightDM::Greeter::MessageType)));
    connect(m_greeter, SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),
            this, SLOT(showPrompt_cb(QString,QLightDM::Greeter::PromptType)));
    connect(m_greeter, SIGNAL(authenticationComplete()),
            this, SLOT(authenticationComplete_cb()));
    connect(m_greeter, SIGNAL(autologinTimerExpired()),
            this, SLOT(autologinTimerExpired_cb()));
    connect(m_greeter, SIGNAL(reset()), this, SLOT(reset_cb()));
/*
    if(m_greeter->connectSync())
        qDebug()<< "connect to daemon";
    else
    {
        qDebug()<< "connect to daemon failed!";
        exit(1);
    }
*/
    //m_greeter->authenticate(m_nameLabel->text());

}

void LoginWindow::initUI()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("this"));
    this->resize(583, 162);

    m_backLabel = new QLabel(this);
    m_backLabel->setObjectName(QStringLiteral("m_backLabel"));
    m_backLabel->setGeometry(QRect(0, 0, 32, 32));
    m_backLabel->setPixmap(QPixmap(":/resource/arrow_left.png"));
    m_backLabel->installEventFilter(this);

    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QStringLiteral("m_faceLabel"));
    m_faceLabel->setGeometry(QRect(60, 0, 130, 130));
    m_faceLabel->setStyleSheet("QLabel{ border: 2px solid white}");

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::white);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QStringLiteral("m_nameLabel"));
    m_nameLabel->setGeometry(QRect(220, 0, 200, 30));
    m_nameLabel->setPalette(plt);
    m_nameLabel->setFont(QFont("ubuntu", 14));

    m_isLoginLabel = new QLabel(this);
    m_isLoginLabel->setObjectName(QStringLiteral("m_isLoginLabel"));
    m_isLoginLabel->setGeometry(QRect(220, 35, 200, 20));
    m_isLoginLabel->setPalette(plt);

    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName(QStringLiteral("m_messageLabel"));
    m_messageLabel->setGeometry(QRect(220, 60, 300, 20));
    m_isLoginLabel->setPalette(plt);

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
            m_backLabel->setPixmap(QPixmap(":/resource/arrow_left_active.png"));
            return true;
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            m_backLabel->setPixmap(QPixmap(":/resource/arrow_left.png"));
            emit back();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void LoginWindow::setModel(QAbstractItemModel *model)
{
    if(!model)
        return;
    m_model = QWeakPointer<QAbstractItemModel>(model);
    //#TODO: 连接lightdm
}

bool LoginWindow::setIndex(int row)
{
    if(row >= m_model.data()->rowCount())
        return false;

    QModelIndex index = m_model.data()->index(row, 0);
    QString name = index.data(Qt::DisplayRole).toString();
    m_nameLabel->setText(name);

    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    QFile faceFile(facePath);
    QPixmap face;
    if(faceFile.exists())
        face.load(facePath);
    else
        face.load(":/resource/default_face.png");
    face = face.scaled(m_faceLabel->width(), m_faceLabel->height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_faceLabel->setPixmap(face);

    bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
    m_isLoginLabel->setText(islogin ? tr("logged in") : "");
    return true;
}

void LoginWindow::login_cb(const QString &str)
{
    qDebug() << "login: " << str;
    m_greeter->respond(str);
}

void LoginWindow::showPrompt_cb(QString text, QLightDM::Greeter::PromptType type)
{
    qDebug()<< "prompt: "<< text;
    m_passwordEdit->setPrompt(text);
}

void LoginWindow::showMessage_cb(QString text, QLightDM::Greeter::MessageType type)
{
    qDebug()<< "message: "<< text;
    addMessage(text);
}

void LoginWindow::authenticationComplete_cb()
{
    qDebug() << "authentication complete";
    if(m_greeter->isAuthenticated())
    {
        qDebug()<< "authentication success";
        m_greeter->startSessionSync("ubuntu");
        exit(0);
    }
    else
    {
        m_greeter->authenticate(m_nameLabel->text());
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
