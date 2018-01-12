#include "userwindow.h"
#include "usersmodel.h"
#include <QMouseEvent>
#include <QDebug>
#include <QPalette>
#include<QHBoxLayout>
#include "globalv.h"
UserWindow::UserWindow(QWidget *parent)
    : QWidget(parent), m_userList(nullptr)
{
    initUI();
}

void UserWindow::initUI()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("this"));
    this->resize(1100*scale, 350*scale);
    this->setMaximumSize(QSize(1200*scale, 400*scale));

    m_userList = new PageListView(this);
    m_userList->setObjectName(QStringLiteral("m_userList"));
//    m_userList->setGeometry(QRect(150*scale, 25*scale, 901*scale, 251*scale));
    connect(m_userList, SIGNAL(switchPage(int)), this, SLOT(onSwitchPage(int)));
    connect(m_userList, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));

    m_prevLabel = new QLabel(this);
    m_prevLabel->setObjectName(QStringLiteral("m_prevLabel"));
//    m_prevLabel->setGeometry(QRect(100*scale, 65*scale, 64*scale, 128*scale));
    m_prevLabel->installEventFilter(this);
    m_prevLabel->setPixmap(QPixmap(":/resource/prev.png"));
//    m_prevLabel->setStyleSheet("QLabel:hover{background-color:#FF7835;}");

    m_nextLabel = new QLabel(this);
    m_nextLabel->setObjectName(QStringLiteral("m_nextLabel"));
//    m_nextLabel->setGeometry(QRect(1050*scale, 65*scale, 64*scale, 128*scale));
    m_nextLabel->installEventFilter(this);
    m_nextLabel->setPixmap(QPixmap(":/resource/next.png"));
//    m_nextLabel->setStyleSheet("QLabel:hover{background-color:#FF7835;}");

//    m_pageIndicator = new QLabel(this);
//    m_pageIndicator->setObjectName(QStringLiteral("m_pageIndicator"));
//    m_pageIndicator->setGeometry(QRect(450*scale, 300*scale, 300*scale, 20*scale));
//    m_pageIndicator->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_prevLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(m_userList, 0, Qt::AlignTop);
    layout->addWidget(m_nextLabel, 0, Qt::AlignTop);
}

bool UserWindow::eventFilter(QObject *obj, QEvent *event)
{

    if(obj == m_prevLabel)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_prevLabel->setPixmap(QPixmap(":/resource/prev_hl.png"));
                m_prevLabel->setStyleSheet("QLabel{background:transparent;}");
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_prevLabel->setPixmap(QPixmap(":/resource/prev.png"));
                m_userList->pageUp();
                return true;
            }
        }
    }
    if(obj == m_nextLabel)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_nextLabel->setPixmap(QPixmap(":/resource/next_hl.png"));
                m_nextLabel->setStyleSheet("QLabel{background:transparent;}");
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_nextLabel->setPixmap(QPixmap(":/resource/next.png"));
                m_userList->pageDown();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserWindow::setModel(QSharedPointer<UsersModel> model)
{
    if(m_userList){
        m_userList->setModel(model);
    }
}

void UserWindow::onSwitchPage(int)
{
    qDebug() << "switch page";
//    if(!m_userList->hasNext())
//    {
//        m_userList->removeEventFilter(this);
//    }

}

void UserWindow::onLoggedIn(const QModelIndex& index)
{
    emit loggedIn(index);
}
