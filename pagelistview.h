#ifndef PAGELISTVIEW_H
#define PAGELISTVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include "userentry.h"

class PageListView : public QWidget
{
    Q_OBJECT
public:
    explicit PageListView(QWidget *parent = 0);
    void setModel(QAbstractItemModel*);

protected:
    void keyReleaseEvent(QKeyEvent *);
    bool eventFilter(QObject *, QEvent *);

private:
    void drawPageIndicator();
    void drawPage();
    void drawPageLayout();
    void destroyPage();
    void pageUp();      //切换到上一页
    void pageDown();    //切换到下一页
    void goHome();      //切换到首页
    void goEnd();       //切换到尾页
    void preItem();     //切换到上一项
    void nextItem();    //切换到下一项
    void switchToUser();    //切换到选中的用户登录界面

signals:

public slots:
    void onEntryLogin(const QString &name);

public:
    const int MAX_NUM_PP = 5;   //每页最大的数量
private:
    QAbstractItemModel *m_model;
    QList<UserEntry*>  m_itemList;
    QLabel  *m_preLabel;
    QLabel  *m_nextLabel;
    QLabel  *m_pageIdcLabel;

    QHBoxLayout *m_layout;

    int m_itemNum;      //每页的项数量
    int m_pageNum;      //总页数
    int m_curPage;      //当前页数
    int m_curItem;      //当前焦点所在项
};

#endif // PAGELISTVIEW_H
