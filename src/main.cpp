#include "greeterwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QResource>
#include <QRect>
#include <QDesktopWidget>
#include <QDebug>
#include "globalv.h"
#include "screenmanager.h"
float scale;
int fontSize;
QFont font;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
    QTextCodec::setCodecForLocale(codec);

    //计算缩放比例
    QRect screen = QApplication::desktop()->rect();
    scale = QString::number(screen.width() / 1920.0, 'f', 1).toFloat();
    scale = scale > 1.0 ? 1.0 : (scale < 0.5 ? 0.5 : scale);
    qDebug() <<"screen:" << screen.width() << " "<< screen.height()<< " scale: "<< scale;
    scale = 1.0;
    //字体大小
    fontSize = 10;
    font = QFont("ubuntu", fontSize);

    QResource::registerResource("./resource.qrc");
    /*
    GreeterWindow *w = new GreeterWindow();
    w->show();
    */
    ScreenManager sm;
    return a.exec();
}
