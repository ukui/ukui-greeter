#include "loginwindow.h"
#include "tipedit.h"
#include <QGridLayout>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form), m_greeter(new QLightDM::Greeter(this))
{    
    ui->setupUi(this);
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

    if(m_greeter->connectSync())
        qDebug()<< "connect to daemon";
    else
    {
        qDebug()<< "connect to daemon failed!";
        exit(1);
    }
    //m_greeter->authenticate(ui->nameLB->text());

}

void LoginWindow::initUI()
{
    QPixmap backImg("/usr/share/kylin-greeter/arrow_left.png");
    backImg = backImg.scaled(ui->backLB->width(), ui->backLB->height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ui->backLB->setPixmap(backImg);
    //ui->backLB->setStyleSheet("QLabel:hover{background-image:url('/usr/share/kylin-greeter/arrow_left_active.png'), 30, 30}");

    QPixmap faceImg("/usr/share/pixmaps/faces/baseball.png");
    faceImg = faceImg.scaled(ui->faceLB->width(), ui->faceLB->height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ui->faceLB->setPixmap(faceImg);
    ui->faceLB->setStyleSheet("QLabel{ border: 2px solid white}");

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::white);

    ui->nameLB->setText("test");
    ui->nameLB->setPalette(plt);

    ui->isLoginLB->setText("已登录");
    ui->isLoginLB->setPalette(plt);

    m_passwordEdit = new IconEdit(QIcon("/usr/share/kylin-greeter/arrow_right.png"), this);
    QRect pwdRect(220, 90, 300, 40);
    m_passwordEdit->setGeometry(pwdRect);
    m_passwordEdit->resize(QSize(300, 40));
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)), this, SLOT(login_cb(const QString&)));
/*
    TipEdit *edit = new TipEdit(this);
    QRect pwdRect(220, 90, 300, 40);
    edit->setGeometry(pwdRect);
    edit->resize(QSize(300, 40));
*/
    /*
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(0);

    QPushButton *backButton = new QPushButton(this);
    backButton->setIcon(QIcon("/usr/share/kylin-greeter/arrow_left_prelight.png"));
    QRect backBtnRect(0, 0, 60, 60);
    backButton->setFixedSize(60, 60);
    backButton->setIconSize(QSize(60, 60));
    backButton->setGeometry(backBtnRect);

    m_faceLabel = new QLabel(this);
    QPixmap face("/usr/share/pixmaps/faces/baseball.png");
    face = face.scaled(130, 130, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);     //按比例缩放图片，并保证图片光滑
    m_faceLabel->setPixmap(face);
    QRect faceLbRect(backBtnRect.right()+20, backBtnRect.top(), 130, 130);
    //m_faceLabel->setFixedSize(130, 130);
    m_faceLabel->setStyleSheet("QLabel{ border: 2px solid white}");
    m_faceLabel->setGeometry(faceLbRect);

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::red);

    m_userLabel = new QLabel(this);
    m_userLabel->setText("yh");
    QRect userLbRect(faceLbRect.right()+20, backBtnRect.top(), 200, 35);
    //m_userLabel->setFixedSize(200, 35);
    m_userLabel->setGeometry(userLbRect);
    m_userLabel->setPalette(plt);

    m_isLoginLabel = new QLabel(this);
    m_isLoginLabel->setText("已登录");
    QRect loginLbRect(faceLbRect.right()+5, userLbRect.bottom()+5, 200, 35);
    //m_isLoginLabel->setFixedSize(200, 35);
    m_userLabel->setGeometry(loginLbRect);
    m_isLoginLabel->setPalette(plt);

    m_messageLabel = new QLabel(this);
    QRect msgLbRect(faceLbRect.right()+5, loginLbRect.bottom()+5, 200, 35);
    m_messageLabel->setPalette(plt);
    m_messageLabel->setGeometry(msgLbRect);

    m_passwordEdit = new IconEdit(QIcon("/usr/share/kylin-greeter/arrow_right.png"), this);
    QRect pwdRect(faceLbRect.right()+5, msgLbRect.bottom()+5, 300, 40);
    //m_passwordEdit->resize(QSize( 300, 40));
    m_userLabel->setGeometry(pwdRect);
    connect(m_passwordEdit, SIGNAL(clicked(const QString&)), this, SLOT(login_cb(const QString&)));
    */
/*
    mainLayout->addWidget(m_faceLabel, 0, 0, 4, 4);
    mainLayout->addWidget(m_userLabel, 0, 3, 1, 6);
    mainLayout->addWidget(m_isLoginLabel, 1, 3, 1, 6);
    mainLayout->addWidget(m_messageLabel, 2, 3, 1, 6);
    mainLayout->addWidget(m_passwordEdit, 3, 3, 1, 6);
*/
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
        m_greeter->authenticate(ui->nameLB->text());
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
    ui->msgLB->setText(text);
}
