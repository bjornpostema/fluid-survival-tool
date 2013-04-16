/****************************************************************************
** Meta object code from reading C++ file 'GUIController.h'
**
** Created: Tue Apr 16 17:03:39 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../FST/controller/GUIController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GUIController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GUIController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      26,   14,   14,   14, 0x08,
      43,   14,   38,   14, 0x08,
      55,   14,   38,   14, 0x08,
      69,   14,   14,   14, 0x08,
      85,   14,   14,   14, 0x08,
      95,   14,   14,   14, 0x08,
     106,   14,   38,   14, 0x08,
     117,   14,   38,   14, 0x08,
     130,   14,   14,   14, 0x08,
     145,   14,   14,   14, 0x08,
     153,   14,   14,   14, 0x08,
     174,   14,   14,   14, 0x08,
     188,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GUIController[] = {
    "GUIController\0\0modelNew()\0modelOpen()\0"
    "bool\0modelSave()\0modelSaveAs()\0"
    "modelModified()\0specNew()\0specOpen()\0"
    "specSave()\0specSaveAs()\0specModified()\0"
    "about()\0openProjectWebsite()\0generateSTD()\0"
    "generateProbFunc()\0"
};

void GUIController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GUIController *_t = static_cast<GUIController *>(_o);
        switch (_id) {
        case 0: _t->modelNew(); break;
        case 1: _t->modelOpen(); break;
        case 2: { bool _r = _t->modelSave();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = _t->modelSaveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: _t->modelModified(); break;
        case 5: _t->specNew(); break;
        case 6: _t->specOpen(); break;
        case 7: { bool _r = _t->specSave();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: { bool _r = _t->specSaveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: _t->specModified(); break;
        case 10: _t->about(); break;
        case 11: _t->openProjectWebsite(); break;
        case 12: _t->generateSTD(); break;
        case 13: _t->generateProbFunc(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GUIController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GUIController::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_GUIController,
      qt_meta_data_GUIController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GUIController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GUIController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GUIController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GUIController))
        return static_cast<void*>(const_cast< GUIController*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int GUIController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
