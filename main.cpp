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
#include "userwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
//    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);

    QResource::registerResource("./resource.qrc");

//    GreeterWindow w;

/*    PageListView *w = new PageListView();
    UsersModel *model = new UsersModel();
    for(int i = 0; i < 3; i++)
    {
        QStandardItem *item = new QStandardItem("test" + QString::number(i));
        model->extraRowModel()->appendRow(item);
    }
    w->setModel(model);
*/
    GreeterWindow *w = new GreeterWindow();

    w->show();

    /*
    QPixmap pixmap("/usr/share/kylin-greeter/default_face.png");
    pixmap.load("/usr/share/kylin-greeter/arrow_left_active.png");
    qDebug() << pixmap.size();
    */

    return a.exec();
}
