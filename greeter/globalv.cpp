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
#include <QIcon>
#include <QFontMetrics>
#include <QPainter>
#include <QProcess>
#include <QStandardPaths>
#include <QImageReader>
#include <QtSvg/QSvgRenderer>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

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
    QImage image(url);
    QPixmap pixmap = QPixmap::fromImage(image);
    if(pixmap.isNull()) {
        qDebug() << "pixmap is null";
        QProcess exec;
        QString program("file " + url);
        exec.start(program);
        exec.waitForFinished(1000);
        QString output = exec.readAllStandardOutput();
        qDebug() << output;
        if(output.contains("SVG")){
            qDebug() << "image format is SVG";
            QSvgRenderer render(url);
            QImage image(width, height, QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            render.render(&painter, image.rect());
            pixmap.convertFromImage(image);
        } else if(output.contains("TIFF")) {
            qDebug() << "image format is TIFF";

        }
    }

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
    bool capsLock = false;
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

QString getSystemDistrib()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString distribId = settings.value("DISTRIB_ID").toString();
    return distribId;
}

/**
 * @brief logoGenerator
 * @param text
 * @return
 * 生成logo
 */
QPixmap logoGenerator(const QString &text)
{
    if(getSystemDistrib() == "Kylin")
        return QPixmap(":/resource/kylin-logo.png");

    QString logoFile(IMAGE_DIR + "logo.png");
    if(QFile(logoFile).exists())
    {
        QPixmap logo(IMAGE_DIR + "logo.png");
        return logo;
    }
    QPixmap logoBare;
    logoBare.load(":/resource/uk-logo.png");

    QFont font("ubuntu", 18);
    QFontMetrics fm(font);
    int textPixelSize = fm.width(text);

    QPixmap logo(logoBare.width() + textPixelSize + 3 + 30, logoBare.height());
    logo.fill(Qt::transparent);

    QRect logoBareRect(30 , 0, logoBare.width(), logoBare.height());
    QPainter painter;
    painter.begin(&logo);
    painter.drawPixmap(logoBareRect, logoBare, logoBare.rect());

    painter.setPen(Qt::white);
    painter.setFont(font);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);
    QRect versionRect(logoBareRect.right()+3, 3, logo.width() - logoBare.width(), logo.height());
    painter.drawText(versionRect, text, option);

    return logo;
}
