/**
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
#ifndef FAKEDIALOG_H
#define FAKEDIALOG_H

#include <QWidget>

class QPushButton;

class FakeDialog : public QWidget
{
    Q_OBJECT
public:
    explicit FakeDialog(QWidget *parent = nullptr);
    void setDialogSize(int w, int h);
    QWidget *centerWidget();
    QWidget *dialog();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void initUI();

private:
    int dialogWidth;
    int dialogHeight;

    QWidget         *m_dialog;
    QWidget         *m_centerWidget;
    QPushButton     *m_closeButton;
//    QPushButton     *m_okButton;
//    QPushButton     *m_cancelButton;
};

#endif // FAKEDIALOG_H
