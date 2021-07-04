/* userentry.h
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
#ifndef USERENTRY_H
#define USERENTRY_H

#include <QWidget>
#include <QLabel>
#include <QPersistentModelIndex>

#define FACE_WIDTH (160 * scale)
#define BORDER_WIDTH (95 * scale)
#define C_BORDER_WIDTH (60 *scale)
#define C_BORDER_HEIGHT C_BORDER_WIDTH
#define IMG_WIDTH (160 * scale)
#define ENTRY_HEIGHT (FACE_WIDTH + 60)
#define ENTRY_WIDTH (160 * scale)

#define CENTER_IMG_WIDTH (160 * scale)
#define CENTER_FACE_WIDTH (160 * scale)
#define CENTER_ENTRY_WIDTH (160 * scale )
#define CENTER_ENTRY_HEIGHT (CENTER_FACE_WIDTH + 60)

extern float scale;

class UserEntry : public QWidget
{
    Q_OBJECT
public:
    UserEntry(const QString &name, const QString &facePath, bool isLogin, QWidget *parent);
    UserEntry(QWidget *parent=0);

    void setUserIndex(const QPersistentModelIndex &index);
    QPersistentModelIndex userIndex();
    void setFace(const QString &facePath);
    const QString& userName();
    void setUserName(const QString &name);
    void setLogin(bool isLogin);
    void setSelected(bool selected=true);
    bool selected();
    void setResize();
    void setEnterEvent(bool isEnter);
    void setMoveSize();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *);

private:
    void onClicked();
    void initUI();
    QPixmap PixmapToRound(const QPixmap &src, int radius);
    QPixmap PixmapToOpacity(const QPixmap src , double val);


signals:
    void clicked(int row);
    void pressed();

private:
    QLabel *m_faceLabel;
    QLabel *m_nameLabel;
    QLabel *m_loginLabel;
    QPixmap userface;
    QPersistentModelIndex index;

    QString m_face;
    QString m_name;
    bool m_login;
    int id;
    static int count;
    static int selectedId;

};



#endif // USERENTRY_H

