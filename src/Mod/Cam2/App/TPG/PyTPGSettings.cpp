/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include "PyTPGSettings.h"


/**
 * The python deallocator
 */
static void
PyTPGSettings_dealloc(cam_PyTPGSettings* self) {
    if (self->settings != NULL) {
        self->settings->release();
        self->settings = NULL;
    }
    self->ob_type->tp_free((PyObject*)self);
}

/**
 * The python allocator
 */
static PyObject *
PyTPGSettings_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    cam_PyTPGSettings *self;

    self = (cam_PyTPGSettings *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->settings = new Cam::TPGSettings();
    }

    return (PyObject *)self;
}

/**
 * Python initialiser
 */
static int
PyTPGSettings_init(cam_PyTPGSettings *self, PyObject *args, PyObject *kwds) {

//    self->ts->clear();
    return 0;
}

/**
 * Python Attribute getter
 */
static PyObject *
PyTPGSettings_getattro(cam_PyTPGSettings *self, PyObject *name) {

    // get default attributes (such as methods)
    PyObject *tmp;
    if (!(tmp = PyObject_GenericGetAttr((PyObject *) self, name))) {
        if (!PyErr_ExceptionMatches(PyExc_AttributeError))
            return NULL;
        PyErr_Clear();
    } else
        return tmp;

    // otherwise try my own attributes
    char* attrname = PyString_AsString(name);
    QString *tmpstr = self->settings->getSetting(attrname);
    if (tmpstr != NULL)
        return PyString_FromString(((const char*)(tmpstr->toStdString().c_str())));
    Py_RETURN_NONE;
}

/**
 * Add a command to the toolpath
 */
static PyObject *
PyTPGSettings_addSetting (cam_PyTPGSettings* self, PyObject* args) {

    //Expects (<name>, <label>, <type>, <defaultvalue>, <unit>, <helptext>)

    const char *name;
    const char *label;
    const char *type;
    const char *value;
    const char *unit;
    const char *helptext;

    if (!PyArg_ParseTuple(args, "ssssss", &name, &label, &type, &value, &unit, &helptext)) {
        PyErr_SetString(PyExc_TypeError, "Expects '<name>, <label>, <type>, <defaultvalue>, <unit>, <helptext>' (6 strings)");
        return NULL;
    }

    Cam::TPGSettingDefinition *setting = new Cam::TPGSettingDefinition(name, label, type, value, unit, helptext);
    self->settings->addSetting(setting);

    // release my copy of setting (TPGSettings object will grab its own copy)
    setting->release();

    Py_RETURN_NONE;
}

/**
 * Method table for PyToolPath python type
 */
static PyMethodDef PyTPGSettings_methods[] = {
    {"addToolPath", (PyCFunction)PyTPGSettings_addSetting, METH_VARARGS, "Add a setting to this settings collection"},
    {NULL}  /* Sentinel */
};

static PyTypeObject PyTPGSettingsType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "cam.PyTPGSettings",       /*tp_name*/
    sizeof(PyTPGSettingsType), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyTPGSettings_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    PyTPGSettings_getattro,    /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,       /*tp_flags*/
    "Storage object for a collection TPG Settings", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyTPGSettings_methods,     /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyTPGSettings_init, /* tp_init */
    0,                         /* tp_alloc */
    PyTPGSettings_new,         /* tp_new */
};
