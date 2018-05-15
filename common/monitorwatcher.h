#ifndef MONITORWATCHER_H
#define MONITORWATCHER_H

#include <QThread>
#include <QMap>
#include <QSize>

/*!
 * \brief The MonitorWatcher class
 * 监控显示器的插拔
 */
class MonitorWatcher : public QThread
{
    Q_OBJECT

public:
    MonitorWatcher(QObject *parent=nullptr);
    ~MonitorWatcher();
    QSize getVirtualSize();
    int getMonitorCount();

signals:
    void monitorCountChanged(int newCount);
    void virtualSizeChanged(const QSize &newVirtualSize);

protected:
    void run();

private:
    QSize getMonitorMaxSize(const QString &drm);

    QMap<QString, QString>  drmStatus;
    QSize                   virtualSize;
    int                     monitorCount;
};

#endif // MONITORWATCHER_H
