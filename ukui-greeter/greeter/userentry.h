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

#define SHADOW_WIDTH (scale > 0.5 ? 10 : 5)
#define FACE_WIDTH (152 * scale)
#define BORDER_WIDTH FACE_WIDTH + SHADOW_WIDTH * 2
#define IMG_WIDTH (150 * scale)
#define ENTRY_HEIGHT (FACE_WIDTH + SHADOW_WIDTH * 2 + 45)
#define ENTRY_WIDTH (152 * scale + SHADOW_WIDTH * 2)

#define CENTER_IMG_WIDTH (180 * scale)
#define CENTER_FACE_WIDTH (182 * scale)
#define CENTER_ENTRY_WIDTH (182 * scale + SHADOW_WIDTH * 2)
#define CENTER_ENTRY_HEIGHT (CENTER_FACE_WIDTH + SHADOW_WIDTH * 2 + 45)
#define CENTER_BORDER_WIDTH CENTER_FACE_WIDTH + SHADOW_WIDTH * 2


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
    QPixmap DrawRound(QPixmap &src, int radius);
    QPixmap PixmapToRound(const QPixmap &src, int radius);

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *);

private:
    void onClicked();
    void initUI();

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

