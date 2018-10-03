/*
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
#include "monitorwatcher.h"
#include <QDir>
#include <QFile>
#include <QDebug>

#define DRM_DIR "/sys/class/drm/"

MonitorWatcher::MonitorWatcher(QObject *parent)
    : QThread(parent),
      virtualSize(0, 0),
      monitorCount(0)
{

}

MonitorWatcher::~MonitorWatcher()
{
    requestInterruption();
    terminate();
    wait();
}

void MonitorWatcher::run()
{
    QDir drmDir(DRM_DIR);
    QStringList drms = drmDir.entryList(QDir::Dirs);
    for(auto iter = drms.begin(); iter != drms.end(); ) {
        if((*iter).indexOf("card") == 0 || iter->length() > 5)
            iter++;
        else
            iter = drms.erase(iter);
    }

    /* 每隔3秒遍历一次显卡接口的连接状态 */
    while(!isInterruptionRequested()) {
        int width   = 0;
        int height  = 0;
        QMap<QString, QString> tmpStatus;
        for(const QString & drm : drms) {
            QFile drmStatusFile(DRM_DIR + drm + "/status");
            if(drmStatusFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&drmStatusFile);
                QString status = in.readLine();
                tmpStatus[drm] = status;

                if(status == "connected") {
                    QSize maxMode = getMonitorMaxSize(drm);
                    width += maxMode.width();
                    height = height > maxMode.height() ? height : maxMode.height();
                }
            }
        }
        if(drmStatus != tmpStatus) {
            drmStatus = tmpStatus;
            int count = 0;
            for(auto &pair : tmpStatus)
                if(pair == "connected")
                    count++;
            qDebug() << "monitor count changed ---" << count;
            monitorCount = count;
            Q_EMIT monitorCountChanged(count);
        }

        virtualSize = QSize(width, height);
        sleep(3);
    }
}

/**
 * 获取显示器的最大分辨率
 */
QSize MonitorWatcher::getMonitorMaxSize(const QString &drm)
{
    int width, height;
    QFile drmModeFile(DRM_DIR + drm + "/modes");
    if(drmModeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in2(&drmModeFile);
        QString maxMode = in2.readLine();
        int xpos = maxMode.indexOf('x', 0);
        width = maxMode.left(xpos).toInt();
        int ipos = maxMode.indexOf('i', 0);
        if(ipos != -1)
            height = maxMode.mid(xpos+1, ipos-xpos-1).toInt();
        else
            height = maxMode.mid(xpos+1).toInt();
    }
    return QSize(width, height);
}

QSize MonitorWatcher::getVirtualSize()
{
    return virtualSize;
}

int MonitorWatcher::getMonitorCount()
{
    return monitorCount;
}
