/* userentry.cpp
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
#include "userentry.h"
#include <QPainter>
#include <QBrush>
#include <QFile>
#include <QFontMetrics>
#include <QApplication>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QDebug>
#include "globalv.h"


UserEntry::UserEntry(const QString &name, const QString &facePath, bool isLogin, QWidget *parent)
    : QWidget(parent),
      m_selected(false)
{
    initUI();

    m_faceLabel->installEventFilter(this);

    setFace(facePath);
    setUserName(name);
    setLogin(isLogin);
}

UserEntry::UserEntry(QWidget *parent)
    : UserEntry("", "", false, parent)
{
}

void UserEntry::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Entry"));   
//    setFixedSize(130 * scale + 20, 130 * scale + 75);//不能用resize，否则放到layout中时，如果layout加有stretch，则该widget显示不出来
    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QString::fromUtf8("faceLabel"));
    m_faceLabel->setScaledContents(true);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
    m_nameLabel->setFont(QFont("Ubuntu", fontSize));

    m_loginLabel = new QLabel(this);
    m_loginLabel->setObjectName(QString::fromUtf8("isloginLabel"));
    m_loginLabel->setAlignment(Qt::AlignCenter);
    m_loginLabel->setFont(QFont("Ubuntu", fontSize));
}

void UserEntry::paintEvent(QPaintEvent *event)
{
    //绘制阴影边框
    if(this->selected())
    {
        int shadowWidth = scale > 0.5 ? 10 : 5;
        QRect rect = m_faceLabel->geometry();
        QRect border(rect.left()-shadowWidth, rect.top()-shadowWidth,
                     rect.width()+shadowWidth*2, rect.height()+shadowWidth*2);

        QPainter painter(this);
        painter.setPen(QPen(QColor(255, 255, 255, 0), 1));
        painter.setBrush(QColor(0, 0, 0, 50));
        painter.drawRect(border);
    }
    return QWidget::paintEvent(event);
}

void UserEntry::resizeEvent(QResizeEvent *)
{
    int shadowWidth = scale > 0.5 ? 10 : 5;
    QRect faceRect(shadowWidth, shadowWidth, 130*scale, 130*scale);
    m_faceLabel->setGeometry(faceRect);
    this->m_faceLabel->setPixmap(scaledPixmap(128*scale, 128*scale, m_face));

    QRect nameRect(shadowWidth, faceRect.bottom()+shadowWidth, 130*scale, 20);
    m_nameLabel->setGeometry(nameRect);
    m_nameLabel->setFont(QFont("Ubuntu", fontSize));

    QRect loginRect(shadowWidth, nameRect.bottom()+5, 130*scale, 20);
    m_loginLabel->setGeometry(loginRect);
    m_loginLabel->setFont(QFont("Ubuntu", fontSize));
}

bool UserEntry::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_faceLabel){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                this->setSelected();
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                emit clicked(m_name);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserEntry::onClicked()
{
    this->setSelected();
    emit clicked(m_name);
}

void UserEntry::setFace(const QString &facePath)
{
    this->m_face = facePath;
    QFile faceFile(facePath);
    if(!faceFile.exists())
        this->m_face = ":/resource/default_face.png";
    m_faceLabel->setPixmap(scaledPixmap(128*scale, 128*scale, m_face));
}

const QString& UserEntry::userName()
{
    return this->m_name;
}

void UserEntry::setUserName(const QString &name)
{
    if(m_name != name)
        m_name = name;
    this->m_nameLabel->setText(m_name);
    /* 当用户名短的时候居中， 否则是居左显示前半部分 */
    QFont font("ubuntu", fontSize);
    QFontMetrics fm(font);
    int pixelsWide = fm.width(m_name);
    if(pixelsWide < m_nameLabel->width())
        m_nameLabel->setAlignment(Qt::AlignCenter);
}

void UserEntry::setLogin(bool isLogin)
{
    if(m_login != isLogin)
    {
        m_login = isLogin;
        this->m_loginLabel->setText(m_login ? tr("logged in") : "");
    }
}

void UserEntry::setSelected(bool selected)
{
    this->m_selected = selected;
    update();
}

bool UserEntry::selected()
{
    return this->m_selected;
}
