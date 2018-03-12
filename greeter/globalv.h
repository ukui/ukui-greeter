/* globalv.h
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

#ifndef GLOBALV_H
#define GLOBALV_H

#include <QPixmap>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QLocale>

#define STR(s) #s
#define GET_STR(s) STR(s)

#define PREFIX QString(GET_STR(RESOURCE_PATH))
#define IMAGE_DIR PREFIX+"/images/"
#define QM_DIR PREFIX+"/translations/"

class QPoint;
class QRect;

extern float scale;     //缩放比例
extern int fontSize;    //字体大小
extern QFont font;      //字体
extern QString configFile; //配置文件路径
extern QLocale::Language language;

QPixmap scaledPixmap(int width, int height, QString url);
QPixmap logoGenerator(const QString &);
QString getSystemVersion();
#endif // GLOBALV_H

