/* biodeviceview.cpp
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#include "biodeviceview.h"
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QHeaderView>
#include <QtMath>
#include <QLabel>
#include <QMovie>
#include <QListWidget>
#include <QPushButton>
#include <QKeyEvent>
#include "bioauthentication.h"

BioDeviceView::BioDeviceView(qint32 uid, QWidget *parent)
    : QWidget(parent),
      devicesList(nullptr),
      promptLabel(nullptr),
      prevButton(nullptr),
      nextButton(nullptr),
      uid(uid),
      currentIndex(0),
      authControl(nullptr)
{
    BioDevices bioDevices;
    deviceInfos = bioDevices.getAvaliableDevices(uid);
    deviceCount = deviceInfos.size() + 1;

    if(deviceCount > 1){
        deviceTypes << "fingerprint" << "fingervein" << "iris" << "face" << "voiceprint";
        initUI();
    }
}

void BioDeviceView::initUI()
{
#ifdef TEST
    addTestDevices();
#endif
    /* 只为该用户显示录入了生物特征的设备 */

    devicesList = new QListWidget(this);
    devicesList->setObjectName(QStringLiteral("devicesList"));
    devicesList->setFlow(QListWidget::LeftToRight);
    devicesList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    devicesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    devicesList->setFocusPolicy(Qt::NoFocus);
    devicesList->setSelectionMode(QListWidget::NoSelection);
    devicesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    devicesList->setFixedSize(LISTWIDGET_WIDTH, LISTWIDGET_HEIGHT);
    connect(devicesList, &QListWidget::currentItemChanged, this, [&]{
        currentIndex = devicesList->currentRow();
        QListWidgetItem *item = devicesList->currentItem();
        devicesList->itemWidget(item)->setFocus();
        setPromptText(currentIndex);
    });


    int itemSize;
    if(deviceCount <= MAX_NUM)
        itemSize = (LISTWIDGET_WIDTH - deviceCount * 10) / deviceCount;
    else
        itemSize = (LISTWIDGET_WIDTH - MAX_NUM * 10) / MAX_NUM;
#define LABEL_WIDTH itemSize
#define ITEM_WIDTH (LABEL_WIDTH + 10)

    /* 为每一个设备添加一个icon到table中 */
    for(int i = 0; i < deviceCount; i++){
        QString iconName;
        if(i == 0)
            iconName = QString(":/resource/password-icon.png");
        else{
            DeviceInfo deviceInfo = deviceInfos.at(i - 1);
            QString deviceType = deviceTypes.at(deviceInfo.biotype);
            iconName = QString(":/resource/%1-icon.png").arg(deviceType);
        }

        QListWidgetItem *item = new QListWidgetItem(devicesList);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_SIZE));

        QLabel *iconLabel = new QLabel(this);
        iconLabel->setObjectName(QString("bioIconLabel")+QString::number(i));
        iconLabel->installEventFilter(this);
        iconLabel->setFixedSize(LABEL_WIDTH, ITEM_SIZE);
        iconLabel->setStyleSheet("QLabel{border:1px solid #026096;}"
                                 "QLabel::hover{background:rgb(255, 255, 255, 100);}"
                                 "QLabel::focus{background:white;}");
        QPixmap icon(iconName);
        icon = icon.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        iconLabel->setPixmap(icon);
        iconLabel->setAlignment(Qt::AlignCenter);

        devicesList->insertItem(i, item);
        devicesList->setItemWidget(item, iconLabel);
    }

    /* 设备名称提示 */
    promptLabel = new QLabel(this);
    QRect promptLbRect(devicesList->geometry().left(),
                       devicesList->geometry().bottom() + 10,
                       devicesList->width(), 25);
    promptLabel->setGeometry(promptLbRect);

    setCurrentRow(0);

    /* 操作提示 */
    notifyLabel = new QLabel(this);
    QRect notifyLbRect(promptLbRect.left(), promptLbRect.bottom() + 10,
                       promptLbRect.width(), 25);
    notifyLabel->setGeometry(notifyLbRect);
    notifyLabel->setText("this is a notify label");

    /* 翻页按键 */
    if(deviceCount > MAX_NUM) {
        prevButton = new QPushButton(this);
        prevButton->setObjectName(QStringLiteral("bioPrevButton"));
        prevButton->setFocusPolicy(Qt::NoFocus);
        connect(prevButton, &QPushButton::clicked, this, &BioDeviceView::pageUp);
        QRect prevBtnRect(devicesList->geometry().right(),
                          devicesList->geometry().bottom() - ARROW_SIZE,
                          ARROW_SIZE, ARROW_SIZE);
        prevButton->setGeometry(prevBtnRect);

        nextButton = new QPushButton(this);
        nextButton->setObjectName(QStringLiteral("bioNextButton"));
        nextButton->setFocusPolicy(Qt::NoFocus);
        connect(nextButton, &QPushButton::clicked, this, &BioDeviceView::pageDown);
        QRect nextBtnRect(prevBtnRect.right(), prevBtnRect.top(),
                          ARROW_SIZE, ARROW_SIZE);
        nextButton->setGeometry(nextBtnRect);
    }

    resize(BIODEVICEVIEW_WIDTH, BIODEVICEVIEW_HEIGHT);
}

void BioDeviceView::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Up:
        setCurrentRow(currentIndex-1);
        break;
    case Qt::Key_Down:
        setCurrentRow(currentIndex+1);
        break;
    case Qt::Key_PageUp:
        pageUp();
        break;
    case Qt::Key_PageDown:
        pageDown();
        break;
    case Qt::Key_Return:
        onDeviceIconClicked(currentIndex);
        break;
    }
    return QWidget::keyReleaseEvent(event);
}
void BioDeviceView::focusInEvent(QFocusEvent *event)
{
    QListWidgetItem *item = devicesList->item(currentIndex);
    devicesList->itemWidget(item)->setFocus();
    return QWidget::focusInEvent(event);
}
void BioDeviceView::showEvent(QShowEvent *event)
{
    this->setFocus();
    return QWidget::showEvent(event);
}

bool BioDeviceView::eventFilter(QObject *obj, QEvent *event)
{
    QString objName = obj->objectName();
    if(objName.left(12) == "bioIconLabel"){
        int index = objName.right(objName.size() - 12).toInt();
        if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *e = static_cast<QMouseEvent*>(event);
            if(e->button() == Qt::LeftButton){
                setCurrentRow(index);
                onDeviceIconClicked(index);
                return true;
            }
        } else if(event->type() == QEvent::Enter){

            setPromptText(index);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void BioDeviceView::setCurrentRow(int row)
{
    if(row >= devicesList->count() || row < 0)
        return;
    currentIndex = row;
//    QListWidgetItem *item = devicesList->item(currentIndex);
    devicesList->setCurrentRow(currentIndex);
    setPromptText(currentIndex);
}

void BioDeviceView::setPromptText(int index)
{
    if(index == 0)
        promptLabel->setText(tr("password login"));
    else{
        DeviceInfo deviceInfo = deviceInfos.at(index-1);
        QString deviceType;
        switch(deviceInfo.biotype){
        case BIOTYPE_FINGERPRINT:
            deviceType = tr("fingerprint");
            break;
        case BIOTYPE_FINGERVEIN:
            deviceType = tr("fingerevin");
            break;
        case BIOTYPE_IRIS:
            deviceType = tr("iris");
            break;
        }
        promptLabel->setText(deviceType + ": " + deviceInfo.device_fullname);
    }
}

void BioDeviceView::onDeviceIconClicked(int index)
{
    if(index > deviceInfos.size())
        return;

    //每次认证时都会停止之前的认证
    if(authControl){
        authControl->stopAuthentication();
        delete authControl;
        authControl = nullptr;
    }

    if(index == 0){
        qDebug() << "密码登录";
        Q_EMIT backToPasswd();
        return;
    }
    if(index > deviceCount){
        qDebug() << "test device";
        return;
    }

    DeviceInfo deviceInfo = deviceInfos.at(index-1);

    authControl = new BioAuthentication(uid, deviceInfo, this);
    authControl->startAuthentication();
    connect(authControl, &BioAuthentication::authenticationComplete,
            this, &BioDeviceView::authenticationComplete);
    connect(authControl, &BioAuthentication::notify, this, &BioDeviceView::notify);
}

void BioDeviceView::pageUp()
{
    if(devicesList->currentRow() >= MAX_NUM)
        setCurrentRow(currentIndex - MAX_NUM);
    else
        setCurrentRow(0);
}

void BioDeviceView::pageDown()
{
    if(devicesList->count() - devicesList->currentRow() >= MAX_NUM)
        setCurrentRow(currentIndex + MAX_NUM);
    else
        setCurrentRow(devicesList->count()-1);
}

#ifdef TEST
void BioDeviceView::addTestDevices()
{
    for(int i = 0; i < 2; i++){
        DeviceInfo info;
        info.biotype = BIOTYPE_FINGERPRINT;
        info.driver_enable = 1;
        info.device_available = 1;
        deviceInfos.push_back(info);
    }
    for(int i = 0; i < 2; i++){
        DeviceInfo info;
        info.biotype = BIOTYPE_IRIS;
        info.driver_enable = 1;
        info.device_available = 1;
        deviceInfos.push_back(info);
    }
    deviceCount += 4;
}

#endif

