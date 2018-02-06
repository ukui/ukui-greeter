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
    case QtInfoMsg:
        fprintf(stderr, "%s Info: %s:%u: %s\n", time.constData(), context.file, context.line, localMsg.constData());
        break;
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

    QResource::registerResource("resource.qrc");
    QResource::registerResource("translate.qrc");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    language = QLocale::system().language();
    qDebug() << QLocale::languageToString(language);

    //加载翻译文件
    QTranslator translator;
    if(language == QLocale::Chinese) {
        translator.load(":/ts/zh_CN.qm");
    }
    a.installTranslator(&translator);

    //用于QSettings
    QApplication::setOrganizationName("Kylin");
    QApplication::setApplicationName("Greeter");

    configFile = QStandardPaths::displayName(QStandardPaths::CacheLocation) + "/kylin-greeter.conf";
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

    return a.exec();
}
