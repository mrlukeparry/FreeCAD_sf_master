/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson  (andrewjrobinson@gmail.com)       *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PyTPGManager.h"
#include <QVector>
#include <Base/Interpreter.h>

namespace Cam {

PyTPGManagerInst* PyTPGManagerInst::_pcSingleton = NULL;

PyTPGManagerInst& PyTPGManagerInst::instance(void)
{
    if (_pcSingleton == NULL)
        _pcSingleton = new PyTPGManagerInst();

    return *_pcSingleton;
}
void PyTPGManagerInst::destruct (void)
{
    if (_pcSingleton != NULL)
        delete _pcSingleton;
    _pcSingleton = NULL;
}

PyTPGManagerInst::PyTPGManagerInst()
{
    Base::Interpreter().runString("import PyTPG");
    this->obj = NULL;
}

/**
 * Cleanup the Python objects if set
 */
PyTPGManagerInst::~PyTPGManagerInst()
{
    if (this->obj != NULL) {
        Py_DecRef(this->obj);
        this->obj = NULL;
    }
}

///////////// Callback setters //////////////

/**
 * Saves the callback object for later use.
 * Note: Refcount is increased for both params when it is stored
 */
void PyTPGManagerInst::setCallback(PyObject* obj)
{
    if (this->obj != NULL)
        Py_DecRef(this->obj);
    this->obj = obj;
    Py_IncRef(obj);
}

/**
 * Gets the PyTPGManager python class from the given module and sets it as the
 * callback
 */
void PyTPGManagerInst::loadCallbackFromModule(PyObject *mod)
{
    PyObject *pyTPGManagercls = PyObject_GetAttrString(mod, "PyTPGManager");
    if (pyTPGManagercls != NULL) {
        PyObject *obj = PyObject_CallMethod(pyTPGManagercls, "instance", NULL);
        if (obj != NULL) {
            setCallback(obj);
            Py_DecRef(obj);
        }
        Py_DecRef(pyTPGManagercls);
    }
}


///////// Testing methods ///////////////

void PyTPGManagerInst::test()
{
    if (this->obj != NULL) {
        PyObject * result = PyObject_CallMethod(obj, "printPlugins", "");

        if (result != NULL)
          Py_DecRef(result);
    }
}

bool PyTPGManagerInst::test1(PyObject* obj)
{
//    return this->pytest(obj);
    QString str = this->PythonUCToQString(obj);
    printf("Test1((QString)'%s')\n", str.toStdString().c_str());
    PyObject* uc = this->QStringToPythonUC(str);
    return this->pytest(uc);
}


bool PyTPGManagerInst::pytest(PyObject* arg)
{
    if (this->obj != NULL)
    {
        PyObject * result = PyObject_CallMethod(this->obj, "pytest", "(O)", arg);
        if (result != NULL) {
            Py_DecRef(result);
            return true;
        } else {
          if (PyErr_Occurred() == NULL) {
            printf("pytest: no error\n");
            return true;
          } else
            printf("pytest: error\n");
        }
    }

    return false;
}

//////////// C++ API (to Python TPG's) ///////////////
/**
 * Returns (by reference) the list of found Python Plugins.
 * This is a list of ID's
 */
std::vector<QString> &PyTPGManagerInst::scanPlugins()
{
    if (this->obj != NULL) {
        PyObject * result = PyObject_CallMethod(obj, "scanPlugins", NULL);
        if (result != NULL) {
            if (PyList_Check(result)) {

              this->plugins.clear();
              int len = PyList_Size(result);

              for (int i = 0; i < len; i++) {
                  PyObject *item = PyList_GetItem(result, i);
                  this->plugins.push_back(PythonUCToQString(item));
              }
            }
            Py_DecRef(result);
        }
    }
    return this->plugins;
}

/**
 * Gets a TPG given its id.
 */
TPGPython *PyTPGManagerInst::getPlugin(QString id)
{
    TPGPython *tpg = NULL;
    if (this->obj != NULL)
    {
        PyObject *arg    = QStringToPythonUC(id);
        PyObject *result = PyObject_CallMethod(obj, "getPlugin", "(O)", arg);

        if (result != NULL) {
          if (PyType_Check(result)) {
            tpg = new TPGPython(result);
            return tpg;
          } else
              printf("ERROR Unable to get Python Class'%s'\n", Py_TYPE(result)->tp_name);

          Py_DecRef(result);
        }
    }
    return tpg;
}

//////////// Support methods /////////////
//TODO: move to a separate CPP file
PyObject *PyTPGManagerInst::QStringToPythonUC(const QString &str)
{
    //TODO: check this works properly, it appears to work with the UCS4 characters I tested
    return PyString_FromString((const char*)str.toStdString().c_str());
}

QString PyTPGManagerInst::PythonUCToQString(PyObject *obj)
{
    Py_UNICODE *pid = PyUnicode_AsUnicode(obj);

    if (pid != NULL)
        return QString::fromUcs4((const uint *)pid); //TODO: this should have a check for older pythons that use Ucs2)

    if (PyString_Check(obj))
        return QString::fromLatin1(PyString_AS_STRING(obj));

    return QString();
}

} /* namespace Cam */