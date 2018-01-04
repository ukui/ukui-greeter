#include "greeterwindow.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "usersmodel.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent)
{
    //#TODO: 连接到Lightdm，获取hint

    initUI();
}

GreeterWindow::~GreeterWindow()
{
    //#TODO: 断开连接
}

void GreeterWindow::initUI()
{
    QRect screen = QApplication::desktop()->rect();
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

    m_model = new UsersModel(this);
    for(int i = 0; i < 3; i++)
    {
        QStandardItem *item = new QStandardItem("test" + QString::number(i));
        m_model->extraRowModel()->appendRow(item);
    }

    m_userWnd = new UserWindow(m_firstWnd);
    m_userWnd->setModel(m_model);
    QRect userRect(m_firstWnd->rect().width()/2 - 600, m_firstWnd->rect().height()/2 - 175, 1200, 350);
    m_userWnd->setGeometry(userRect);
    connect(m_userWnd, SIGNAL(loggedIn(QString)), this, SLOT(onLoggedIn(QString)));

    m_loginWnd = new LoginWindow(m_secondWnd);
    m_loginWnd->setModel(m_model);
    QRect loginRect(m_secondWnd->rect().width()/2 - 300, m_secondWnd->rect().height()/2 - 90, 600, 180);
    m_loginWnd->setGeometry(loginRect);
    connect(m_loginWnd, SIGNAL(back()), this, SLOT(onBack()));

    m_layout->addWidget(m_firstWnd);
    m_layout->addWidget(m_secondWnd);
    m_layout->setCurrentWidget(m_firstWnd);
}

void GreeterWindow::onLoggedIn(const QString &name)
{
    int row = -1;
    for(int i = 0; i < m_model->rowCount(QModelIndex()); i++)
    {
        QString iName = m_model->index(i, 0).data(Qt::DisplayRole).toString();
        if(iName == name)
        {
            row = i;
            break;
        }
    }
    m_loginWnd->setIndex(row);
    m_layout->setCurrentWidget(m_secondWnd);
}

void GreeterWindow::onBack()
{
    m_layout->setCurrentWidget(m_firstWnd);
}
