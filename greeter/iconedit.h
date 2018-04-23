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
    const QString& innerTip(){ return m_tip; }
    void setInnerTip(const QString &tip){ m_tip = tip;}

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

private:
    void drawTip();


signals:
    void capsStateChanged(bool capsState);

private:
    QString m_tip;
};

class KeyEventMonitor;
class IconEdit : public QWidget
{
    Q_OBJECT
public:
    IconEdit(QWidget *parent = 0);


    void setIcon(const QString &filename);
    void setIcon(const QIcon &icon);
    void resize(const QSize& size);
    void clear();
    void setPrompt(const QString &);
    const QString text();
    void setType(QLineEdit::EchoMode type = QLineEdit::Password);
    void setText(const QString& text);
    void setWaiting(bool);

protected:
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

signals:
    void clicked(const QString &);
    void focusOut();

public slots:
    void clicked_cb();
    void showIconButton(const QString &);
    void onCapsStateChanged(int);
private:
    TipEdit         *m_edit;
    QLabel          *m_capsIcon;
    QPushButton     *m_iconButton;
    KeyEventMonitor *keyMonitor;
};

#endif // ICONEDIT_H
