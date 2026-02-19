/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../headers/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[18];
    char stringdata0[225];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 16), // "onAddHostClicked"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 10), // "removeHost"
QT_MOC_LITERAL(4, 40, 19), // "onPlayButtonClicked"
QT_MOC_LITERAL(5, 60, 15), // "onAnsibleOutput"
QT_MOC_LITERAL(6, 76, 4), // "text"
QT_MOC_LITERAL(7, 81, 17), // "onAnsibleFinished"
QT_MOC_LITERAL(8, 99, 7), // "success"
QT_MOC_LITERAL(9, 107, 8), // "exitCode"
QT_MOC_LITERAL(10, 116, 14), // "onAnsibleError"
QT_MOC_LITERAL(11, 131, 7), // "message"
QT_MOC_LITERAL(12, 139, 19), // "onWslCheckCompleted"
QT_MOC_LITERAL(13, 159, 19), // "WSLChecker::WSLInfo"
QT_MOC_LITERAL(14, 179, 4), // "info"
QT_MOC_LITERAL(15, 184, 15), // "onWslCheckError"
QT_MOC_LITERAL(16, 200, 5), // "error"
QT_MOC_LITERAL(17, 206, 18) // "onWslSetupFinished"

    },
    "MainWindow\0onAddHostClicked\0\0removeHost\0"
    "onPlayButtonClicked\0onAnsibleOutput\0"
    "text\0onAnsibleFinished\0success\0exitCode\0"
    "onAnsibleError\0message\0onWslCheckCompleted\0"
    "WSLChecker::WSLInfo\0info\0onWslCheckError\0"
    "error\0onWslSetupFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    1,   62,    2, 0x08 /* Private */,
       7,    2,   65,    2, 0x08 /* Private */,
      10,    1,   70,    2, 0x08 /* Private */,
      12,    1,   73,    2, 0x08 /* Private */,
      15,    1,   76,    2, 0x08 /* Private */,
      17,    1,   79,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    8,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::Bool,    8,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onAddHostClicked(); break;
        case 1: _t->removeHost(); break;
        case 2: _t->onPlayButtonClicked(); break;
        case 3: _t->onAnsibleOutput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onAnsibleFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->onAnsibleError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onWslCheckCompleted((*reinterpret_cast< const WSLChecker::WSLInfo(*)>(_a[1]))); break;
        case 7: _t->onWslCheckError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onWslSetupFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
