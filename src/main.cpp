#include "greeterwindow.h"
#include <stdio.h>
#include <QApplication>
#include <QTextCodec>
#include <QResource>
#include <QTranslator>
#include <QDesktopWidget>
#include <QRect>
#include <QMutex>
#include <QDateTime>
#include <QDebug>
#include "globalv.h"
#include "mainwindow.h"
#include "sessionwindow.h"
#include <QLightDM/SessionsModel>

#define QT_MESSAGELOGCONTEXT

float scale;
int fontSize;
QFont font;

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
    case QtInfoMsg:
        fprintf(stderr, "%s Info: %s:%u: %s\n", time.constData(), __FILE__, __LINE__, localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s Warnning: %s:%u: %s\n", time.constData(), __FILE__, __LINE__, localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s Critical: %s:%u: %s\n", time.constData(), __FILE__, __LINE__, localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s Fatal: %s:%u: %s\n", time.constData(), __FILE__, __LINE__, localMsg.constData());
        abort();
    }

}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);

    //计算缩放比例
    QRect screen = QApplication::desktop()->rect();
    scale = QString::number(screen.width() / 1920.0, 'f', 1).toFloat();
    scale = scale > 1.0 ? 1.0 : (scale < 0.5 ? 0.5 : scale);
    qDebug() <<"ScreenSize:" << screen.width() << " "<< screen.height()<< ", scale: "<< scale;
    scale = 1.0;
    //字体大小
    fontSize = 10;
    font = QFont("ubuntu", fontSize);

    QResource::registerResource("resource.qrc");
    QResource::registerResource("translate.qrc");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QTranslator translator;
    translator.load(":/qm/zh_CN.qm");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();


    return a.exec();
}
