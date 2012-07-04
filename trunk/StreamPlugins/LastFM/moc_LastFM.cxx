/****************************************************************************
** Meta object code from reading C++ file 'LastFM.h'
**
** Created: Wed Jul 4 17:50:15 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "LastFM.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LastFM.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LastFM[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      32,    7,    7,    7, 0x05,
      71,    7,    7,    7, 0x05,
     127,  108,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
     166,    7,    7,    7, 0x0a,
     185,    7,    7,    7, 0x0a,
     211,  209,    7,    7, 0x0a,
     239,  209,    7,    7, 0x0a,
     263,    7,    7,    7, 0x0a,
     284,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LastFM[] = {
    "LastFM\0\0last_fm_logged_in(bool)\0"
    "similar_artists_available(QList<int>&)\0"
    "new_radio_playlist(vector<MetaData>)\0"
    "md,loved,corrected\0"
    "track_info_fetched(MetaData,bool,bool)\0"
    "scrobble(MetaData)\0track_changed(MetaData)\0"
    ",\0login_slot(QString,QString)\0"
    "radio_init(QString,int)\0radio_get_playlist()\0"
    "sim_artists_thread_finished()\0"
};

const QMetaObject LastFM::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LastFM,
      qt_meta_data_LastFM, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LastFM::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LastFM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LastFM::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LastFM))
        return static_cast<void*>(const_cast< LastFM*>(this));
    return QObject::qt_metacast(_clname);
}

int LastFM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: last_fm_logged_in((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: similar_artists_available((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 2: new_radio_playlist((*reinterpret_cast< const vector<MetaData>(*)>(_a[1]))); break;
        case 3: track_info_fetched((*reinterpret_cast< const MetaData(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 4: scrobble((*reinterpret_cast< const MetaData(*)>(_a[1]))); break;
        case 5: track_changed((*reinterpret_cast< const MetaData(*)>(_a[1]))); break;
        case 6: login_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: radio_init((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: radio_get_playlist(); break;
        case 9: sim_artists_thread_finished(); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void LastFM::last_fm_logged_in(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LastFM::similar_artists_available(QList<int> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LastFM::new_radio_playlist(const vector<MetaData> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LastFM::track_info_fetched(const MetaData & _t1, bool _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
