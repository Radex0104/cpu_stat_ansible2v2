/****************************************************************************
** Meta object code from reading C++ file 'ansiblerunner.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../headers/ansiblerunner.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ansiblerunner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AnsibleRunner_t {
    QByteArrayData data[20];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AnsibleRunner_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AnsibleRunner_t qt_meta_stringdata_AnsibleRunner = {
    {
QT_MOC_LITERAL(0, 0, 13), // "AnsibleRunner"
QT_MOC_LITERAL(1, 14, 14), // "outputReceived"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "text"
QT_MOC_LITERAL(4, 35, 13), // "errorOccurred"
QT_MOC_LITERAL(5, 49, 5), // "error"
QT_MOC_LITERAL(6, 55, 8), // "finished"
QT_MOC_LITERAL(7, 64, 7), // "success"
QT_MOC_LITERAL(8, 72, 8), // "exitCode"
QT_MOC_LITERAL(9, 81, 15), // "progressUpdated"
QT_MOC_LITERAL(10, 97, 7), // "percent"
QT_MOC_LITERAL(11, 105, 8), // "taskName"
QT_MOC_LITERAL(12, 114, 11), // "taskStarted"
QT_MOC_LITERAL(13, 126, 13), // "taskCompleted"
QT_MOC_LITERAL(14, 140, 17), // "onProcessFinished"
QT_MOC_LITERAL(15, 158, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(16, 179, 6), // "status"
QT_MOC_LITERAL(17, 186, 22), // "onProcessErrorOccurred"
QT_MOC_LITERAL(18, 209, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(19, 232, 17) // "readProcessOutput"

    },
    "AnsibleRunner\0outputReceived\0\0text\0"
    "errorOccurred\0error\0finished\0success\0"
    "exitCode\0progressUpdated\0percent\0"
    "taskName\0taskStarted\0taskCompleted\0"
    "onProcessFinished\0QProcess::ExitStatus\0"
    "status\0onProcessErrorOccurred\0"
    "QProcess::ProcessError\0readProcessOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AnsibleRunner[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       6,    2,   65,    2, 0x06 /* Public */,
       9,    2,   70,    2, 0x06 /* Public */,
      12,    1,   75,    2, 0x06 /* Public */,
      13,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    2,   81,    2, 0x08 /* Private */,
      17,    1,   86,    2, 0x08 /* Private */,
      19,    0,   89,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   10,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 15,    8,   16,
    QMetaType::Void, 0x80000000 | 18,    5,
    QMetaType::Void,

       0        // eod
};

void AnsibleRunner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AnsibleRunner *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->outputReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->finished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->progressUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->taskStarted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->taskCompleted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 7: _t->onProcessErrorOccurred((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 8: _t->readProcessOutput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AnsibleRunner::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnsibleRunner::outputReceived)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AnsibleRunner::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnsibleRunner::errorOccurred)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AnsibleRunner::*)(bool , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnsibleRunner::finished)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AnsibleRunner::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnsibleRunner::progressUpdated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AnsibleRunner::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnsibleRunner::taskStarted)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (AnsibleRunner::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnsibleRunner::taskCompleted)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AnsibleRunner::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_AnsibleRunner.data,
    qt_meta_data_AnsibleRunner,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AnsibleRunner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AnsibleRunner::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AnsibleRunner.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AnsibleRunner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void AnsibleRunner::outputReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AnsibleRunner::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AnsibleRunner::finished(bool _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AnsibleRunner::progressUpdated(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void AnsibleRunner::taskStarted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void AnsibleRunner::taskCompleted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
