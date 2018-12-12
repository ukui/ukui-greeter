/****************************************************************************
** Meta object code from reading C++ file 'biometricproxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../biometricproxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'biometricproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DeviceType_t {
    QByteArrayData data[8];
    char stringdata0[75];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DeviceType_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DeviceType_t qt_meta_stringdata_DeviceType = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DeviceType"
QT_MOC_LITERAL(1, 11, 4), // "Type"
QT_MOC_LITERAL(2, 16, 11), // "FingerPrint"
QT_MOC_LITERAL(3, 28, 10), // "FingerVein"
QT_MOC_LITERAL(4, 39, 4), // "Iris"
QT_MOC_LITERAL(5, 44, 4), // "Face"
QT_MOC_LITERAL(6, 49, 10), // "VoicePrint"
QT_MOC_LITERAL(7, 60, 14) // "__MAX_NR_TYPES"

    },
    "DeviceType\0Type\0FingerPrint\0FingerVein\0"
    "Iris\0Face\0VoicePrint\0__MAX_NR_TYPES"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DeviceType[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
       1, 0x0,    6,   18,

 // enum data: key, value
       2, uint(DeviceType::FingerPrint),
       3, uint(DeviceType::FingerVein),
       4, uint(DeviceType::Iris),
       5, uint(DeviceType::Face),
       6, uint(DeviceType::VoicePrint),
       7, uint(DeviceType::__MAX_NR_TYPES),

       0        // eod
};

void DeviceType::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject DeviceType::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DeviceType.data,
      qt_meta_data_DeviceType,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DeviceType::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeviceType::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceType.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DeviceType::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_BiometricProxy_t {
    QByteArrayData data[22];
    char stringdata0[227];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BiometricProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BiometricProxy_t qt_meta_stringdata_BiometricProxy = {
    {
QT_MOC_LITERAL(0, 0, 14), // "BiometricProxy"
QT_MOC_LITERAL(1, 15, 13), // "StatusChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 5), // "drvid"
QT_MOC_LITERAL(4, 36, 6), // "status"
QT_MOC_LITERAL(5, 43, 16), // "USBDeviceHotPlug"
QT_MOC_LITERAL(6, 60, 6), // "action"
QT_MOC_LITERAL(7, 67, 9), // "deviceNum"
QT_MOC_LITERAL(8, 77, 8), // "Identify"
QT_MOC_LITERAL(9, 86, 16), // "QDBusPendingCall"
QT_MOC_LITERAL(10, 103, 3), // "uid"
QT_MOC_LITERAL(11, 107, 10), // "indexStart"
QT_MOC_LITERAL(12, 118, 8), // "indexEnd"
QT_MOC_LITERAL(13, 127, 7), // "StopOps"
QT_MOC_LITERAL(14, 135, 7), // "waiting"
QT_MOC_LITERAL(15, 143, 10), // "GetDevList"
QT_MOC_LITERAL(16, 154, 10), // "DeviceList"
QT_MOC_LITERAL(17, 165, 10), // "GetDevMesg"
QT_MOC_LITERAL(18, 176, 13), // "GetNotifyMesg"
QT_MOC_LITERAL(19, 190, 10), // "GetOpsMesg"
QT_MOC_LITERAL(20, 201, 12), // "UpdateStatus"
QT_MOC_LITERAL(21, 214, 12) // "StatusReslut"

    },
    "BiometricProxy\0StatusChanged\0\0drvid\0"
    "status\0USBDeviceHotPlug\0action\0deviceNum\0"
    "Identify\0QDBusPendingCall\0uid\0indexStart\0"
    "indexEnd\0StopOps\0waiting\0GetDevList\0"
    "DeviceList\0GetDevMesg\0GetNotifyMesg\0"
    "GetOpsMesg\0UpdateStatus\0StatusReslut"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BiometricProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   74,    2, 0x06 /* Public */,
       5,    3,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    4,   86,    2, 0x0a /* Public */,
       8,    3,   95,    2, 0x2a /* Public | MethodCloned */,
       8,    2,  102,    2, 0x2a /* Public | MethodCloned */,
      13,    2,  107,    2, 0x0a /* Public */,
      13,    1,  112,    2, 0x2a /* Public | MethodCloned */,
      15,    0,  115,    2, 0x0a /* Public */,
      17,    1,  116,    2, 0x0a /* Public */,
      18,    1,  119,    2, 0x0a /* Public */,
      19,    1,  122,    2, 0x0a /* Public */,
      20,    1,  125,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    6,    7,

 // slots: parameters
    0x80000000 | 9, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,   10,   11,   12,
    0x80000000 | 9, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,   10,   11,
    0x80000000 | 9, QMetaType::Int, QMetaType::Int,    3,   10,
    QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,   14,
    QMetaType::Int, QMetaType::Int,    3,
    0x80000000 | 16,
    QMetaType::QString, QMetaType::Int,    3,
    QMetaType::QString, QMetaType::Int,    3,
    QMetaType::QString, QMetaType::Int,    3,
    0x80000000 | 21, QMetaType::Int,    3,

       0        // eod
};

void BiometricProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BiometricProxy *_t = static_cast<BiometricProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->StatusChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->USBDeviceHotPlug((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: { QDBusPendingCall _r = _t->Identify((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QDBusPendingCall*>(_a[0]) = std::move(_r); }  break;
        case 3: { QDBusPendingCall _r = _t->Identify((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QDBusPendingCall*>(_a[0]) = std::move(_r); }  break;
        case 4: { QDBusPendingCall _r = _t->Identify((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusPendingCall*>(_a[0]) = std::move(_r); }  break;
        case 5: { int _r = _t->StopOps((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 6: { int _r = _t->StopOps((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 7: { DeviceList _r = _t->GetDevList();
            if (_a[0]) *reinterpret_cast< DeviceList*>(_a[0]) = std::move(_r); }  break;
        case 8: { QString _r = _t->GetDevMesg((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 9: { QString _r = _t->GetNotifyMesg((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 10: { QString _r = _t->GetOpsMesg((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 11: { StatusReslut _r = _t->UpdateStatus((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< StatusReslut*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BiometricProxy::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BiometricProxy::StatusChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BiometricProxy::*)(int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BiometricProxy::USBDeviceHotPlug)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BiometricProxy::staticMetaObject = {
    { &QDBusAbstractInterface::staticMetaObject, qt_meta_stringdata_BiometricProxy.data,
      qt_meta_data_BiometricProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BiometricProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BiometricProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BiometricProxy.stringdata0))
        return static_cast<void*>(this);
    return QDBusAbstractInterface::qt_metacast(_clname);
}

int BiometricProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void BiometricProxy::StatusChanged(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BiometricProxy::USBDeviceHotPlug(int _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
