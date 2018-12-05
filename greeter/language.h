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
Language &getLanguage(const QString &lang);

#endif // LANGUAGE_H
