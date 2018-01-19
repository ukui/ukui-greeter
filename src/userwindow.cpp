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
    this->resize(1050*scale, 300*scale);
    this->setMaximumSize(QSize(1200*scale, 400*scale));

    m_userList = new PageListView(this);
    m_userList->setObjectName(QStringLiteral("m_userList"));
    QRect listRect(64*scale, 25*scale, 900*scale, 300*scale);
    m_userList->setGeometry(listRect);
    connect(m_userList, SIGNAL(pageChanged()), this, SLOT(onPageChanged()));
    connect(m_userList, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));

    m_prevLabel = new QLabel(this);
    m_prevLabel->setObjectName(QStringLiteral("m_prevLabel"));
    QRect prevRect(0, 65*scale, 64*scale, 128*scale);
    m_prevLabel->setGeometry(prevRect);
    m_prevLabel->installEventFilter(this);
    m_prevLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/prev.png"));
    m_prevLabel->setStyleSheet("QLabel{background-color: rgba(255, 255, 255, 0.1)}"
                               "QLabel::hover{background-color: rgba(255, 255, 255, 0.2)}");
//    m_prevLabel->hide();

    m_nextLabel = new QLabel(this);
    m_nextLabel->setObjectName(QStringLiteral("m_nextLabel"));
    QRect nextRect(listRect.right()+20*scale, 65*scale, 64*scale, 128*scale);
    m_nextLabel->setGeometry(nextRect);
    m_nextLabel->installEventFilter(this);
    m_nextLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/next.png"));
    m_nextLabel->setStyleSheet("QLabel{background-color: rgba(255, 255, 255, 0.1)}"
                               "QLabel::hover{background-color: rgba(255, 255, 255, 0.3)}");
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
//                m_prevLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/prev_hl.png"));
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
//                m_prevLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/prev.png"));
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
//                m_nextLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/next_hl.png"));
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
//                m_nextLabel->setPixmap(scaledPixmap(64*scale, 64*scale, ":/resource/next.png"));
                m_userList->pageDown();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserWindow::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if(m_userList){
        m_userList->setModel(model);
    }
}

void UserWindow::onPageChanged()
{
    if(m_userList->hasPrev()) {
        m_prevLabel->show();
    } else {
        m_prevLabel->hide();
    }
    if(m_userList->hasNext()) {
        m_nextLabel->show();
    } else {
        m_nextLabel->hide();
    }
}

void UserWindow::onLoggedIn(const QModelIndex& index)
{
    emit loggedIn(index);
}
