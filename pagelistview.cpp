#include "pagelistview.h"
#include <QPainter>
#include <qmath.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include <algorithm>
#include <functional>
#include "userentry.h"
#include <QLightDM/UsersModel>

PageListView::PageListView(QWidget *parent) : QWidget(parent)
{
    resize(900, 250);
}

bool PageListView::eventFilter(QObject *obj, QEvent *event)
{
    return QWidget::eventFilter(obj, event);
}

void PageListView::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Return:
        {
            qDebug() << "login";
            UserEntry *entry = qobject_cast<UserEntry*>(focusWidget());
            qDebug() << entry->objectName();
            onEntryLogin(entry->userName());
            break;
        }
        case Qt::Key_PageUp:
//            qDebug() << "previous page";
            pageUp();
            break;
        case Qt::Key_PageDown:
//            qDebug() << "next page";
            pageDown();
            break;
        case Qt::Key_Left:
//            qDebug() << "previous";
            preItem();
            break;
        case Qt::Key_Right:
//            qDebug() << "next";
            nextItem();
            break;
        case Qt::Key_Home:
//            qDebug() << "home page";
            goHome();
            break;
        case Qt::Key_End:
//            qDebug() << "end page";
            goEnd();
            break;
        default:
            QWidget::keyReleaseEvent(event);
    }
}

void PageListView::setModel(QAbstractItemModel* model)
{
    if(!model)
        return;
    if(!m_model)
    {

    }
    m_model = model;
    m_pageNum = qCeil(m_model->rowCount() * 1.0 / MAX_NUM_PP);
    m_curPage = 0;

//    drawPage();
    drawPageLayout();
}

void PageListView::drawPageIndicator()
{

}

void PageListView::drawPage()
{
    QPoint midPoint(rect().width()/2, rect().height()/2);

    int w = UserEntry::X();
    int h = UserEntry::Y();
    m_itemNum = m_model->rowCount() - m_curPage * MAX_NUM_PP > 5 ? 5 : m_model->rowCount() - m_curPage * MAX_NUM_PP;

    //* 创建entry */
    if(m_itemNum % 2 == 0)
    {
        for(int i = 0; i < m_itemNum / 2; i++)
        {
            QRect rectL(midPoint.x() - (i + 1) * w, midPoint.y() - h / 2, w, h);
            UserEntry *entryL = new UserEntry(this);
            m_itemList.push_front(entryL);
            entryL->setGeometry(rectL);

            QRect rectR(midPoint.x() + i * w, midPoint.y() - h / 2, w, h);
            UserEntry *entryR = new UserEntry(this);
            m_itemList.append(entryR);
            entryR->setGeometry(rectR);
        }
    }
    else
    {
        QRect rectM(midPoint.x() - w / 2, midPoint.y() - h / 2, w, h);
        UserEntry *entryM = new UserEntry(this);
        m_itemList.append(entryM);
        entryM->setGeometry(rectM);

        for(int i = 0; i < m_itemNum / 2; i++)
        {
            QRect rectL(midPoint.x() - (i+1+0.5) * w, midPoint.y() - h / 2, w, h);
            UserEntry *entryL = new UserEntry(this);
            m_itemList.push_front(entryL);
            entryL->setGeometry(rectL);

            QRect rectR(midPoint.x() + (i+0.5) * w, midPoint.y() - h / 2, w, h);
            UserEntry *entryR = new UserEntry(this);
            m_itemList.append(entryR);
            entryR->setGeometry(rectR);
        }
    }
    /* 设置entry的属性，并将entry添加到list中，监听click信号 */
    for(int i = 0; i < m_itemList.length(); i++)
    {
        UserEntry *entry = m_itemList[i];
        entry->setObjectName("Entry"+QString::number(i));
        connect(entry, SIGNAL(clicked(QString)), this, SLOT(onEntryLogin(QString)));

        QModelIndex index = m_model->index(m_curPage * MAX_NUM_PP + i, 0);
        QString face = index.data(QLightDM::UsersModel::ImagePathRole).toString();
        QString name = index.data(Qt::DisplayRole).toString();
        bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
        entry->setFace(face);
        entry->setUserName(name);
        entry->setLogin(islogin);
        qDebug() << face << " "<< name << " " << islogin;
    }

    m_itemList[0]->setFocus();
    m_curItem = 0;

    /* 绘制左翻页键 */
    if(m_curPage > 0)
    {
        m_preLabel = new QLabel(this);
//        connect(m_preLabel, SIGNAL(clicked()), this, SLOT(pageUp()));
        int x = 64; //label的宽度
        double num = MAX_NUM_PP * 1.0 / 2;
        QRect rect(midPoint.x() - (w * num + x), m_itemList[0]->rect().top() + 100, x, x);
        m_preLabel->setGeometry(rect);
        QPixmap pixmap(":/resource/previous.png");
        m_preLabel->setPixmap(pixmap);
    }
    /* 绘制右翻页键 */
    if(m_curPage < m_pageNum - 1)
    {
        m_nextLabel = new QLabel(this);
//        connect(m_nextLabel, SIGNAL(clicked()), this, SLOT(pageDown()));
        int x = 64;
        double num = MAX_NUM_PP * 1.0 / 2;
        QRect rect(midPoint.x() + (w * num), m_itemList[0]->rect().top() + 100, x, x);
        m_nextLabel->setGeometry(rect);
        QPixmap pixmap(":/resource/next.png");
        m_nextLabel->setPixmap(pixmap);
    }
}

void PageListView::drawPageLayout()
{
    if(m_layout == NULL)
    {
        m_layout = new QHBoxLayout(this);
        m_layout->setSpacing(0);
    }
    m_itemNum = m_model->rowCount() - m_curPage * MAX_NUM_PP > 5 ? 5 : m_model->rowCount() - m_curPage * MAX_NUM_PP;
    qDebug()<< "第" << m_curPage<<  "页， 共" <<  m_itemNum << "项";


    m_layout->addStretch();

    for(int i = 0; i< m_itemNum; i++)
    {
        UserEntry *entry = new UserEntry(this);
        entry->setObjectName("Entry"+QString::number(i));
        connect(entry, SIGNAL(clicked(QString)), this, SLOT(onEntryLogin(QString)));

        QModelIndex index = m_model->index(m_curPage * MAX_NUM_PP + i, 0);
        QString face = index.data(QLightDM::UsersModel::ImagePathRole).toString();
        QString name = index.data(Qt::DisplayRole).toString();
        bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
        qDebug() << face << " "<< name << " " << islogin;
        entry->setFace(face);
        entry->setUserName(name);
        entry->setLogin(islogin);

        m_itemList.append(entry);
        m_layout->addWidget(entry);

    }

    m_layout->addStretch();


    this->setLayout(m_layout);
    m_itemList[0]->setFocus();
    m_curItem = 0;
}

void PageListView::destroyPage()
{
    int count = m_itemList.length();
    for(int i = 0; i< count; i++)
        delete m_itemList[i];
    m_itemList.clear();

    if(m_layout)
    {
        delete m_layout;
        m_layout = NULL;
    }
}

void PageListView::pageUp()
{
    if(m_curPage > 0)
    {
        --m_curPage;
        destroyPage();
        drawPageLayout();
        m_itemList[4]->setFocus();
        m_curItem = 4;
        emit switchPage(m_curPage);
    }
}

void PageListView::pageDown()
{
    qDebug() << "next page";
    if(m_curPage < m_pageNum - 1)
    {
        ++m_curPage;
        destroyPage();
        drawPageLayout();
        emit switchPage(m_curPage);
    }
}

void PageListView::goHome()
{
    if(m_curPage == 0)
        return;
    else
    {
        m_curPage = 0;
        destroyPage();
        drawPageLayout();
        emit switchPage(m_curPage);
    }
}

void PageListView::goEnd()
{
    if(m_curPage == m_pageNum - 1)
        return;
    else
    {
        m_curPage = m_pageNum - 1;
        destroyPage();
        drawPageLayout();
        emit switchPage(m_curPage);
    }
}

void PageListView::preItem()
{
    if(m_curItem > 0)
    {
        --m_curItem;
        m_itemList[m_curItem]->setFocus();
    }
    else if(m_curItem == 0)
    {
        pageUp();

    }
}

void PageListView::nextItem()
{
    if(m_curItem < m_itemNum - 1)
    {
        ++m_curItem;
        m_itemList[m_curItem]->setFocus();
    }
    else if(m_curItem == MAX_NUM_PP - 1)
    {
        pageDown();
    }
}

void PageListView::onEntryLogin(const QString &name)
{
    auto iter = std::find_if(m_itemList.begin(), m_itemList.end(), [name](UserEntry *entry){return entry->userName() == name;});
    m_curItem = std::distance(m_itemList.begin(), iter);
    emit loggedIn(name);
//    qDebug() << "login: " << name;
}

bool PageListView::hasPrev()
{
    if(m_curPage > 0)
        return true;
    else
        return false;
}

bool PageListView::hasNext()
{
    if(m_curPage < m_pageNum - 1)
        return true;
    else
        return false;
}

int PageListView::pageNum()
{
    return m_pageNum;
}

int PageListView::curPage()
{
    return m_curPage;
}
