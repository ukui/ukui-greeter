/****************************************************************************
** Meta object code from reading C++ file 'biometricdeviceswidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../biometricdeviceswidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'biometricdeviceswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BiometricDevicesWidget_t {
    QByteArrayData data[14];
    char stringdata0[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BiometricDevicesWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BiometricDevicesWidget_t qt_meta_stringdata_BiometricDevicesWidget = {
    {
QT_MOC_LITERAL(0, 0, 22), // "BiometricDevicesWidget"
QT_MOC_LITERAL(1, 23, 13), // "deviceChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 13), // "DeviceInfoPtr"
QT_MOC_LITERAL(4, 52, 11), // "pDeviceInfo"
QT_MOC_LITERAL(5, 64, 22), // "deviceTypeCountChanged"
QT_MOC_LITERAL(6, 87, 12), // "newTypeCount"
QT_MOC_LITERAL(7, 100, 22), // "onBtnDeviceTypeClicked"
QT_MOC_LITERAL(8, 123, 34), // "onCmbDeviceNameCurrentIndexCh..."
QT_MOC_LITERAL(9, 158, 5), // "index"
QT_MOC_LITERAL(10, 164, 18), // "onUSBDeviceHotPlug"
QT_MOC_LITERAL(11, 183, 5), // "drvid"
QT_MOC_LITERAL(12, 189, 6), // "action"
QT_MOC_LITERAL(13, 196, 6) // "devNum"

    },
    "BiometricDevicesWidget\0deviceChanged\0"
    "\0DeviceInfoPtr\0pDeviceInfo\0"
    "deviceTypeCountChanged\0newTypeCount\0"
    "onBtnDeviceTypeClicked\0"
    "onCmbDeviceNameCurrentIndexChanged\0"
    "index\0onUSBDeviceHotPlug\0drvid\0action\0"
    "devNum"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BiometricDevicesWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   45,    2, 0x08 /* Private */,
       8,    1,   46,    2, 0x08 /* Private */,
      10,    3,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   11,   12,   13,

       0        // eod
};

void BiometricDevicesWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BiometricDevicesWidget *_t = static_cast<BiometricDevicesWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deviceChanged((*reinterpret_cast< const DeviceInfoPtr(*)>(_a[1]))); break;
        case 1: _t->deviceTypeCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onBtnDeviceTypeClicked(); break;
        case 3: _t->onCmbDeviceNameCurrentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onUSBDeviceHotPlug((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BiometricDevicesWidget::*)(const DeviceInfoPtr & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BiometricDevicesWidget::deviceChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BiometricDevicesWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BiometricDevicesWidget::deviceTypeCountChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BiometricDevicesWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BiometricDevicesWidget.data,
      qt_meta_data_BiometricDevicesWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BiometricDevicesWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BiometricDevicesWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BiometricDevicesWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BiometricDevicesWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void BiometricDevicesWidget::deviceChanged(const DeviceInfoPtr & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BiometricDevicesWidget::deviceTypeCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
