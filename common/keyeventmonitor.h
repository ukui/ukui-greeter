#ifndef KEYEVENTMONITOR_H
#define KEYEVENTMONITOR_H

#include <QThread>
#include <X11/Xlib.h>
#include <X11/extensions/record.h>

class KeyEventMonitor : public QThread
{
	Q_OBJECT

private:
    explicit KeyEventMonitor(QObject *parent = 0);
    bool checkCapsState();

public:
    ~KeyEventMonitor();
    static KeyEventMonitor *instance(QObject *parent = 0);

Q_SIGNALS:
    void CapsLockChanged(int);
    void displaySwitchSelect();
    void displaySwitchConfirm();

protected:
	static void callback(XPointer trash, XRecordInterceptData* data);
	void handleRecordEvent(XRecordInterceptData *);
	void run();

private:
    bool                    hasModifer;
    bool                    isCapsLock;
    static KeyEventMonitor *instance_;
};

#endif // KEYEVENTMONITOR_H
