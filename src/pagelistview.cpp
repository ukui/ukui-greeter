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
#include "globalv.h"

PageListView::PageListView(QWidget *parent)
    : QWidget(parent),
      m_layout(new QHBoxLayout(this))
{
    resize(920*scale, 300*scale);
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

void PageListView::setModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull())
        return;
    if(!m_model.isNull())
    {

    }
    m_model = model;
    m_itemList = QVector<UserEntry*>(m_model.data()->rowCount(), NULL);

//    m_pageNum = qCeil(m_model.data()->rowCount() * 1.0 / MAX_NUM_PP);
//    m_curPage = 0;

    m_lastend = -1;
    m_itemCount = m_model.data()->rowCount();
    m_end = m_itemCount >= 5 ? 4 : m_itemCount - 1; //每页最多显示5个
    m_curItem = 0;
    drawPage();

//    drawPageLayout();
}

void PageListView::drawPage()
{
    int begin = m_end - (m_itemCount > 5 ? 4 : m_itemCount - 1);

    //从layout中移除之前的entry
    if(m_lastend > 0)
    {
        int lastbegin = m_lastend - (m_itemCount > 5 ? 4 : m_itemCount - 1);
        for(int i = lastbegin; i <= m_lastend; i++)
        {
            m_layout->removeWidget(m_itemList[i]);
            m_itemList[i]->hide();
        }
    }
//    if(m_itemCount < 5)
        m_layout->addStretch();
    //如果entry不存在则创建，添加到layout中
    for(int i = begin; i <= m_end; i++)
    {
        if(m_itemList[i] == NULL)
        {
            QModelIndex index = m_model.data()->index(i, 0);
            UserEntry *entry = new UserEntry(index.data(Qt::DisplayRole).toString(),
                                             index.data(QLightDM::UsersModel::ImagePathRole).toString(),
                                             index.data(QLightDM::UsersModel::LoggedInRole).toBool(), this);
            connect(entry, SIGNAL(clicked(QString)), this, SLOT(onEntryLogin(QString)));
            m_itemList[i] = entry;
        }
        m_itemList[i]->show();
        m_layout->addWidget(m_itemList[i], 0, Qt::AlignTop);
    }
//    if(m_itemCount < 5)
        m_layout->addStretch();
    m_itemList[m_curItem]->setFocus();
    emit pageChanged();
}

/*
void PageListView::drawPageLayout()
{
    if(m_layout == NULL)
    {
        m_layout = new QHBoxLayout(this);
        m_layout->setSpacing(0);
    }
    m_itemNum = m_model.data()->rowCount() - m_curPage * MAX_NUM_PP > 5 ? 5 : m_model.data()->rowCount() - m_curPage * MAX_NUM_PP;
    qDebug()<< "第" << m_curPage<<  "页， 共" <<  m_itemNum << "项";


    m_layout->addStretch();

    for(int i = 0; i< m_itemNum; i++)
    {
        UserEntry *entry = new UserEntry(this);
        entry->setObjectName("Entry"+QString::number(i));
        connect(entry, SIGNAL(clicked(QString)), this, SLOT(onEntryLogin(QString)));

        QModelIndex index = m_model.data()->index(m_curPage * MAX_NUM_PP + i, 0);
        QString face = index.data(QLightDM::UsersModel::ImagePathRole).toString();
        QString name = index.data(Qt::DisplayRole).toString();
        bool islogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
        qDebug() << face << " "<< name << " " << islogin;
        entry->setFace(face);
        entry->setUserName(name);
        entry->setLogin(islogin);

        m_itemList[i] = entry;
        m_layout->addWidget(entry);

    }

    m_layout->addStretch();


    this->setLayout(m_layout);
    m_itemList[0]->setFocus();
    m_curItem = 0;
}
*/
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
    if(m_itemCount <= 5 || m_end == 4)  //位于首页
    {
        m_curItem = 0;
        m_itemList[m_curItem]->setFocus();
        return;
    }
    m_lastend = m_end;
    m_end -= (m_end-5 >= 4 ? 5 : (m_end-4));
    m_curItem = m_end - 4;
    drawPage();
}

void PageListView::pageDown()
{
    if(m_itemCount <= 5 || m_end == m_itemCount - 1)    //位于尾页
    {
        m_curItem = m_end;
        m_itemList[m_curItem]->setFocus();
        return;
    }
    m_lastend = m_end;
    m_end += (m_itemCount-1-m_end >= 5 ? 5 : (m_itemCount-1-m_end));
    m_curItem = m_end;
    drawPage();
}

void PageListView::goHome()
{
    if(m_itemCount <= 5 || m_end == 4)
    {
        m_curItem = 0;
        m_itemList[m_curItem]->setFocus();
        return;
    }
    m_lastend = m_end;
    m_end = 4;
    m_curItem = 0;
    drawPage();
}

void PageListView::goEnd()
{
    if(m_itemCount <= 5 || m_end == m_itemCount - 1)    //位于尾页
    {
        m_curItem = m_end;
        m_itemList[m_curItem]->setFocus();
        return;
    }
    m_lastend = m_end;
    m_end = m_itemCount - 1;
    m_curItem = m_end;
    drawPage();
}

void PageListView::preItem()
{
    int begin = m_itemCount > 5 ? m_end - 4 : 0;
    if(m_curItem - begin > 0)
    {
        --m_curItem;
        m_itemList[m_curItem]->setFocus();
    }
    else if(m_curItem > 0)
    {
        --m_curItem;
        m_lastend = m_end;
        --m_end;
        drawPage();
    }
}

void PageListView::nextItem()
{
    if(m_curItem < m_end)
    {
        ++m_curItem;
        m_itemList[m_curItem]->setFocus();
    }
    else if(m_curItem < m_itemCount - 1)
    {
        ++m_curItem;
        m_lastend = m_end;
        ++m_end;
        drawPage();
    }
}

void PageListView::onEntryLogin(const QString &name)
{
    for(int i = 0; i < m_itemList.size(); i++){
        if(m_itemList[i]!=NULL && m_itemList[i]->userName() == name){
            m_curItem = i;
        }
    }
//    auto iter = std::find_if(m_itemList.begin(), m_itemList.end(), [name](UserEntry *entry){return entry->userName() == name;});
//    m_curItem = std::distance(m_itemList.begin(), iter);
    emit loggedIn(m_model.data()->index(m_curItem, 0));
}

bool PageListView::hasPrev()
{
    if(m_end > 4)
        return true;
    else
        return false;
}

bool PageListView::hasNext()
{
    if(m_end < m_itemCount -1)
        return true;
    else
        return false;
}

//int PageListView::pageNum()
//{
//    return m_pageNum;
//}

//int PageListView::curPage()
//{
//    return m_curPage;
//}
