#include "displayservice.h"
#include <QException>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <X11/extensions/Xrandr.h>

DisplayService::DisplayService(QObject *parent) : QObject(parent)
{

}

bool DisplayService::switchDisplayMode(DisplayMode targetMode)
{
    //获取显示器列表
    getMonitors();

    //如果当前只有一个显示器就什么也不做
    if(monitors.keys().size() < 2) {
        qDebug() << "only one monitor, doNothing";
        return false;
    }

    QProcess subProcess;
    //模式切换
    try {
        switch(targetMode){
        case DISPLAY_MODE_ORI:
        {
            QString XRANDR_ORIONLY = "xrandr --output " + monitorNames[0] + " --auto";
            for(int i = 1; i < monitorNames.size(); i++)
                XRANDR_ORIONLY = XRANDR_ORIONLY + " --output " + monitorNames[i] + " --off";
            qDebug() << "XRANDR_ORIONLY: " << XRANDR_ORIONLY;

            subProcess.start(XRANDR_ORIONLY, QIODevice::NotOpen);
            break;
        }
        case DISPLAY_MODE_CLONE:
        {
            //查找最佳克隆分辨率
            QString BEST_CLONE_MODE;
            bool find = false;
            for(auto & mode_0 : monitors[monitorNames[0]]) {
                for(auto & mode_1 : monitors[monitorNames[1]]) {
                    if(mode_0 == mode_1) {
                        BEST_CLONE_MODE = mode_0;
                        find = true;
                        break;
                    }
                }
                if(find)
                    break;
            }
            QString XRANDR_CLONE = "xrandr --output " + monitorNames[0] + " --mode " + BEST_CLONE_MODE;
            for(int i = 1; i < monitorNames.size(); i++)
                XRANDR_CLONE = XRANDR_CLONE + " --output " + monitorNames[i] + " --mode " + BEST_CLONE_MODE + " --same-as " + monitorNames[0];
            qDebug() << "XRANDR_CLONE: " << XRANDR_CLONE;

            subProcess.start(XRANDR_CLONE, QIODevice::NotOpen);
            break;
        }
        case DISPLAY_MODE_EXTEND:
        {
            QString XRANDR_EXTEND = "xrandr --output " + monitorNames[0] + " --auto";
            for(int i = 1; i < monitorNames.size(); i++)
                XRANDR_EXTEND = XRANDR_EXTEND + " --output " + monitorNames[i] + " --right-of " + monitorNames[i-1] + " --auto";
            qDebug() << "XRANDR_EXTEND: " << XRANDR_EXTEND;

            subProcess.start(XRANDR_EXTEND, QIODevice::NotOpen);
            break;
        }
        case DISPLAY_MODE_ONLY_OUT:
        {
            QString XRANDR_OUTONLY = "xrandr --output " + monitorNames[0] + " --off --output " + monitorNames[1] + " --auto";

            subProcess.start(XRANDR_OUTONLY, QIODevice::NotOpen);
            break;
        }
        }
        subProcess.waitForFinished();
        return true;
    } catch(const QException &e) {
        qWarning() << e.what();
        return false;
    }
}


void DisplayService::getMonitors()
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
    monitors[name] = modes;     //将最后一个显示器的信息存储到map

    qDebug() << "find motinors: " << monitorNames;
}
