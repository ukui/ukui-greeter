/* biodeviceview.h
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
#ifndef BIODEVICEVIEW_H
#define BIODEVICEVIEW_H

#include <QWidget>
#include <QList>
#include <QVector>
#include "biocustomtype.h"
#include "biodevices.h"

//#define TEST 1

#define ICON_SIZE 32
#define ITEM_SIZE (ICON_SIZE + 4)
#define ARROW_SIZE 24
#define MAX_NUM 4
#define LISTWIDGET_WIDTH (ITEM_SIZE * MAX_NUM * 2)
#define LISTWIDGET_HEIGHT ITEM_SIZE
#define BIODEVICEVIEW_WIDTH (LISTWIDGET_WIDTH + ARROW_SIZE * 2)
#define BIODEVICEVIEW_HEIGHT (ITEM_SIZE + 40)

class QDBusInterface;
class QTableWidget;
class QLabel;
class QListWidget;
class QPushButton;
class BioAuthentication;

typedef QVector<QString> QStringVector;

class BioDeviceView : public QWidget
{
    Q_OBJECT

public:
    explicit BioDeviceView(qint32 uid=0, QWidget *parent=nullptr);
    void initUI();
    void pageUp();
    void pageDown();
#ifdef TEST
    void addTestDevices();
#endif

protected:
    void keyReleaseEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void showEvent(QShowEvent *event);
    void focusInEvent(QFocusEvent *event);
    void setCurrentRow(int row);
    void setPromptText(int index);

private slots:
    void onDeviceIconClicked(int index);

signals:
    void backToPasswd();
    void authenticationComplete(bool);
    void notify(const QString& message);

private:

    QListWidget         *devicesList;
    QLabel              *promptLabel;
    QLabel              *notifyLabel;
    QPushButton         *prevButton;
    QPushButton         *nextButton;

    QStringVector       deviceTypes;
    QList<DeviceInfo>   deviceInfos;
    qint32              uid;
    int                 deviceCount;
    int                 currentIndex;
    BioAuthentication   *authControl;
};

#endif // BIODEVICEVIEW_H
