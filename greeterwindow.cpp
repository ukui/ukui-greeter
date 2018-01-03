#include "greeterwindow.h"
#include "iconedit.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "ui_login.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>
#include "pagelistview.h"
#include "usersmodel.h"

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent)
{
    QRect screen = QApplication::desktop()->rect();
    setGeometry(screen);
    this->setAutoFillBackground(true);
    QPalette palette;
    QPixmap background("/usr/share/kylin-greeter/background.png");
    background = background.scaled(screen.width(), screen.height(),  Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    palette.setBrush(QPalette::Window, QBrush(background));
    this->setPalette(palette);

    PageListView *w = new PageListView(this);
    QRect listRect(this->rect().width()/2 - w->width() / 2, this->rect().height()/2 - w->height()/2, w->width(), w->height());
    w->setGeometry(listRect);
    UsersModel *model = new UsersModel(this);
    for(int i = 0; i < 3; i++)
    {
        QStandardItem *item = new QStandardItem("test" + QString::number(i));
        model->extraRowModel()->appendRow(item);
    }
    w->setModel(model);

    /*
    UserWindow *userWindow = new UserWindow(this);
    QRect userRect(this->rect().width()/2 - 100, this->rect().height()/2 - 125, 200, 250);
    userWindow->setGeometry(userRect);
    */

    /*
    LoginWindow *loginWindow = new LoginWindow(this);
    QRect loginRect(this->rect().width()/2 - 300, this->rect().height()/2 - 90, 600, 180);
    loginWindow->setGeometry(loginRect);
    */
}

GreeterWindow::~GreeterWindow()
{

}
