#include "biometricdeviceswidget.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>
#include <QAbstractItemView>
#include <QStyledItemDelegate>


BiometricDevicesWidget::BiometricDevicesWidget(BiometricProxy *proxy, QWidget *parent)
    : QWidget(parent),
      proxy(proxy)
{
    initUI();
    if(proxy && proxy->isValid())
    {
        connect(proxy, &BiometricProxy::USBDeviceHotPlug,
                this, &BiometricDevicesWidget::onUSBDeviceHotPlug);
        updateDevice();
    }

    resize(500, 500);
}

void BiometricDevicesWidget::initUI()
{
    lblPrompt = new QLabel(this);
    lblPrompt->setObjectName(QStringLiteral("lblBioetricDevicesPrompt"));
    lblPrompt->setText(tr("Please select the biometric device"));
    lblPrompt->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    lblDeviceType = new QLabel(this);
    lblDeviceType->setObjectName(QStringLiteral("lblDeviceType"));
    lblDeviceType->setText(tr("Device type:"));
    lblDeviceType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();

    cmbDeviceType = new QComboBox(this);
    cmbDeviceType->setObjectName(QStringLiteral("cmbDeviceType"));
    cmbDeviceType->setMaxVisibleItems(5);
    cmbDeviceType->setItemDelegate(itemDelegate);
    connect(cmbDeviceType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onCmbDeviceTypeCurrentIndexChanged(int)));

    lblDeviceName = new QLabel(this);
    lblDeviceName->setObjectName(QStringLiteral("lblDeviceName"));
    lblDeviceName->setText(tr("Device name:"));
    lblDeviceName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    cmbDeviceName = new QComboBox(this);
    cmbDeviceName->setObjectName(QStringLiteral("cmbDeviceName"));
    cmbDeviceName->setMaxVisibleItems(5);
    cmbDeviceName->setItemDelegate(itemDelegate);

    btnOK = new QPushButton(tr("OK"), this);
    btnOK->setObjectName(QStringLiteral("OKButton"));
    btnOK->setCursor(Qt::PointingHandCursor);
    connect(btnOK, &QPushButton::clicked,
            this, &BiometricDevicesWidget::onOKButtonClicked);
}

void BiometricDevicesWidget::resizeEvent(QResizeEvent */*event*/)
{
    lblPrompt->setGeometry(0, 0, width(), 40);
    lblDeviceType->setGeometry(0, lblPrompt->geometry().bottom() + 40,
                               120, 20);
    cmbDeviceType->setGeometry(0, lblDeviceType->geometry().bottom() + 15,
                               300, 40);
    lblDeviceName->setGeometry(0, cmbDeviceType->geometry().bottom() + 80,
                               120, 20);
    cmbDeviceName->setGeometry(0, lblDeviceName->geometry().bottom() + 15,
                               300, 40);
    btnOK->setGeometry(0, cmbDeviceName->geometry().bottom() + 80, 140, 38);

}

void BiometricDevicesWidget::updateDevice()
{
    deviceMap.clear();
    DeviceList deviceList = proxy->GetDevList();
    for(auto pDeviceInfo : deviceList)
    {
        qDebug() << *pDeviceInfo;
        deviceMap[pDeviceInfo->deviceType].push_back(pDeviceInfo);
    }
    cmbDeviceType->clear();
    for(int type : deviceMap.keys())
    {
        QString iconPath = QString(UKUI_BIOMETRIC_IMAGES_PATH"icon/%1.png")
                .arg(DeviceType::getDeviceType(type));
        qDebug() << iconPath;
        cmbDeviceType->addItem(QIcon(iconPath), DeviceType::getDeviceType_tr(type), type);
    }
    if(deviceMap.size() > 0)
    {
        int index = deviceMap.keys().at(0);
        setCurrentDevice(deviceMap[index].at(0));
    }
}


void BiometricDevicesWidget::setCurrentDevice(int drvid)
{
    DeviceInfoPtr pDeviceInfo = findDeviceById(drvid);
    if(pDeviceInfo)
    {
        setCurrentDevice(pDeviceInfo);
    }
}

void BiometricDevicesWidget::setCurrentDevice(const QString &deviceName)
{
    DeviceInfoPtr pDeviceInfo = findDeviceByName(deviceName);
    if(pDeviceInfo)
    {
        setCurrentDevice(pDeviceInfo);
    }
}

void BiometricDevicesWidget::setCurrentDevice(const DeviceInfoPtr &pDeviceInfo)
{
    this->currentDevice = pDeviceInfo;
    cmbDeviceType->setCurrentText(DeviceType::getDeviceType_tr(pDeviceInfo->deviceType));
    cmbDeviceName->setCurrentText(pDeviceInfo->shortName);
}

bool BiometricDevicesWidget::deviceExists(int drvid)
{
    return (findDeviceById(drvid) != nullptr);
}

bool BiometricDevicesWidget::deviceExists(const QString &deviceName)
{
    return (findDeviceByName(deviceName) != nullptr);
}

DeviceInfoPtr BiometricDevicesWidget::findDeviceById(int drvid)
{
    for(int type : deviceMap.keys())
    {
        DeviceList &deviceList = deviceMap[type];
        auto iter = std::find_if(deviceList.begin(), deviceList.end(),
                                 [&](DeviceInfoPtr ptr){
            return ptr->id == drvid;
        });
        if(iter != deviceList.end())
        {
            return *iter;
        }
    }
    return DeviceInfoPtr();
}

DeviceInfoPtr BiometricDevicesWidget::findDeviceByName(const QString &name)
{
    for(int type : deviceMap.keys())
    {
        DeviceList &deviceList = deviceMap[type];
        auto iter = std::find_if(deviceList.begin(), deviceList.end(),
                                 [&](DeviceInfoPtr ptr){
            return ptr->shortName == name;
        });
        if(iter != deviceList.end())
        {
            return *iter;
        }
    }
    return DeviceInfoPtr();
}

void BiometricDevicesWidget::onCmbDeviceTypeCurrentIndexChanged(int index)
{
    if(index < 0 || index >= deviceMap.keys().size())
    {
        return;
    }
    int type = cmbDeviceType->itemData(index).toInt();
    cmbDeviceName->clear();
    for(auto &deviceInfo : deviceMap.value(type))
    {
        cmbDeviceName->addItem(deviceInfo->shortName);
    }
}

void BiometricDevicesWidget::onOKButtonClicked()
{
    int type = cmbDeviceType->currentData().toInt();
    int index = cmbDeviceName->currentIndex();
    qDebug() << type << index;
    DeviceInfoPtr deviceInfo = deviceMap.value(type).at(index);
    Q_EMIT deviceChanged(deviceInfo);
    hide();
}

void BiometricDevicesWidget::onUSBDeviceHotPlug(int drvid, int action, int /*devNum*/)
{
    int savedDeviceId = currentDevice->id;
    int savedCount = 0;
    for(int type : deviceMap.keys())
        savedCount += deviceMap.value(type).count();

    switch(action)
    {
    case ACTION_ATTACHED:
    {
        //插入设备后，需要更新设备列表
        deviceMap.clear();
        updateDevice();
        setCurrentDevice(savedDeviceId);
        break;
    }
    case ACTION_DETACHED:
    {
        DeviceInfoPtr pDeviceInfo = findDeviceById(drvid);
        if(pDeviceInfo)
        {
            int type = pDeviceInfo->deviceType;
            deviceMap[type].removeOne(pDeviceInfo);
            int index = cmbDeviceName->findText(pDeviceInfo->shortName);
            cmbDeviceName->removeItem(index);

            //如果该类型的设备全被移除，删除该类型相关的列表
            if(deviceMap[type].isEmpty())
            {
                deviceMap.remove(type);
                index = cmbDeviceType->findData(type);
                cmbDeviceType->removeItem(index);
            }
        }
        if(savedDeviceId != drvid)
        {
            setCurrentDevice(savedDeviceId);
        }
        break;
    }
    }

    int count = 0;
    for(int type : deviceMap.keys())
        count += deviceMap.value(type).count();

    //设备数量发生了变化
    if(count != savedCount)
    {
        Q_EMIT deviceCountChanged(count);
    }
}


