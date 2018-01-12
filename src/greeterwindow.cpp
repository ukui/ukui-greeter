#include "greeterwindow.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "usersmodel.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include "globalv.h"

float scale;

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent), m_model(new UsersModel(this)),
      m_greeter(new GreeterWrapper(this))
{
    if(m_greeter.data()->hasGuestAccountHint())    //允许游客登录
        m_model.data()->setShowGuest(true);
    if(m_greeter.data()->showManualLoginHint())    //允许手动输入用户名
        m_model.data()->setShowManualLogin(true);

    initUI();
}

GreeterWindow::~GreeterWindow()
{
    //#TODO: 断开连接
}

void GreeterWindow::initUI()
{
    QRect screen = QApplication::desktop()->rect();
    if(screen.width() >= 1920)
        scale = 0.63;
    else{
        scale = screen.width() / 1920.0;
    }
    qDebug() <<"screen:" << screen.width() << " "<< screen.height()<< " scale: "<< scale;

    //背景
    setGeometry(screen);
    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap background(":/resource/background.png");
    background = background.scaled(screen.width(), screen.height(),  Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    palette.setBrush(QPalette::Window, QBrush(background));
    this->setPalette(palette);

    m_layout = new QStackedLayout(this);

    m_firstWnd = new QWidget(this);
    m_firstWnd->setGeometry(screen);
    m_secondWnd = new QWidget(this);
    m_secondWnd->setGeometry(screen);

    for(int i = 0; i < 3; i++)
    {
        QStandardItem *item = new QStandardItem("test" + QString::number(i));
        m_model.data()->extraRowModel()->appendRow(item);
    }

    m_userWnd = new UserWindow(m_firstWnd);
    m_userWnd->setModel(m_model);
    QRect userRect(m_firstWnd->rect().width()/2 - 600*scale, m_firstWnd->rect().height()/2 - 175*scale, 1200*scale, 350*scale);
    m_userWnd->setGeometry(userRect);
    connect(m_userWnd, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));

    m_loginWnd = new LoginWindow(m_greeter, m_secondWnd);
    m_loginWnd->setModel(m_model);
    QRect loginRect(m_secondWnd->rect().width()/2 - 300*scale, m_secondWnd->rect().height()/2 - 90*scale, 600*scale, 180*scale);
    m_loginWnd->setGeometry(loginRect);
    connect(m_loginWnd, SIGNAL(back()), this, SLOT(onBack()));

    m_layout->addWidget(m_firstWnd);
    m_layout->addWidget(m_secondWnd);
    m_layout->setCurrentWidget(m_firstWnd);
}

void GreeterWindow::onLoggedIn(const QModelIndex &index)
{

    QString name = index.data().toString();
    if(name == tr("Guest")){
        //直接登录
    }
    m_loginWnd->setIndex(index);
    m_layout->setCurrentWidget(m_secondWnd);
}

void GreeterWindow::onBack()
{
    m_layout->setCurrentWidget(m_firstWnd);
}
