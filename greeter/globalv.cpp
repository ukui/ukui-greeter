/* globalv.cpp
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
#include "globalv.h"

#include <QDebug>
#include <QRect>
#include <QPixmap>
#include <QFontMetrics>
#include <QPainter>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

bool capsLock;

/**
 * @brief scaledPixmap
 * @param width
 * @param height
 * @param url
 * @return
 * 图片缩放
 */
QPixmap scaledPixmap(int width, int height, QString url)
{
    QFile imgFile(url);
    if(!imgFile.exists()){
        qDebug()<< "not find the pixmap file";
        return QPixmap();
    }
    QPixmap pixmap(url);
    return pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}

/**
 * @brief isCapsLock
 * @return
 * 获取大写键打开状态
 */
bool getCapsLock()
{
    //判断大写键状态
    Display *display = XOpenDisplay(NULL);
//    bool capsState = false;
    capsLock = false;
    if(display) {
        unsigned int n;
        XkbGetIndicatorState(display, XkbUseCoreKbd, &n);
        capsLock = (n & 0x01) == 1;
    }
    return capsLock;
}

/**
 * @brief getSystemVersion
 * @return
 * 获取系统版本号
 */
QString getSystemVersion()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString release = settings.value("DISTRIB_RELEASE").toString();
    QString description = settings.value("DISTRIB_DESCRIPTION").toString();
    if(description.right(3) == "LTS")
        release = release + " LTS";
    return release;
}

/**
 * @brief logoGenerator
 * @param text
 * @return
 * 生成logo
 */
QPixmap logoGenerator(const QString &text)
{
    QString logoFile(IMAGE_DIR + "logo.png");
    if(QFile(logoFile).exists())
    {
        QPixmap logo(IMAGE_DIR + "logo.png");
        return logo;
    }
    QPixmap logoBare;
    logoBare.load(":/resource/logo-bare.png");

    QFont font("ubuntu", 18);
    QFontMetrics fm(font);
    int textPixelSize = fm.width(text);

    QPixmap logo(logoBare.width() + textPixelSize + 3, logoBare.height());
    logo.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&logo);
    painter.drawPixmap(logoBare.rect(), logoBare, logoBare.rect());

    painter.setPen(Qt::white);
    painter.setFont(font);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);
    QRect versionRect(logoBare.rect().right()+3, 3, logo.width() - logoBare.width(), logo.height());
    painter.drawText(versionRect, text, option);

    return logo;
}
