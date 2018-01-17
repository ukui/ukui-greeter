#include "greeterwindow.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "usersmodel.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include "globalv.h"

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent),
      m_model(new UsersModel()),
      m_greeter(new GreeterWrapper())
{
    if(m_greeter.data()->hasGuestAccountHint())    //允许游客登录
        m_model.data()->setShowGuest(true);
    if(m_greeter.data()->showManualLoginHint())    //允许手动输入用户名
        m_model.data()->setShowManualLogin(true);
}

GreeterWindow::~GreeterWindow()
{
    //#TODO: 断开连接
}

void GreeterWindow::initUI()
{
    m_layout = new QStackedLayout(this);

    m_firstWnd = new QWidget(this);
    m_firstWnd->setGeometry(rect());
    m_secondWnd = new QWidget(this);
    m_secondWnd->setGeometry(rect());

    for(int i = 0; i < 3; i++)
    {
        QStandardItem *item = new QStandardItem("test" + QString::number(i));
        m_model.data()->extraRowModel()->appendRow(item);
    }

    m_userWnd = new UserWindow(m_firstWnd);
    m_userWnd->setModel(m_model);
    QRect userRect(rect().width()/2 - 600*scale, rect().height()/2 - 175*scale, 1200*scale, 350*scale);
    m_userWnd->setGeometry(userRect);
    connect(m_userWnd, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));

    m_loginWnd = new LoginWindow(m_greeter, m_secondWnd);
    m_loginWnd->setModel(m_model);
    QRect loginRect(rect().width()/2 - 300, rect().height()/2 - 90, 600, 180);
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
