#ifndef POWERMANAGER_H
#define POWERMANAGER_H
#include <QWidget>
#include <QListWidget>
#include <QLightDM/Power>
#include <QSize>
#include <QTime>

#define ITEM_WIDTH 168
#define ITEM_HEIGHT ITEM_WIDTH
#define ITEM_SPACING (ITEM_WIDTH/8)

class QListWidget;
class QListWidgetItem;
class PowerManager:public QListWidget
{
    Q_OBJECT

public:
    PowerManager(QWidget *parent = 0);
    QSize windowSize();

private:
    void initUI();
    QListWidget *list;
    QWidget *lockWidget;
    QWidget *switchWidget;
    QWidget *suspendWidget;
    QWidget *rebootWidget;
    QWidget *shutdownWidget;
    QLightDM::PowerInterface *m_power;
    QTime lasttime;

private:
    void lockWidgetClicked();
    void switchWidgetClicked();
    void suspendWidgetCliced();
    void rebootWidgetClicked();
    void shutdownWidgetClicked();

private Q_SLOTS:
    void powerClicked(QListWidgetItem *item);

Q_SIGNALS:
    void switchToUser();
    void lock();
};

#endif // POWERMANAGER_H
