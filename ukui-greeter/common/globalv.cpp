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
#include <QPixmap>
#include <QProcess>
#include <QPainter>
#include <QMimeDatabase>
#include <QtSvg/QSvgRenderer>

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

bool isPicture(QString filepath)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filepath);
    qDebug() << "mime: " << filepath << mime.name();
    return mime.name().startsWith("image/");
}
