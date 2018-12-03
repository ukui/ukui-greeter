#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QVector>

struct Language
{
    QString code;
    QString name;
    QString territory;
};

typedef QVector<Language> LanguagesVector;

LanguagesVector &getLanguages();

#endif // LANGUAGE_H
