/* main.cpp
 * Copyright (C) 2018 yanghao <yanghao@kylinos.cn>
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
#include "greeterwindow.h"
#include <stdio.h>
#include <string>
#include <QApplication>
#include <QTextCodec>
#include <QResource>
#include <QTranslator>
#include <QLocale>
#include <QDesktopWidget>
#include <QRect>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>
#include "globalv.h"
#include "mainwindow.h"

float scale;
int fontSize;
QFont font;
QString configFile;
QLocale::Language language;

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QDateTime dateTime = QDateTime::currentDateTime();
    QByteArray time = QString("[%1] ").arg(dateTime.toString("MM-dd hh:mm:ss.zzz")).toLocal8Bit();
    QByteArray localMsg = msg.toLocal8Bit();
    switch(type) {
    case QtDebugMsg:
        fprintf(stderr, "%s Debug: %s:%u: %s\n", time.constData(), context.file, context.line, localMsg.constData());
        break;
//    case QtInfoMsg:
//        fprintf(stderr, "%s Info: %s:%u: %s\n", time.constData(), context.file, context.line, localMsg.constData());
//        break;
    case QtWarningMsg:
        fprintf(stderr, "%s Warnning: %s:%u: %s\n", time.constData(), context.file, context.line, localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s Critical: %s:%u: %s\n", time.constData(), context.file, context.line, localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s Fatal: %s:%u: %s\n", time.constData(), context.file, context.line, localMsg.constData());
        abort();
    }

}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);

    QResource::registerResource("image.qrc");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    language = QLocale::system().language();
    qDebug() << QLocale::languageToString(language);

    //加载翻译文件
    QTranslator translator;
    if(language == QLocale::Chinese) {
        translator.load(QM_DIR + "zh_CN.qm");
    }
    a.installTranslator(&translator);

    //加载qss文件
//    QFile qss(":/kylin-greeter.qss");
//    qss.open(QFile::ReadOnly);
//    a.setStyleSheet(qss.readAll());
//    qss.close();

    //用于QSettings
    QApplication::setOrganizationName("Kylin");
    QApplication::setApplicationName("ukui-greeter");

    configFile = QStandardPaths::displayName(QStandardPaths::CacheLocation) + "/ukui-greeter.conf";
    qDebug() << "load configure file: "<< configFile;
    //计算缩放比例
    QRect screen = QApplication::desktop()->rect();
    scale = QString::number(screen.width() / 1920.0, 'f', 1).toFloat();
    scale = scale > 1.0 ? 1.0 : (scale < 0.6 ? 0.6 : scale);
    qDebug() <<"ScreenSize:" << screen.width() << " "<< screen.height()<< ", scale: "<< scale;
//    scale = std::stof(argv[1]);
//    top = std::stoi(argv[2]);
    //字体大小
    fontSize = 10;
    font = QFont("ubuntu", fontSize);

    MainWindow w;
    w.show();
    //在没有窗口管理器的情况下，需要激活窗口，行为类似于用鼠标点击窗口
    w.activateWindow();

    return a.exec();
}
