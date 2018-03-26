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
#include <QMouseEvent>
#include <QDebug>
#include "globalv.h"

int UserEntry::count = 0;
int UserEntry::selectedId = 0;

UserEntry::UserEntry(const QString &name, const QString &facePath, bool isLogin, QWidget *parent)
    : QWidget(parent),
      id(count++)
{
    initUI();

    setUserName(name);
    setFace(facePath);
    setLogin(isLogin);
}

UserEntry::UserEntry(QWidget *parent)
    : UserEntry("kylin", "", true, parent)
{
    setUserName("Kylin"+QString::number(id));
}

void UserEntry::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Entry")+QString::number(id));
    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QString::fromUtf8("faceLabel"));
    m_faceLabel->setScaledContents(true);
    m_faceLabel->installEventFilter(this);

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
    if(id == selectedId)
    {
        QRect rect = m_faceLabel->geometry();
        QRect border(rect.left() - SHADOW_WIDTH, rect.top() - SHADOW_WIDTH,
                     rect.width() + SHADOW_WIDTH*2, rect.height() + SHADOW_WIDTH*2);

        QPainter painter(this);
        painter.setPen(QPen(QColor(255, 255, 255, 0), 1));
        painter.setBrush(QColor(0, 0, 0, 50));
        painter.drawRect(border);
    }
    return QWidget::paintEvent(event);
}

void UserEntry::resizeEvent(QResizeEvent *)
{
    QRect faceRect(SHADOW_WIDTH, SHADOW_WIDTH, FACE_WIDDTH, FACE_WIDDTH);
    m_faceLabel->setGeometry(faceRect);
    QPixmap pixmap(this->m_face);
    pixmap = pixmap.scaled(IMG_WIDTH, IMG_WIDTH, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    this->m_faceLabel->setPixmap(pixmap);

    QRect nameRect(SHADOW_WIDTH, faceRect.bottom() + SHADOW_WIDTH, FACE_WIDDTH, 20);
    m_nameLabel->setGeometry(nameRect);
    m_nameLabel->setFont(QFont("Ubuntu", fontSize));

    QRect loginRect(SHADOW_WIDTH, nameRect.bottom()+5, FACE_WIDDTH, 20);
    m_loginLabel->setGeometry(loginRect);
    m_loginLabel->setFont(QFont("Ubuntu", fontSize));
}

bool UserEntry::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_faceLabel){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                Q_EMIT pressed();
                return true;
            }
        } else if(event->type() == QEvent::MouseButtonRelease){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                onClicked();
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
    QPixmap pixmap(m_face);
    pixmap = pixmap.scaled(IMG_WIDTH, IMG_WIDTH, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_faceLabel->setPixmap(pixmap);
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
        m_login = isLogin;
    this->m_loginLabel->setText(m_login ? tr("logged in") : "");
}

void UserEntry::setSelected(bool selected)
{
    if(selected) {
        selectedId = this->id;
    }
    update();
}

bool UserEntry::selected()
{
    return id == selectedId;
}
