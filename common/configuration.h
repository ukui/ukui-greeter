#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QSettings>
#include <QPixmap>

class Configuration : public QObject
{
    Q_OBJECT
private:
    explicit Configuration(QObject *parent = nullptr);

public:
    static Configuration *instance(QObject *parent = nullptr);

    QVariant getValue(const QString &);
    void setValue(const QString&, const QVariant &);

    QString getLastLoginUser();
    void saveLastLoginUser(const QString &);
    QPixmap getLogo();

private:
    QSettings               *configSettings;
    QSettings               *recordSettings;

    static Configuration    *instance_;
};

#endif // CONFIGURATION_H
