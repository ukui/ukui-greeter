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
#include "borderdialog.h"
#include <QPushButton>
#include <QPropertyAnimation>
#include <QPainter>
#include <QDebug>
#include <QPalette>
#include <QMouseEvent>

BorderDialog::BorderDialog(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

QWidget *BorderDialog::center() const
{
    return m_centerWidget;
}


void BorderDialog::initUI()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setGeometry(parentWidget()->rect());

    m_borderWidget = new QWidget(this);
    m_borderWidget->setObjectName("BorderDialogBorder");
    m_centerWidget = new QWidget(m_borderWidget);
    m_centerWidget->setObjectName("BorderDialogWidget");

    /* 窗口关闭按钮 */
    m_close = new QPushButton(m_borderWidget);
    m_close->setObjectName(QStringLiteral("close"));
    connect(m_close, &QPushButton::clicked, this, &BorderDialog::close);

    setBorderWidth();
    setBorderColor();
    setDialogColor();
}

void BorderDialog::setBorderWidth(int width)
{
    borderWidth = width;
    setDialogSize(this->width(), this->height());
}

void BorderDialog::setBorderColor(QColor color)
{
    setWidgetColor(m_borderWidget, color);
}

void BorderDialog::setDialogSize(int w, int h)
{
    m_borderWidget->setGeometry((width() - w) / 2,
                                (height() - h) / 2,
                                w, h);
    m_centerWidget->setGeometry(borderWidth, borderWidth,
                                m_borderWidget->width() - borderWidth * 2,
                                m_borderWidget->height() - borderWidth * 2);

    m_close->setGeometry(QRect(0, 0, borderWidth, borderWidth));
}

void BorderDialog::setDialogColor(QColor color)
{
    setWidgetColor(m_centerWidget, color);
}

void BorderDialog::setWidgetColor(QWidget *widget, QColor color)
{
    QString borderStyle = QString("#%1{background: rgb(%2, %3, %4, %5)}")
                        .arg(widget->objectName())
                        .arg(QString::number(color.red()))
                        .arg(QString::number(color.green()))
                        .arg(QString::number(color.blue()))
                        .arg(QString::number(color.alpha()));
    widget->setStyleSheet(borderStyle);
}

void BorderDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(QColor(0, 0, 0, m_opacity));
    painter.drawRect(rect());

    QWidget::paintEvent(e);
}

void BorderDialog::mousePressEvent(QMouseEvent *event)
{
    if(!m_borderWidget->geometry().contains(event->pos()))
        close();
}

void BorderDialog::close()
{
    m_borderWidget->close();
    startBackground(80, 0, false);
}

void BorderDialog::showEvent(QShowEvent *)
{
    startBackground(0, 80, true);
}

void BorderDialog::startBackground(int begin, int end, bool show)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(300);
    animation->setStartValue(begin);
    animation->setEndValue(end);
    if(!show)
        connect(animation, &QPropertyAnimation::finished, this, &QWidget::close);

    animation->start();
}

void BorderDialog::setOpacity(int opacity)
{
    m_opacity = opacity;
    update();
}

int BorderDialog::opacity()
{
    return m_opacity;
}
