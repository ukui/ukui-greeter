#ifndef DISPLAYSERVICE_H
#define DISPLAYSERVICE_H

#include <QObject>
#include <QVector>
#include <QMap>

enum DisplayMode{
    DISPLAY_MODE_ORI        = 1,
    DISPLAY_MODE_CLONE      = 2,
    DISPLAY_MODE_EXTEND     = 3,
    DISPLAY_MODE_ONLY_OUT   = 4
};

class DisplayService : public QObject
{
    Q_OBJECT
public:
    explicit DisplayService(QObject *parent = nullptr);
    bool switchDisplayMode(DisplayMode targetMode);

private:
    void getMonitors();

    QMap<QString, QVector<QString>> monitors;
    QStringList monitorNames;
};

#endif // DISPLAYSERVICE_H
