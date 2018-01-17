#ifndef PAGELISTVIEW_H
#define PAGELISTVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QVector>
#include "userentry.h"
#include "usersmodel.h"

class PageListView : public QWidget
{
    Q_OBJECT
public:
    explicit PageListView(QWidget *parent = 0);
    void setModel(QSharedPointer<QAbstractItemModel>);
    void pageUp();      //切换到上一页    留给父窗口通过按钮调用
    void pageDown();    //切换到下一页
    bool hasPrev();     //是否有上一页
    bool hasNext();     //是否有下一页
    int pageNum();      //页数
    int curPage();      //当前页 0起

protected:
    void keyReleaseEvent(QKeyEvent *);
    bool eventFilter(QObject *, QEvent *);

private:
    void drawPage();
    void drawPageLayout();
    void destroyPage();
    void goHome();      //切换到首页
    void goEnd();       //切换到尾页
    void preItem();     //切换到上一项
    void nextItem();    //切换到下一项
    void switchToUser();    //切换到选中的用户登录界面

signals:
    void switchPage(int cur_page);  //当翻页的时候发出
    void loggedIn(const QModelIndex& index);    //当选定用户时发出

private slots:
    void onEntryLogin(const QString &name);
//    void onUserChanged();
//    void onUserInserted();
//    void onUserRemoved();
public:
    const int MAX_NUM_PP = 5;   //每页最大的数量
private:
    QSharedPointer<QAbstractItemModel> m_model;
    QVector<UserEntry*> m_itemList;
    QHBoxLayout *m_layout;

    int m_itemNum;      //每页的项数量
    int m_pageNum;      //总页数
    int m_curPage;      //当前页数
    int m_curItem;      //当前焦点所在项
    int m_end;
    int m_lastend;
    int m_itemCount;
};

#endif // PAGELISTVIEW_H
