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
    Q_PROPERTY(int page NOTIFY pageChanged)
public:
    explicit PageListView(QWidget *parent = 0);
    void setModel(QSharedPointer<QAbstractItemModel>);
    void pageUp();      //切换到上一页    留给父窗口通过按钮调用
    void pageDown();    //切换到下一页
    bool hasPrev();
    bool hasNext();

protected:
    void keyReleaseEvent(QKeyEvent *);
    void showEvent(QShowEvent *);

private:
    UserEntry* createEntry(int );
    void drawPage();
    void goHome();      //切换到首页
    void goEnd();       //切换到尾页
    void preItem();     //切换到上一项
    void nextItem();    //切换到下一项
    void switchToUser();    //切换到选中的用户登录界面

signals:
    void pageChanged();  //当焦点所在项发生变化时发出
    void selectedChanged(const QModelIndex& index);    //当选定用户时发出

private slots:
    void onEntrySelected(const QString &name);
    void onUserChanged(const QModelIndex&, const QModelIndex&);
    void onUserInserted(const QModelIndex&, int, int);
    void onUserRemoved(const QModelIndex&, int, int);
public:
    const int MAX_NUM_PP = 5;   //每页最大的数量
private:
    QSharedPointer<QAbstractItemModel> m_model;
    QVector<UserEntry*> m_itemList;
    QHBoxLayout *m_layout;

    int m_lastItem;     //上一次选择的项
    int m_curItem;      //当前焦点所在项
    int m_end;          //当前页的最后一项
    int m_lastend;      //上一次的最后一项
    int m_itemCount;
};

#endif // PAGELISTVIEW_H
