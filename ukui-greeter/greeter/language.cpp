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
#include "language.h"
#include <QProcess>
#include <langinfo.h>
#include <libintl.h>
#include <QDebug>

QString getLocaleName(const QString &code)
{
    static QStringList locales;

    QString language;
    int at;
    if((at = code.indexOf('@')) != -1)
        language = code.left(at);
    else
        language = code;

    if(locales.isEmpty())
    {
        QProcess process;
        QString command = "locale -a";

        process.start(command);
        process.waitForFinished(3000);
        QString result = process.readAll();
        locales = result.split("\n");
    }

    for(auto locale : locales)
    {
        if(locale.lastIndexOf(".utf8") == -1)
            continue;
        if(!locale.isEmpty() && locale.indexOf(language) != -1)
            return locale;
    }
    return "";
}

QString getName(const QString &code)
{
    QString name;
    QString locale = getLocaleName(code);

    if(!locale.isEmpty())
    {
        char *currentLocale = setlocale(LC_ALL, NULL);
        setlocale(LC_IDENTIFICATION, locale.toUtf8().data());
        setlocale(LC_MESSAGES, "");

        char *language = nl_langinfo(_NL_IDENTIFICATION_LANGUAGE);
        if(language && strlen(language) > 0)
        {
            name = QString(dgettext("iso_639_3", language));
        }
        setlocale(LC_ALL, currentLocale);
    }
    return name;
}

QString getTerritory(const QString &code)
{
    if(code.indexOf('_') == -1)
        return "";

    QString territory;
    QString locale = getLocaleName(code);
    if(!locale.isEmpty())
    {
        char *currentLocale = setlocale(LC_ALL, NULL);
        setlocale(LC_IDENTIFICATION, locale.toUtf8().data());
        setlocale(LC_MESSAGES, "");

        char *country = nl_langinfo(_NL_IDENTIFICATION_TERRITORY);
        if(country && strlen(country) > 0 && strcmp(country, "ISO") != 0)
        {
            territory = QString(dgettext("iso_3166", country));
            QString str = QString(QLatin1String(country));
            if(str.contains("taiwan",Qt::CaseInsensitive))
            {
                territory = "台湾";
            }
        }
        setlocale(LC_ALL, currentLocale);
    }
    return territory;
}

LanguagesVector& getLanguages()
{
    static LanguagesVector languagesVector;
    if(!languagesVector.isEmpty())
    	languagesVector.clear(); 

    QProcess process;
    QString command = "/usr/share/language-tools/language-options";

    process.start(command);
    process.waitForFinished(3000);
    QString result = process.readAll();
    QStringList langs = result.split("\n");

    if(result.isEmpty())
        return languagesVector;

    for(auto lang : langs)
    {
        QString name = getName(lang);
        QString territory = getTerritory(lang);
        Language language{lang, name, territory};
        languagesVector.push_back(language);
    }
    return languagesVector;
}

Language getLanguage(const QString &lang)
{
    LanguagesVector &languages = getLanguages();
    QString code = lang;

    if(code.toLower().lastIndexOf(".utf-8") != -1)
    {
        code = code.left(code.length() - 6);
    }

    for(int i=0;i<languages.count();i++)
    {
        if(languages.at(i).code == code){
            return languages.at(i);
        }
    }

    Language nolanguage;
    nolanguage.code = "";
    nolanguage.territory = "";
    nolanguage.name = "";

    return nolanguage;
}
