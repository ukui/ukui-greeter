#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QSharedPointer>
#include <QLightDM/UsersModel>
#include "pagelistview.h"

class UserWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UserWindow(QWidget *parent = 0);

    void initUI();
    void setModel(QSharedPointer<QAbstractItemModel> model);

protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void onPageChanged();
    void onSelectedChanged(const QModelIndex &);

signals:
    void selectedChanged(const QModelIndex &index);

private:
//    QLightDM::UsersModel *m_model;
//    QVBoxLayout *m_vlayout;
//    QHBoxLayout *m_hlayout;
    QLabel      *m_pageIndicator;
    QLabel      *m_prevLabel;
    QLabel      *m_nextLabel;
    PageListView *m_userList;
};

#endif // USERWINDOW_H
