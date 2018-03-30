#ifndef DISPLAYSWITCH_H
#define DISPLAYSWITCH_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class KeyEventMonitor;
class DisplaySwitch : public QMainWindow
{
    Q_OBJECT
public:
    explicit DisplaySwitch(QWidget *parent = nullptr);
    void initUi();

signals:

public slots:
    void onDisplaySwitchSelect();
    void onDisplaySwitchConfirm();
    void onPositionChanged(const QRect&);

private:
    Ui::MainWindow *ui;
    int     currentMode;
    KeyEventMonitor *keyEventMonitor;
};

#endif // DISPLAYSWITCH_H
