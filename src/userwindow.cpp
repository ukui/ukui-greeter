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
    this->resize(1100*scale, 400*scale);
    this->setMaximumSize(QSize(1200*scale, 400*scale));

    m_userList = new PageListView(this);
    m_userList->setObjectName(QStringLiteral("m_userList"));
    QRect listRect(150*scale, 25*scale, 900*scale, 300*scale);
    m_userList->setGeometry(listRect);
    connect(m_userList, SIGNAL(switchPage(int)), this, SLOT(onSwitchPage(int)));
    connect(m_userList, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));


    m_prevLabel = new QLabel(this);
    m_prevLabel->setObjectName(QStringLiteral("m_prevLabel"));
    QRect prevRect(listRect.left()-64*scale, 65*scale, 64*scale, 128*scale);
    m_prevLabel->setGeometry(prevRect);
    m_prevLabel->installEventFilter(this);
    QPixmap prevImage(":/resource/prev.png");
    prevImage = prevImage.scaled(32*scale, 64*scale, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_prevLabel->setPixmap(prevImage);

    m_nextLabel = new QLabel(this);
    m_nextLabel->setObjectName(QStringLiteral("m_nextLabel"));
    QRect nextRect(listRect.right(), 70*scale, 64*scale, 128*scale);
    m_nextLabel->setGeometry(nextRect);
    m_nextLabel->installEventFilter(this);
    QPixmap nextImage(":/resource/next.png");
    nextImage = nextImage.scaled(32*scale, 64*scale, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_nextLabel->setPixmap(nextImage);


//    m_pageIndicator = new QLabel(this);
//    m_pageIndicator->setObjectName(QStringLiteral("m_pageIndicator"));
//    m_pageIndicator->setGeometry(QRect(450*scale, 300*scale, 300*scale, 20*scale));
//    m_pageIndicator->setAlignment(Qt::AlignCenter);

//    QHBoxLayout *layout = new QHBoxLayout(this);
//    layout->addWidget(m_prevLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
//    layout->addWidget(m_userList, 0, Qt::AlignTop);
//    layout->addWidget(m_nextLabel, 0, Qt::AlignTop);
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
//                m_prevLabel->setPixmap(QPixmap(":/resource/prev_hl.png"));
                QPixmap prevHLImg = scaledPixmap(32*scale, 64*scale, ":/resource/prev_hl.png");
                m_prevLabel->setPixmap(prevHLImg);
                m_prevLabel->setStyleSheet("QLabel{background:transparent;}");
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_prevLabel->setPixmap(scaledPixmap(32*scale, 64*scale, ":/resource/prev.png"));
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
                m_nextLabel->setPixmap(scaledPixmap(32*scale, 64*scale, ":/resource/next_hl.png"));
                m_nextLabel->setStyleSheet("QLabel{background:transparent;}");
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_nextLabel->setPixmap(scaledPixmap(32*scale, 64*scale, ":/resource/next.png"));
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
