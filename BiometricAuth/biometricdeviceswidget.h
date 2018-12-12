#ifndef BIOMETRICDEVICESWIDGET_H
#define BIOMETRICDEVICESWIDGET_H

#include <QWidget>
#include "biometricproxy.h"

class QLabel;
class QPushButton;
class QComboBox;


class BiometricDevicesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BiometricDevicesWidget(BiometricProxy *proxy, QWidget *parent = nullptr);
    void setCurrentDevice(int drvid);
    void setCurrentDevice(const QString &deviceName);
    void setCurrentDevice(const DeviceInfoPtr &pDeviceInfo);
    DeviceInfoPtr findDeviceById(int drvid);
    DeviceInfoPtr findDeviceByName(const QString &name);
    bool deviceExists(int drvid);
    bool deviceExists(const QString &deviceName);

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void deviceChanged(const DeviceInfoPtr &pDeviceInfo);
    void deviceCountChanged(int newCount);

private slots:
    void onCmbDeviceTypeCurrentIndexChanged(int index);
    void onOKButtonClicked();
    void onUSBDeviceHotPlug(int drvid, int action, int devNum);

private:
    void initUI();
    void updateDevice();

private:
    typedef QMap<int, QPushButton*> QButtonMap;

    QLabel              *lblPrompt;
    QLabel              *lblDeviceType;
    QLabel              *lblDeviceName;
    QComboBox           *cmbDeviceType;
    QComboBox           *cmbDeviceName;
    QPushButton         *btnOK;
    QPushButton         *btnCancel;

    BiometricProxy      *proxy;
    DeviceMap           deviceMap;
    DeviceInfoPtr       currentDevice;
};

#endif // BIOMETRICDEVICESWIDGET_H
