/* sessionwindow.h
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
#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include <QAbstractItemModel>

class QLabel;
class QPushButton;
class IconLabel;
class QListWidget;
class QListWidgetItem;
class SessionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SessionWindow(const QString &session, QWidget *parent = 0);
    void setSessionModel(QAbstractItemModel *model);
    void setSession(const QString&session);

protected:
    void keyReleaseEvent(QKeyEvent *);
    void showEvent(QShowEvent *event);

signals:
    void back();
    void sessionSelected(const QString& sessionName);

private slots:
    void saveAndBack();

private:
    void initUI();
    QString getSessionIcon(const QString&);
    void addSessionLabels();
    void testAddSessionLabels();

    QString              m_defaultSession;
    QAbstractItemModel  *m_sessionsModel;

    //UI
    QPushButton         *m_backLabel;
    QLabel              *m_prompt;
    QListWidget         *m_sessionsList;
};

#endif // SESSIONWINDOW_H
