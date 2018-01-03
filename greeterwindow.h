#ifndef GREETERWINDOW_H
#define GREETERWINDOW_H

#include <QWidget>

class GreeterWindow : public QWidget
{
    Q_OBJECT

public:
    GreeterWindow(QWidget *parent = 0);
    ~GreeterWindow();
};

#endif // GREETERWINDOW_H
