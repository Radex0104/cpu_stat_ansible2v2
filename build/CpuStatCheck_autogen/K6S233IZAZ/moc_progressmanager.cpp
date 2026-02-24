/****************************************************************************
** Meta object code from reading C++ file 'progressmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../headers/progressmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'progressmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProgressManager_t {
    QByteArrayData data[25];
    char stringdata0[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgressManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgressManager_t qt_meta_stringdata_ProgressManager = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ProgressManager"
QT_MOC_LITERAL(1, 16, 15), // "progressChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 5), // "value"
QT_MOC_LITERAL(4, 39, 13), // "statusChanged"
QT_MOC_LITERAL(5, 53, 4), // "text"
QT_MOC_LITERAL(6, 58, 19), // "runningStateChanged"
QT_MOC_LITERAL(7, 78, 7), // "running"
QT_MOC_LITERAL(8, 86, 17), // "progressCompleted"
QT_MOC_LITERAL(9, 104, 7), // "success"
QT_MOC_LITERAL(10, 112, 11), // "stepReached"
QT_MOC_LITERAL(11, 124, 4), // "step"
QT_MOC_LITERAL(12, 129, 11), // "description"
QT_MOC_LITERAL(13, 141, 13), // "startProgress"
QT_MOC_LITERAL(14, 155, 8), // "maxSteps"
QT_MOC_LITERAL(15, 164, 12), // "stopProgress"
QT_MOC_LITERAL(16, 177, 14), // "updateProgress"
QT_MOC_LITERAL(17, 192, 15), // "stepDescription"
QT_MOC_LITERAL(18, 208, 17), // "incrementProgress"
QT_MOC_LITERAL(19, 226, 13), // "setStatusText"
QT_MOC_LITERAL(20, 240, 5), // "reset"
QT_MOC_LITERAL(21, 246, 20), // "onIndeterminateTimer"
QT_MOC_LITERAL(22, 267, 13), // "progressValue"
QT_MOC_LITERAL(23, 281, 10), // "statusText"
QT_MOC_LITERAL(24, 292, 9) // "isRunning"

    },
    "ProgressManager\0progressChanged\0\0value\0"
    "statusChanged\0text\0runningStateChanged\0"
    "running\0progressCompleted\0success\0"
    "stepReached\0step\0description\0startProgress\0"
    "maxSteps\0stopProgress\0updateProgress\0"
    "stepDescription\0incrementProgress\0"
    "setStatusText\0reset\0onIndeterminateTimer\0"
    "progressValue\0statusText\0isRunning"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgressManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       3,  136, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       4,    1,   97,    2, 0x06 /* Public */,
       6,    1,  100,    2, 0x06 /* Public */,
       8,    1,  103,    2, 0x06 /* Public */,
      10,    2,  106,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    1,  111,    2, 0x0a /* Public */,
      13,    0,  114,    2, 0x2a /* Public | MethodCloned */,
      15,    1,  115,    2, 0x0a /* Public */,
      15,    0,  118,    2, 0x2a /* Public | MethodCloned */,
      16,    2,  119,    2, 0x0a /* Public */,
      16,    1,  124,    2, 0x2a /* Public | MethodCloned */,
      18,    1,  127,    2, 0x0a /* Public */,
      18,    0,  130,    2, 0x2a /* Public | MethodCloned */,
      19,    1,  131,    2, 0x0a /* Public */,
      20,    0,  134,    2, 0x0a /* Public */,
      21,    0,  135,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   12,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   17,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      22, QMetaType::Int, 0x00495001,
      23, QMetaType::QString, 0x00495001,
      24, QMetaType::Bool, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void ProgressManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProgressManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->progressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->statusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->runningStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->progressCompleted((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->stepReached((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->startProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->startProgress(); break;
        case 7: _t->stopProgress((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->stopProgress(); break;
        case 9: _t->updateProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 10: _t->updateProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->incrementProgress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->incrementProgress(); break;
        case 13: _t->setStatusText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->reset(); break;
        case 15: _t->onIndeterminateTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProgressManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressManager::progressChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProgressManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressManager::statusChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProgressManager::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressManager::runningStateChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProgressManager::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressManager::progressCompleted)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProgressManager::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressManager::stepReached)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<ProgressManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->progressValue(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->statusText(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isRunning(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ProgressManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_ProgressManager.data,
    qt_meta_data_ProgressManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProgressManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgressManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProgressManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProgressManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ProgressManager::progressChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProgressManager::statusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProgressManager::runningStateChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProgressManager::progressCompleted(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ProgressManager::stepReached(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
