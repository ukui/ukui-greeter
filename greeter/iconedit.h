/* iconedit.h
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

#ifndef ICONEDIT_H
#define ICONEDIT_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>

class TipEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString innerTip READ innerTip WRITE setInnerTip)
public:
    TipEdit(QWidget *parent=0);
    void paintEvent(QPaintEvent *);
    void keyReleaseEvent(QKeyEvent *);

    void drawTip();
    const QString& innerTip(){ return m_tip; }
    void setInnerTip(const QString &tip){ m_tip = tip;}

signals:
    void CapsStateChanged(bool capsState);

private:
    QString m_tip;
    bool m_capsState;
};

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    IconButton(TipEdit *edit, QWidget *parent=0);
    IconButton(TipEdit *edit, const QIcon &icon, QWidget *parent=0);
    void resize(const QSize &size);

public slots:
    void onCapsStateChanged(bool);

private:
    QSize   m_size;
    QLabel  *m_capTips;
};

class IconEdit : public QWidget
{
    Q_OBJECT
public:
    IconEdit(QWidget *parent = 0);
    IconEdit(const QIcon& icon, QWidget *parent = 0);
    virtual void keyReleaseEvent ( QKeyEvent * event );

    void setIcon(const QString &filename);
    void setIcon(const QIcon &icon);
    void resize(const QSize& size);
    void clear();
    void setPrompt(const QString &);
    const QString text();
    void setType(QLineEdit::EchoMode type = QLineEdit::Password);
    void setText(const QString& text);
    void setWaiting(bool);

signals:
    void clicked(const QString &);
    void focusOut();

public slots:
    void clicked_cb();
    void showIcon(const QString&);

private:
    TipEdit      *m_edit;
    IconButton   *m_iconButton;
};

#endif // ICONEDIT_H
