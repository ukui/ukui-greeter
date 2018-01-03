#include "greeterwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "iconedit.h"
#include "loginwindow.h"
#include <QImage>
#include <QDebug>
#include "userentry.h"
#include <QResource>
#include "pagelistview.h"
#include "usersmodel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
//    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);

    QResource::registerResource("./resource.qrc");

//    GreeterWindow w;

    PageListView *w = new PageListView();
//    QRect listRect(this->rect().width()/2 - w->width() / 2, this->rect().height()/2 - w->height()/2, w->width(), w->height());
//    w->setGeometry(listRect);
    UsersModel *model = new UsersModel();
    for(int i = 0; i < 3; i++)
    {
        QStandardItem *item = new QStandardItem("test" + QString::number(i));
        model->extraRowModel()->appendRow(item);
    }
    w->setModel(model);

    w->show();

    /*
    QPixmap pixmap("/usr/share/kylin-greeter/default_face.png");
    pixmap.load("/usr/share/kylin-greeter/arrow_left_active.png");
    qDebug() << pixmap.size();
    */

    return a.exec();
}
