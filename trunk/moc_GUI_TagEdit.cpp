/****************************************************************************
** Meta object code from reading C++ file 'GUI_TagEdit.h'
**
** Created: Wed May 25 14:11:45 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GUI/tagedit/GUI_TagEdit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GUI_TagEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUI_TagEdit[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      32,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      68,   12,   12,   12, 0x08,
      90,   12,   12,   12, 0x08,
     112,   12,   12,   12, 0x08,
     132,   12,   12,   12, 0x08,
     161,  156,   12,   12, 0x08,
     184,  156,   12,   12, 0x08,
     208,   12,   12,   12, 0x08,
     229,   12,   12,   12, 0x08,
     251,   12,   12,   12, 0x08,
     271,   12,   12,   12, 0x08,
     290,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GUI_TagEdit[] = {
    "GUI_TagEdit\0\0id3_tags_changed()\0"
    "id3_tags_changed(vector<MetaData>&)\0"
    "prev_button_clicked()\0next_button_clicked()\0"
    "ok_button_clicked()\0cancel_button_clicked()\0"
    "text\0album_changed(QString)\0"
    "artist_changed(QString)\0all_albums_clicked()\0"
    "all_artists_clicked()\0all_genre_clicked()\0"
    "all_year_clicked()\0"
    "change_meta_data(vector<MetaData>)\0"
};

const QMetaObject GUI_TagEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GUI_TagEdit,
      qt_meta_data_GUI_TagEdit, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUI_TagEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUI_TagEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUI_TagEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUI_TagEdit))
        return static_cast<void*>(const_cast< GUI_TagEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int GUI_TagEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: id3_tags_changed(); break;
        case 1: id3_tags_changed((*reinterpret_cast< vector<MetaData>(*)>(_a[1]))); break;
        case 2: prev_button_clicked(); break;
        case 3: next_button_clicked(); break;
        case 4: ok_button_clicked(); break;
        case 5: cancel_button_clicked(); break;
        case 6: album_changed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: artist_changed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: all_albums_clicked(); break;
        case 9: all_artists_clicked(); break;
        case 10: all_genre_clicked(); break;
        case 11: all_year_clicked(); break;
        case 12: change_meta_data((*reinterpret_cast< const vector<MetaData>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void GUI_TagEdit::id3_tags_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GUI_TagEdit::id3_tags_changed(vector<MetaData> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
