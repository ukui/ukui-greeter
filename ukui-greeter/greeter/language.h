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
#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QVector>

struct Language
{
    /// 语言码，例如zh_CN, en, en_US
    QString code;

    /// 语言码对应的名称，中文, 英语
    QString name;

    /// 地区
    QString territory;
};

typedef QVector<Language> LanguagesVector;

/// 获取语言列表
LanguagesVector &getLanguages();

/// 根据LANG获取语言
Language getLanguage(const QString &lang);

#endif // LANGUAGE_H
