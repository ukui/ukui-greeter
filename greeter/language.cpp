#include "language.h"
#include <QProcess>
#include <langinfo.h>
#include <libintl.h>

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
        }
        setlocale(LC_ALL, currentLocale);
    }
    return territory;
}

LanguagesVector& getLanguages()
{
    static LanguagesVector languagesVector;
    if(!languagesVector.isEmpty())
        return languagesVector;

    QProcess process;
    QString command = "/usr/share/language-tools/language-options";

    process.start(command);
    process.waitForFinished(3000);
    QString result = process.readAll();
    QStringList langs = result.split("\n");

    for(auto lang : langs)
    {
        QString name = getName(lang);
        QString territory = getTerritory(lang);
        Language language{lang, name, territory};
        languagesVector.push_back(language);
    }
    return languagesVector;
}

Language &getLanguage(const QString &lang)
{
    LanguagesVector &languages = getLanguages();
    QString code = lang;

    if(code.toLower().lastIndexOf(".utf-8") != -1)
    {
        code = code.left(code.length() - 6);
    }

    auto iter = std::find_if(languages.begin(), languages.end(),
                             [&](const Language &language){
        return language.code == code;
    });

    return *iter;
}
