#ifndef KEYEVENTMONITOR_H
#define KEYEVENTMONITOR_H

#include <QThread>
#include <X11/Xlib.h>
#include <X11/extensions/record.h>

class KeyEventMonitor : public QThread
{
	Q_OBJECT

public:
	KeyEventMonitor(QObject *parent = 0);
    ~KeyEventMonitor();

Q_SIGNALS:
    void displaySwitchSelect();
    void displaySwitchConfirm();

public Q_SLOTS:
	void isReleaseAlt(int code);

protected:
	static void callback(XPointer trash, XRecordInterceptData* data);
	void handleRecordEvent(XRecordInterceptData *);
	void run();

private:
//    bool isRelease;
    bool hasModifer;
};

#endif // KEYEVENTMONITOR_H
