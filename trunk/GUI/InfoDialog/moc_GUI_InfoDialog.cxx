/****************************************************************************
** Meta object code from reading C++ file 'GUI_InfoDialog.h'
**
** Created: Mon Jul 23 19:31:46 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GUI_InfoDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GUI_InfoDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI_InfoDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      43,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      76,   15,   15,   15, 0x0a,
     118,  105,   15,   15, 0x0a,
     152,  105,   15,   15, 0x0a,
     187,  105,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI_InfoDialog[] = {
    "GUI_InfoDialog\0\0sig_search_cover(MetaData)\0"
    "sig_search_artist_image(QString)\0"
    "psl_image_available(QString)\0target_class\0"
    "psl_album_info_available(QString)\0"
    "psl_artist_info_available(QString)\0"
    "psl_corrected_data_available(QString)\0"
};

const QMetaObject GUI_InfoDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GUI_InfoDialog,
      qt_meta_data_GUI_InfoDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI_InfoDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI_InfoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI_InfoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI_InfoDialog))
        return static_cast<void*>(const_cast< GUI_InfoDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int GUI_InfoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sig_search_cover((*reinterpret_cast< const MetaData(*)>(_a[1]))); break;
        case 1: sig_search_artist_image((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: psl_image_available((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: psl_album_info_available((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: psl_artist_info_available((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: psl_corrected_data_available((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GUI_InfoDialog::sig_search_cover(const MetaData & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GUI_InfoDialog::sig_search_artist_image(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
