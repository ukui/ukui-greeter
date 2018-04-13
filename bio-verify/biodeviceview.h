#ifndef BIODEVICEVIEW_H
#define BIODEVICEVIEW_H

#include <QWidget>
#include <QList>
#include <QVector>
#include "biocustomtype.h"

#define TEST 1

#define ICON_SIZE 32
#define ITEM_SIZE (ICON_SIZE + 4)
#define ARROW_SIZE 24
#define MAX_NUM 4
#define LISTWIDGET_WIDTH (ITEM_SIZE * MAX_NUM * 2)
#define LISTWIDGET_HEIGHT ITEM_SIZE
#define BIODEVICEVIEW_WIDTH (LISTWIDGET_WIDTH + ARROW_SIZE * 2)
#define BIODEVICEVIEW_HEIGHT (ITEM_SIZE + 50)

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
    int deviceNum();
    int usedDeviceNum();
    void initUI();
    void pageUp();
    void pageDown();
#ifdef TEST
    void addTestDevices();
#endif

protected:
    void keyReleaseEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void focusInEvent(QFocusEvent *event);
    void setCurrentRow(int row);
    void setPromptText(int index);

private slots:
    void onDeviceIconClicked(int index);

signals:
    void backToPasswd();
    void startVerification(const DeviceInfo &deviceInfo);

private:

    QListWidget         *devicesList;
    QLabel              *promptLabel;
    QPushButton         *prevButton;
    QPushButton         *nextButton;

    QStringVector       deviceTypes;
    QDBusInterface      *serviceInterface;
    QList<DeviceInfo*>  deviceInfos;
    QMap<int, int>      savedFeaturesNum;       //[uid, featuresNum]
    qint32              uid;
    int                 deviceCount;
    int                 currentIndex;
};

#endif // BIODEVICEVIEW_H
