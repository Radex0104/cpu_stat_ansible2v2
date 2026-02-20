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
    QByteArrayData data[24];
    char stringdata0[391];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WSLChecker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WSLChecker_t qt_meta_stringdata_WSLChecker = {
    {
QT_MOC_LITERAL(0, 0, 10), // "WSLChecker"
QT_MOC_LITERAL(1, 11, 16), // "wslSetupFinished"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 7), // "success"
QT_MOC_LITERAL(4, 37, 21), // "distroInstallFinished"
QT_MOC_LITERAL(5, 59, 22), // "ansibleInstallFinished"
QT_MOC_LITERAL(6, 82, 21), // "ansibleOutputReceived"
QT_MOC_LITERAL(7, 104, 6), // "output"
QT_MOC_LITERAL(8, 111, 18), // "ansibleInfoUpdated"
QT_MOC_LITERAL(9, 130, 9), // "installed"
QT_MOC_LITERAL(10, 140, 7), // "version"
QT_MOC_LITERAL(11, 148, 24), // "onInstallProcessFinished"
QT_MOC_LITERAL(12, 173, 8), // "exitCode"
QT_MOC_LITERAL(13, 182, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(14, 203, 10), // "exitStatus"
QT_MOC_LITERAL(15, 214, 23), // "onDistroInstallFinished"
QT_MOC_LITERAL(16, 238, 14), // "onDistroOutput"
QT_MOC_LITERAL(17, 253, 13), // "onDistroError"
QT_MOC_LITERAL(18, 267, 24), // "onAnsibleInstallFinished"
QT_MOC_LITERAL(19, 292, 15), // "onAnsibleOutput"
QT_MOC_LITERAL(20, 308, 14), // "onAnsibleError"
QT_MOC_LITERAL(21, 323, 22), // "onVersionCheckFinished"
QT_MOC_LITERAL(22, 346, 22), // "onAnsibleInstallOutput"
QT_MOC_LITERAL(23, 369, 21) // "onAnsibleInstallError"

    },
    "WSLChecker\0wslSetupFinished\0\0success\0"
    "distroInstallFinished\0ansibleInstallFinished\0"
    "ansibleOutputReceived\0output\0"
    "ansibleInfoUpdated\0installed\0version\0"
    "onInstallProcessFinished\0exitCode\0"
    "QProcess::ExitStatus\0exitStatus\0"
    "onDistroInstallFinished\0onDistroOutput\0"
    "onDistroError\0onAnsibleInstallFinished\0"
    "onAnsibleOutput\0onAnsibleError\0"
    "onVersionCheckFinished\0onAnsibleInstallOutput\0"
    "onAnsibleInstallError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WSLChecker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06 /* Public */,
       4,    1,   92,    2, 0x06 /* Public */,
       5,    1,   95,    2, 0x06 /* Public */,
       6,    1,   98,    2, 0x06 /* Public */,
       8,    2,  101,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    2,  106,    2, 0x08 /* Private */,
      15,    2,  111,    2, 0x08 /* Private */,
      16,    0,  116,    2, 0x08 /* Private */,
      17,    0,  117,    2, 0x08 /* Private */,
      18,    2,  118,    2, 0x08 /* Private */,
      19,    0,  123,    2, 0x08 /* Private */,
      20,    0,  124,    2, 0x08 /* Private */,
      21,    2,  125,    2, 0x08 /* Private */,
      22,    0,  130,    2, 0x08 /* Private */,
      23,    0,  131,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    9,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 13,   12,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void WSLChecker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WSLChecker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wslSetupFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->distroInstallFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->ansibleInstallFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->ansibleOutputReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->ansibleInfoUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->onInstallProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 6: _t->onDistroInstallFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 7: _t->onDistroOutput(); break;
        case 8: _t->onDistroError(); break;
        case 9: _t->onAnsibleInstallFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 10: _t->onAnsibleOutput(); break;
        case 11: _t->onAnsibleError(); break;
        case 12: _t->onVersionCheckFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 13: _t->onAnsibleInstallOutput(); break;
        case 14: _t->onAnsibleInstallError(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WSLChecker::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::wslSetupFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WSLChecker::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::distroInstallFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WSLChecker::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::ansibleInstallFinished)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WSLChecker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::ansibleOutputReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WSLChecker::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WSLChecker::ansibleInfoUpdated)) {
                *result = 4;
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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void WSLChecker::wslSetupFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WSLChecker::distroInstallFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WSLChecker::ansibleInstallFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WSLChecker::ansibleOutputReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WSLChecker::ansibleInfoUpdated(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
