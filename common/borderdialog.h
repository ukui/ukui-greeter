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
#ifndef BORDERDIALOG_H
#define BORDERDIALOG_H

#include <QWidget>

class QPushButton;
class BorderDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
public:
    explicit BorderDialog(QWidget *parent = 0);
    virtual void close();

    void setBorderWidth(int width = 24);
    void setBorderColor(QColor color = QColor(0, 0, 0, 50));
    void setDialogSize(int width, int h);
    void setDialogColor(QColor color = QColor("#2D698E"));

protected:
    void initUI();
    QWidget *center() const;

private:
    void startBackground(int begin, int end, bool show);
    int opacity();
    void setOpacity(int);
    void setWidgetColor(QWidget *widget, QColor color);


protected:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void aboutToClose();

private:
    qreal       m_opacity;

    QWidget     *m_centerWidget;
    QWidget     *m_borderWidget;
    QPushButton *m_close;

    int borderWidth;
};

#endif // BORDERDIALOG_H
