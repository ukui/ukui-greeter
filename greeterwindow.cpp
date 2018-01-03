#include "greeterwindow.h"
#include "iconedit.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "ui_login.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>
#include "pagelistview.h"
#include "usersmodel.h"

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent)
{
    QRect screen = QApplication::desktop()->rect();
    setGeometry(screen);
    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap background("/usr/share/kylin-greeter/background.png");
    background = background.scaled(screen.width(), screen.height(),  Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    palette.setBrush(QPalette::Window, QBrush(background));
    this->setPalette(palette);

    m_layout = new QStackedLayout(this);

    m_firstWnd = new QWidget(this);
    m_firstWnd->setGeometry(screen);
    m_secondWnd = new QWidget(this);
    m_secondWnd->setGeometry(screen);

    m_userWnd = new UserWindow(m_firstWnd);
    QRect userRect(m_firstWnd->rect().width()/2 - 600, m_firstWnd->rect().height()/2 - 175, 1200, 350);
    m_userWnd->setGeometry(userRect);
    connect(m_userWnd, SIGNAL(loggedIn(QString)), this, SLOT(onLoggedIn(QString)));

    m_loginWnd = new LoginWindow(m_secondWnd);
    QRect loginRect(m_secondWnd->rect().width()/2 - 300, m_secondWnd->rect().height()/2 - 90, 600, 180);
    m_loginWnd->setGeometry(loginRect);

    m_layout->addWidget(m_firstWnd);
    m_layout->addWidget(m_secondWnd);
    m_layout->setCurrentWidget(m_firstWnd);
}

GreeterWindow::~GreeterWindow()
{

}

void GreeterWindow::onLoggedIn(const QString &name)
{
    delete m_userWnd;
    m_userWnd = nullptr;
    m_layout->setCurrentWidget(m_secondWnd);
}
