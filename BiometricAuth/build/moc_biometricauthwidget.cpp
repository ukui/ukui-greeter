/****************************************************************************
** Meta object code from reading C++ file 'biometricauthwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../biometricauthwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'biometricauthwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BiometricAuthWidget_t {
    QByteArrayData data[11];
    char stringdata0[142];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BiometricAuthWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BiometricAuthWidget_t qt_meta_stringdata_BiometricAuthWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "BiometricAuthWidget"
QT_MOC_LITERAL(1, 20, 12), // "authComplete"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 6), // "result"
QT_MOC_LITERAL(4, 41, 18), // "onIdentifyComplete"
QT_MOC_LITERAL(5, 60, 24), // "QDBusPendingCallWatcher*"
QT_MOC_LITERAL(6, 85, 7), // "watcher"
QT_MOC_LITERAL(7, 93, 15), // "onStatusChanged"
QT_MOC_LITERAL(8, 109, 5), // "drvid"
QT_MOC_LITERAL(9, 115, 6), // "status"
QT_MOC_LITERAL(10, 122, 19) // "onMoviePixmapUpdate"

    },
    "BiometricAuthWidget\0authComplete\0\0"
    "result\0onIdentifyComplete\0"
    "QDBusPendingCallWatcher*\0watcher\0"
    "onStatusChanged\0drvid\0status\0"
    "onMoviePixmapUpdate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BiometricAuthWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   37,    2, 0x08 /* Private */,
       7,    2,   40,    2, 0x08 /* Private */,
      10,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
    QMetaType::Void,

       0        // eod
};

void BiometricAuthWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BiometricAuthWidget *_t = static_cast<BiometricAuthWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->authComplete((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->onIdentifyComplete((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 2: _t->onStatusChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->onMoviePixmapUpdate(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QDBusPendingCallWatcher* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BiometricAuthWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BiometricAuthWidget::authComplete)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BiometricAuthWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BiometricAuthWidget.data,
      qt_meta_data_BiometricAuthWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BiometricAuthWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BiometricAuthWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BiometricAuthWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BiometricAuthWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void BiometricAuthWidget::authComplete(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
