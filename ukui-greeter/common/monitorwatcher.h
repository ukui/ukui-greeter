/* monitorwatcher.h
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
#ifndef MONITORWATCHER_H
#define MONITORWATCHER_H

#include <QThread>
#include <QMap>
#include <QSize>
#include <QVector>

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

    void getMonitors();

    QMap<QString, QVector<QString>> monitors;
    QStringList monitorNames;
    QString     monitorFisrst;

    QMap<QString, QString>  drmStatus;
    QSize                   virtualSize;
    int                     monitorCount;
};

#endif // MONITORWATCHER_H
