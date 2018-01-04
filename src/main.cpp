#include "greeterwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QResource>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
    QTextCodec::setCodecForLocale(codec);

    QResource::registerResource("./resource.qrc");
    GreeterWindow *w = new GreeterWindow();

    w->show();

    return a.exec();
}
