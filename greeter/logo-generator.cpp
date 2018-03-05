/* logo-generator.cpp
 * Copyright (C) 2018 yanghao <yanghao@kylinos.cn>
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

#include <QPixmap>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QRegularExpression>
#include "globalv.h"

/**
 * @brief getSystemVersion
 * @return
 * 获取系统版本号
 */
QString getSystemVersion()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString version = settings.value("DISTRIB_DESCRIPTION").toString();
    int n = version.length() - version.indexOf(QRegularExpression("\\d"));
    version = version.right(n);
    return version;
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
