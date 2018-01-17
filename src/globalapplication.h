#ifndef GLOBALAPPLICATION_H
#define GLOBALAPPLICATION_H
#include <QApplication>

class ScreenManager;
class GlobalApplication : public QApplication
{
    Q_OBJECT
public:
    GlobalApplication(int argc, char **argv);
//    ~GlobalApplication() { }
    bool notify(QObject *, QEvent *);
    void setScreenManager(ScreenManager*);

signals:
    void mouseMove(const QPoint&);

private:
    ScreenManager* m_screenManager;
};

#endif // GLOBALAPPLICATION_H
