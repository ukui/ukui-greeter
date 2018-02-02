#ifndef POWERWINDOW_H
#define POWERWINDOW_H

#include <QWidget>
#include <QLightDM/Power>

class QLabel;
class PowerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PowerWindow(bool hasOpenSessions = false, QWidget *parent = 0);
    void initUI();
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    void closeEvent(QCloseEvent *);

signals:
    void aboutToClose();

public slots:
private:
    bool m_hasOpenSessions;
    QLightDM::PowerInterface *m_power;

    QWidget *m_centerWidget;
    QLabel *m_prompt;
    QLabel *m_suspend;
    QLabel *m_hibernate;
    QLabel *m_restart;
    QLabel *m_shutdown;
    QLabel *m_close;
    QLabel *m_suspendLabel;
    QLabel *m_hibernateLabel;
    QLabel *m_restartLabel;
    QLabel *m_shutdownLabel;
};

#endif // POWERWINDOW_H
