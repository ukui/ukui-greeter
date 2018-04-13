#ifndef BIODEVICEVIEW_H
#define BIODEVICEVIEW_H

#include <QWidget>
#include <QList>
#include <QVector>
#include "biocustomtype.h"

//#define TEST 1

#define ICON_SIZE 32
#define ITEM_SIZE (ICON_SIZE + 4)
#define ARROW_SIZE 24
#define MAX_NUM 4
#define BIODEVICEVIEW_WIDTH (ITEM_SIZE * MAX_NUM * 2 + ARROW_SIZE * 2)
#define BIODEVICEVIEW_HEIGHT ITEM_SIZE

class QDBusInterface;
class QTableWidget;
class QLabel;
class QListWidget;
class QPushButton;

typedef QVector<QString> QStringVector;

class BioDeviceView : public QWidget
{
    Q_OBJECT

public:
    explicit BioDeviceView(qint32 uid=0, QWidget *parent=nullptr);
    void setUid(qint32 uid);
    void getDevicesList();
    void getFeaturesList();
    void initUI();
    int deviceNum();
    int usedDeviceNum();
#ifdef TEST
    void addTestDevices();
#endif

protected:
    void keyReleaseEvent(QKeyEvent *event);
    void setCurrentRow(int row);
    bool eventFilter(QObject *obj, QEvent *event);
    void focusInEvent(QFocusEvent *event);

private slots:
    void onDeviceIconClicked(int index);

signals:
    void backToPasswd();
    void startVerification(const DeviceInfo &deviceInfo);

private:

    QListWidget         *devicesList;
    QPushButton         *prevButton;
    QPushButton         *nextButton;

    QStringVector       deviceTypes;
    QDBusInterface      *serviceInterface;
    QList<DeviceInfo*>  deviceInfos;
    QMap<int, int>      deviceFeaturesNum;
    qint32              uid;
    int                 deviceCount;
    int                 currentIndex;
};

#endif // BIODEVICEVIEW_H
