/* userwindow.h
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include <QLightDM/UsersModel>

class QPushButton;
class PageListView;
class UserWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UserWindow(QWidget *parent = 0);

    void initUI();
    void setModel(QSharedPointer<QAbstractItemModel> model);

protected:
    bool eventFilter(QObject *, QEvent *);
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);

private slots:
    void onPageChanged();
    void onSelectedChanged(const QModelIndex &);

signals:
    void selectedChanged(const QModelIndex &index);

private:
//    QLightDM::UsersModel *m_model;
//    QVBoxLayout *m_vlayout;
//    QHBoxLayout *m_hlayout;
    QPushButton      *m_prevLabel;
    QPushButton      *m_nextLabel;
    PageListView     *m_userList;
};

#endif // USERWINDOW_H
