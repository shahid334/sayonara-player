/****************************************************************************
** Meta object code from reading C++ file 'GUI_Equalizer.h'
**
** Created: Thu May 19 11:32:40 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GUI/equalizer/GUI_Equalizer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GUI_Equalizer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI_Equalizer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   14,   14,   14, 0x08,
      63,   14,   14,   14, 0x08,
      82,   14,   14,   14, 0x08,
     101,   14,   14,   14, 0x08,
     120,   14,   14,   14, 0x08,
     139,   14,   14,   14, 0x08,
     158,   14,   14,   14, 0x08,
     177,   14,   14,   14, 0x08,
     196,   14,   14,   14, 0x08,
     215,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUI_Equalizer[] = {
    "GUI_Equalizer\0\0,\0eq_changed_signal(int,int)\0"
    "sli_0_changed(int)\0sli_1_changed(int)\0"
    "sli_2_changed(int)\0sli_3_changed(int)\0"
    "sli_4_changed(int)\0sli_5_changed(int)\0"
    "sli_6_changed(int)\0sli_7_changed(int)\0"
    "sli_8_changed(int)\0sli_9_changed(int)\0"
};

const QMetaObject GUI_Equalizer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GUI_Equalizer,
      qt_meta_data_GUI_Equalizer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI_Equalizer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI_Equalizer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI_Equalizer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI_Equalizer))
        return static_cast<void*>(const_cast< GUI_Equalizer*>(this));
    return QWidget::qt_metacast(_clname);
}

int GUI_Equalizer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: eq_changed_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: sli_0_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: sli_1_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: sli_2_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: sli_3_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: sli_4_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: sli_5_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: sli_6_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: sli_7_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: sli_8_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: sli_9_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void GUI_Equalizer::eq_changed_signal(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
