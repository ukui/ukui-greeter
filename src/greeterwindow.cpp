#include "greeterwindow.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "usersmodel.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QLightDM/SessionsModel>
#include "globalv.h"

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent),
      m_usersModel(new UsersModel()),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions)),
      m_greeter(new GreeterWrapper()),
      m_sessionWnd(nullptr)
{
    if(m_greeter->hasGuestAccountHint())    //允许游客登录
        m_usersModel->setShowGuest(true);
    if(m_greeter->showManualLoginHint())    //允许手动输入用户名
        m_usersModel->setShowManualLogin(true);
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
        m_usersModel->extraRowModel()->appendRow(item);
    }

    m_userWnd = new UserWindow(m_firstWnd);
    m_userWnd->setModel(m_usersModel);
    QRect userRect((rect().width()-m_userWnd->width())/2, (rect().height()-m_userWnd->height())/2,
                   m_userWnd->width(), m_userWnd->height());
    m_userWnd->setGeometry(userRect);
    connect(m_userWnd, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));

    m_loginWnd = new LoginWindow(m_greeter, m_secondWnd);
    m_loginWnd->setUsersModel(m_usersModel);
//    m_loginWnd->setSessionsModel(m_sessionsModel);
    QRect loginRect((rect().width()-m_loginWnd->width())/2, (rect().height()-m_loginWnd->height())/2,
                    m_loginWnd->width(), m_loginWnd->height());
    m_loginWnd->setGeometry(loginRect);
    connect(m_loginWnd, SIGNAL(back(int)), this, SLOT(onBack(int)));
    connect(m_loginWnd, SIGNAL(selectSession(QString)), this, SLOT(onSelectSession(QString)));

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
    m_loginWnd->setUserIndex(index);
    m_layout->setCurrentWidget(m_secondWnd);
}

void GreeterWindow::onBack(int wndIndex)
{
    m_layout->setCurrentIndex(wndIndex-1);
}

void GreeterWindow::onSelectSession(const QString &sessionName)
{
    if(!m_sessionWnd) {
        m_thirdWnd = new QWidget(this);
        m_thirdWnd->setGeometry(rect());
        m_sessionWnd = new SessionWindow(m_thirdWnd);
        m_sessionWnd->setGeometry((rect().width()-m_sessionWnd->width())/2,
                                  (rect().height()-m_sessionWnd->height())/2,
                                  m_sessionWnd->width(), m_sessionWnd->height());
        m_layout->addWidget(m_thirdWnd);
        connect(m_sessionWnd, SIGNAL(sessionChanged(QString)), m_loginWnd, SLOT(onSessionChanged(QString)));
        connect(m_sessionWnd, SIGNAL(back(int)), this, SLOT(onBack(int)));
    }
    m_sessionWnd->setSession(sessionName);
    m_layout->setCurrentWidget(m_thirdWnd);
}
