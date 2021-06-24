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
#include <QSettings>
#include <QPixmap>

class Configuration;
class IconEdit : public QWidget
{
    Q_OBJECT
public:
    IconEdit(QWidget *parent = 0);

    void setIcon(const QString &text);
    void setIcon(const QIcon &icon);
    void clear();
    void setPrompt(const QString &);
    const QString text();
    void setType(QLineEdit::EchoMode type = QLineEdit::Password);
    void startWaiting();
    void stopWaiting();
    void setCapsState(bool capsState);

protected:
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void updatePixmap();
    void refreshTranslate();

signals:
    void clicked(const QString &);
    void focusOut();

private slots:
    void clicked_cb();
    void onGlobalKeyRelease(const QString &key);

private:
    QLineEdit       *m_edit;
    QLabel          *m_capsIcon;
    QPushButton     *m_iconButton;
    QPushButton     *m_modeButton;
    QTimer          *m_timer;
    QPixmap         m_waitingPixmap;
    QString         m_iconText;     //文字作为图标
    QIcon           m_icon;
    Configuration   *m_configuration;
};

#endif // ICONEDIT_H
