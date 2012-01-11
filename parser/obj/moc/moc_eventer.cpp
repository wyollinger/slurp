/****************************************************************************
** Meta object code from reading C++ file 'eventer.h'
**
** Created: Wed Jan 11 15:21:15 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../inc/eventer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eventer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_parser__Eventer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   31,   16,   16, 0x0a,
      63,   31,   16,   16, 0x0a,
      87,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_parser__Eventer[] = {
    "parser::Eventer\0\0startParser()\0,\0"
    "writeOut(parseResult,Parser*)\0"
    "writeFail(QUrl,Parser*)\0quit()\0"
};

const QMetaObject parser::Eventer::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_parser__Eventer,
      qt_meta_data_parser__Eventer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &parser::Eventer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *parser::Eventer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *parser::Eventer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_parser__Eventer))
        return static_cast<void*>(const_cast< Eventer*>(this));
    return QApplication::qt_metacast(_clname);
}

int parser::Eventer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startParser(); break;
        case 1: writeOut((*reinterpret_cast< parseResult(*)>(_a[1])),(*reinterpret_cast< Parser*(*)>(_a[2]))); break;
        case 2: writeFail((*reinterpret_cast< QUrl(*)>(_a[1])),(*reinterpret_cast< Parser*(*)>(_a[2]))); break;
        case 3: quit(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void parser::Eventer::startParser()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
