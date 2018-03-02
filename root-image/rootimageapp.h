#ifndef ROOTIMAGEAPP_H
#define ROOTIMAGEAPP_H

#include <QApplication>

class RootImageApp : public QApplication
{
    Q_OBJECT
public:
    RootImageApp(int &argc, char **argv);
private slots:
    void setBackground();
private:
};

#endif // ROOTIMAGEAPP_H

