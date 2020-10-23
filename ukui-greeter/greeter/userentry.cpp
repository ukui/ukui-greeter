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
#include <QPainterPath>
#include <QBrush>
#include <QFile>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QLightDM/UsersModel>
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
    m_loginLabel->setPixmap(QPixmap(":/images/is_logined.png"));
    m_loginLabel->resize(24,24);

}

void UserEntry::resizeEvent(QResizeEvent *)
{
    setResize();
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
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserEntry::onClicked()
{
    this->setSelected();
    emit clicked(index.row());
}

QPixmap UserEntry::PixmapToRound(const QPixmap &src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    QPixmap pixmapa(src);
    QPixmap pixmap(radius*2,radius*2);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, radius*2, radius*2);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, radius*2, radius*2, pixmapa);
    return pixmap;
}

void UserEntry::setFace(const QString &facePath)
{
    this->m_face = facePath;
    QFile faceFile(facePath);
    if(!faceFile.exists())
        this->m_face = ":/images/default_face.png";

    if(id == selectedId){
        const QString SheetStyle = QString("border-radius: %1px;  border:0px   solid white;").arg(CENTER_IMG_WIDTH/2);
        m_faceLabel->setStyleSheet(SheetStyle);
        userface = scaledPixmap(CENTER_IMG_WIDTH, CENTER_IMG_WIDTH, m_face);
        userface =  PixmapToRound(userface, CENTER_IMG_WIDTH/2);
        m_faceLabel->setPixmap(PixmapToOpacity(userface,1));
    }
    else{
        const QString SheetStyle = QString("border-radius: %1px;  border:0px   solid white;").arg(IMG_WIDTH/2);
        m_faceLabel->setStyleSheet(SheetStyle);
        userface = scaledPixmap(IMG_WIDTH, IMG_WIDTH, m_face);
        userface =  PixmapToRound(userface, IMG_WIDTH/2);
        m_faceLabel->setPixmap(PixmapToOpacity(userface,0.8));
    }

    m_faceLabel->setAlignment(Qt::AlignCenter);


}

const QString& UserEntry::userName()
{
    return this->m_name;
}

void UserEntry::setUserName(const QString &name)
{
    if(m_name != name)
        m_name = name;

     QString str = ElideText(m_nameLabel->font(),IMG_WIDTH,name);
     if(name != str)
         m_nameLabel->setToolTip(name);
     m_nameLabel->setText(str);
     m_nameLabel->setAlignment(Qt::AlignCenter);
}

void UserEntry::setLogin(bool isLogin)
{
    if(m_login != isLogin)
        m_login = isLogin;
    if(isLogin)
        m_loginLabel->show();
    else
        m_loginLabel->hide();
}

void UserEntry::setEnterEvent(bool isEnter)
{
    if(id == selectedId)
        return;

//    QRect faceRect,nameRect,loginRect;

//    const QString SheetStyle = QString("border-radius: %1px;  border:0px   solid white;").arg(width()/2);
//    m_faceLabel->setStyleSheet(SheetStyle);
//    faceRect.setRect(0, 0, width(), width());
//    userface = scaledPixmap(width(), width(), m_face);
//    userface =  PixmapToRound(userface, width()/2);
//    m_faceLabel->setGeom/*etry(faceRect);
//    m_faceLabel->move((width() - m_faceLabel->width())*//2,m_faceLabel->y());

//    m_faceLabel->setPixmap(userface);

//    m_loginLabel->setGeometry(m_faceLabel->x() + m_faceLabel->width() - 24,m_faceLabel->y(),24,24);

//    if(isEnter)
//    {
//        QFont font = m_nameLabel->font();
//        font.setPixelSize(16);
//        m_nameLabel->setFont(font);
//        m_nameLabel->adjustSize();
//	//距离头像保持25距离
//        m_nameLabel->move((width() - m_nameLabel->width())/2,m_faceLabel->y() + m_faceLabel->height() + 25);
//    }
//    else
//    {
//        QFont font = m_nameLabel->font();
//        font.setPixelSize(14);
//        m_nameLabel->setFont(font);
//        m_nameLabel->adjustSize();
//        m_nameLabel->move((width() - m_nameLabel->width())/2,m_faceLabel->y() + m_faceLabel->height() + 32);
//    }

      userface = scaledPixmap(width(), width(), m_face);
      userface =  PixmapToRound(userface, width()/2);
      if(isEnter)
          m_faceLabel->setPixmap(PixmapToOpacity(userface,1));
      else
          m_faceLabel->setPixmap(PixmapToOpacity(userface,0.8));
}

//返回一张带有透明度的图片,0<=val<=1
QPixmap UserEntry::PixmapToOpacity(const QPixmap src, double val)
{
    QPixmap temp(src.size());
    temp.fill(Qt::transparent);
    QPainter p(&temp);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(0, 0, src);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(temp.rect(), QColor(0, 0, 0, val*255));
    return temp;
}

void UserEntry::setResize()
{
    QRect faceRect,nameRect,loginRect;

    const QString SheetStyle = QString("border-radius: %1px;  border:0px   solid white;").arg(width()/2);
    m_faceLabel->setStyleSheet(SheetStyle);
    faceRect.setRect(0, 0, width(), width());
    userface = scaledPixmap(width(), width(), m_face);
    userface =  PixmapToRound(userface, width()/2);
    m_faceLabel->setGeometry(faceRect);
    m_faceLabel->move((width() - m_faceLabel->width())/2,m_faceLabel->y());

    m_loginLabel->setGeometry(m_faceLabel->x() + m_faceLabel->width() - 24,m_faceLabel->y(),24,24);

    if(id == selectedId)
    {
        QFont font = m_nameLabel->font();
        font.setPixelSize(16);
        m_nameLabel->setFont(font);
        setUserName(m_name);
        m_nameLabel->adjustSize();
	//距离头像保持25距离
        m_nameLabel->move((width() - m_nameLabel->width())/2,m_faceLabel->y() + m_faceLabel->height() + 25);
        m_faceLabel->setPixmap(PixmapToOpacity(userface,1));

    }
    else
    {
        QFont font = m_nameLabel->font();
        font.setPixelSize(14);
        m_nameLabel->setFont(font);
	//当前头像，用户名距离头像保持32距离
        setUserName(m_name);
        m_nameLabel->adjustSize();
        m_nameLabel->move((width() - m_nameLabel->width())/2,m_faceLabel->y() + m_faceLabel->height() + 32);
        m_faceLabel->setPixmap(PixmapToOpacity(userface,0.8));

    }
}

void UserEntry::setSelected(bool selected)
{
    if(selected) {
        selectedId = this->id;
    }

    setResize();
}

bool UserEntry::selected()
{
    return id == selectedId;
}

void UserEntry::setUserIndex(const QPersistentModelIndex &index)
{
    Q_ASSERT(index.isValid());
    this->index = index;

    QString realName = index.data(QLightDM::UsersModel::RealNameRole).toString();
    QString name = index.data(QLightDM::UsersModel::NameRole).toString();
    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    bool isLoggedIn = index.data(QLightDM::UsersModel::LoggedInRole).toBool();

    setUserName(realName.isEmpty() ? name : realName);
    setFace(facePath);
    setLogin(isLoggedIn);
}

QPersistentModelIndex UserEntry::userIndex()
{
    return index;
}
