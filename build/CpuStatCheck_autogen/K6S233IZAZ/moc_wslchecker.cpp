/****************************************************************************
** Meta object code from reading C++ file 'wslchecker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../headers/wslchecker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wslchecker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WSLChecker_t {
    QByteArrayData data[11];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WSLChecker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WSLChecker_t qt_meta_stringdata_WSLChecker = {
    {
QT_MOC_LITERAL(0, 0, 10), // "WSLChecker"
QT_MOC_LITERAL(1, 11, 17), // "wslCheckCompleted"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 19), // "WSLChecker::WSLInfo"
QT_MOC_LITERAL(4, 50, 4), // "info"
QT_MOC_LITERAL(5, 55, 16), // "wslSetupFinished"
QT_MOC_LITERAL(6, 72, 7), // "success"
QT_MOC_LITERAL(7, 80, 24), // "onInstallProcessFinished"
QT_MOC_LITERAL(8, 105, 8), // "exitCode"
QT_MOC_LITERAL(9, 114, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(10, 135, 10) // "exitStatus"

    },
    "WSLChecker\0wslCheckCompleted\0\0"
    "WSLChecker::WSLInfo\0info\0wslSetupFinished\0"
    "success\0onInstallProcessFinished\0"
    "exitCode\0QProcess::ExitStatus\0exitStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WSLChecker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 9,    8,   10,

       0        // eod
};

void WSLChecker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WSLChecker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wslCheckCompleted((*reinterpret_cast< const WSLChecker::WSLInfo(*)>(_a[1]))); break;
        case 1: _t->wslSetupFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->onInstallProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WSLChecker::*)(const WSLChecker::WSLInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::wslCheckCompleted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WSLChecker::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::wslSetupFinished)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WSLChecker::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_WSLChecker.data,
    qt_meta_data_WSLChecker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WSLChecker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WSLChecker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WSLChecker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WSLChecker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void WSLChecker::wslCheckCompleted(const WSLChecker::WSLInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WSLChecker::wslSetupFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
