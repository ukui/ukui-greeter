#include <QApplication>
#include <QDebug>
#include "biometricproxy.h"
#include "biometricauthwidget.h"
#include "biometricdeviceswidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BiometricProxy proxy;
    BiometricAuthWidget biometricAuthWidget(&proxy);
    biometricAuthWidget.hide();
    BiometricDevicesWidget biometricDeviceWidget(&proxy);
    QObject::connect(&biometricDeviceWidget, &BiometricDevicesWidget::deviceChanged,
                     &a, [&](const DeviceInfoPtr &pDeviceInfo){
        biometricAuthWidget.startAuth(pDeviceInfo, 1000);
        biometricAuthWidget.show();
    });
    QObject::connect(&biometricDeviceWidget, &BiometricDevicesWidget::deviceCountChanged,
                     &a, [&](int count){
        qDebug() << "device count changed: " << count;
    });
    biometricDeviceWidget.show();

    return a.exec();
}
