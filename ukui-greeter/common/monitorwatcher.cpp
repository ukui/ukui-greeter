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
#include <QException>
#include <QProcess>
#include <QStandardPaths>
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

void MonitorWatcher::getMonitors()
{

    QProcess subProcess;
    subProcess.setProgram(QStandardPaths::findExecutable("xrandr"));
    subProcess.setArguments({"-q"});
    subProcess.start(QIODevice::ReadOnly);
    subProcess.waitForFinished();
    QString outputs = subProcess.readAll();
    QStringList lines = outputs.split('\n');
    QString name;
    QVector<QString> modes;
    bool find = false;
    QString lastName;
    monitorNames.clear();
    monitors.clear();
    for(auto & line : lines) {
        if(line.indexOf(" connected") != -1) {   //找显示器名称
            name = line.left(line.indexOf(' '));
            monitorNames.push_back(name);
            if(find)    //又找到了一个显示器，将上一个显示器的信息存到map
                monitors[lastName] = modes;
            find = true;
            lastName = name;
            modes.clear();
        } else {
            if(line.startsWith(' ')) {      //获取分辨率
                QString mode = line.trimmed().split(' ').at(0);
                modes.push_back(mode);
            }
        }
    }
    if(!name.isEmpty())
    	monitors[name] = modes;     //将最后一个显示器的信息存储到map
}

void MonitorWatcher::run()
{
    /* 每隔3秒遍历一次显卡接口的连接状态 */
    while(!isInterruptionRequested()) {
        getMonitors();
        int tmp = monitors.keys().size();
        if(tmp != monitorCount){
            monitorCount = tmp;
            Q_EMIT monitorCountChanged(monitorCount);
        }
        sleep(3);
    }
}

/**
 * 获取显示器的最大分辨率
 */
QSize MonitorWatcher::getMonitorMaxSize(const QString &drm)
{
//    int width, height;
//    QFile drmModeFile(DRM_DIR + drm + "/modes");
//    if(drmModeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in2(&drmModeFile);
//        QString maxMode;
//        do
//	{
//          maxMode = in2.readLine();
//	}while(maxMode=="preferred");
//	int xpos = maxMode.indexOf('x', 0);
//        width = maxMode.left(xpos).toInt();
//        int ipos = maxMode.indexOf('i', 0);
//        if(ipos != -1)
//            height = maxMode.mid(xpos+1, ipos-xpos-1).toInt();
//	else
//        {
//            int p_pos = maxMode.mid(xpos+1).indexOf('p', 0);
//            if (p_pos != -1)
//            {
//                height = maxMode.mid(xpos+1).left(p_pos).toInt();
//            }
//            else
//                height = maxMode.mid(xpos+1).toInt();
//        }
//    }
//    return QSize(width, height);
    return virtualSize;
}

QSize MonitorWatcher::getVirtualSize()
{
    return virtualSize;
}

int MonitorWatcher::getMonitorCount()
{
    return monitorCount;
}
