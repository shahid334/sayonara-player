/****************************************************************************
** Meta object code from reading C++ file 'GUISocketSetup.h'
**
** Created: Mon Sep 3 16:37:12 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GUISocketSetup.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GUISocketSetup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI_SocketSetup[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      40,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUI_SocketSetup[] = {
    "GUI_SocketSetup\0\0_sl_start_changed(int)\0"
    "_sl_ok_pressed()\0"
};

void GUI_SocketSetup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GUI_SocketSetup *_t = static_cast<GUI_SocketSetup *>(_o);
        switch (_id) {
        case 0: _t->_sl_start_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->_sl_ok_pressed(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUI_SocketSetup::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUI_SocketSetup::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GUI_SocketSetup,
      qt_meta_data_GUI_SocketSetup, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI_SocketSetup::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI_SocketSetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI_SocketSetup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI_SocketSetup))
        return static_cast<void*>(const_cast< GUI_SocketSetup*>(this));
    return QDialog::qt_metacast(_clname);
}

int GUI_SocketSetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
