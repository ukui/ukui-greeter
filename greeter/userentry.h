/* userentry.h
 * Copyright (C) 2018 yanghao <yanghao@kylinos.cn>
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
#ifndef USERENTRY_H
#define USERENTRY_H

#include <QWidget>
#include <QLabel>


class UserEntry : public QWidget
{
    Q_OBJECT
public:
    UserEntry(const QString &facePath, const QString &name, bool isLogin, QWidget *parent);
    UserEntry(QWidget *parent=0);

    void setFace(const QString &facePath);
    const QString& userName();
    void setUserName(const QString &name);
    void setLogin(bool isLogin);
    void setSelected(bool selected=true);
    bool selected();
    static int X(){return 190;}
    static int Y(){return 240;}

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);


private:
    void onClicked();
    void initUI();

signals:
    void clicked(const QString&);

private:
    QLabel *m_faceLabel;
    QLabel *m_nameLabel;
    QLabel *m_loginLabel;

    QString m_face;
    QString m_name;
    bool m_login;
    bool m_selected;
};





#endif // USERENTRY_H

