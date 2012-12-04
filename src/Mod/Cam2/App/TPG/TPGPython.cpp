/***************************************************************************
 *   Copyright (c) 2012 Luke Parry      (l.parry@warwick.ac.uk)            *
 *                 2012 Andrew Robinson (andrewjrobinson@gmail.com)        *
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

#include <QString>

#include "../TPGFeature.h"
#include "TPGPython.h"
#include "PyToolPath.h"

using namespace Cam;

// const char* ts(QString str)
// {
//     return str.toAscii().constData();
// }
//
// const char* ts(QString *str)
// {
//     if (str != NULL)
//         return str->toAscii().constData();
//     return "NULL";
// }

TPGPython::TPGPython(PyObject *cls) :
		TPG()
{
	//TODO: check the cls is an instance of PyTPG (somehow, possibly checking for the presence of api methods)
	Py_XINCREF(cls);
	this->cls = cls;
	this->inst = NULL;
}

TPGPython::~TPGPython()
{
	Py_XDECREF(this->cls);
}

PyObject *TPGPython::QStringToPythonUC(const QString &str)
{
	//TODO: check this works properly, it appears to work with the UCS4 characters I tested
	return PyString_FromString((const char*) str.toStdString().c_str());
}

QString TPGPython::PythonUCToQString(PyObject *obj)
{
	Py_UNICODE *pid = PyUnicode_AsUnicode(obj);

	if (pid != NULL)
		return QString::fromUcs4((const uint *) pid); //TODO: this should have a check for older pythons that use Ucs2)
	if (PyString_Check(obj))
		return QString::fromLatin1(PyString_AS_STRING(obj));
	return QString();
}

/**
 * Creates an instance of cls and stores it in inst if it doesn't exist
 */
PyObject *TPGPython::getInst(void)
{
	if (inst == NULL)
	{
		printf("Creating Python Instance\n");
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject *result = PyObject_CallObject(cls, NULL);

		if (result != NULL)
			inst = result;
        PyGILState_Release(state);
	}
	return inst;
}

// TPG API methods.  Used by the CAM workbench to run the TPG

QString TPGPython::getId()
{
	if (this->id.size() == 0 && this->cls != NULL)
	{
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject *result = PyObject_CallMethod(this->cls, "getId", NULL);
		if (result != NULL)
		{
			this->id = PythonUCToQString(result);
			Py_DecRef(result);
		}
        PyGILState_Release(state);
	}
	return this->id;
}
QString TPGPython::getName()
{
	if (this->cls != NULL)
	{
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject * result = PyObject_CallMethod(this->cls, "getName", NULL);
		if (result != NULL)
		{
			this->name = PythonUCToQString(result);
			Py_DecRef(result);
		}
        PyGILState_Release(state);
	}
	return this->name;
}
QString TPGPython::getDescription()
{
	if (this->cls != NULL)
	{
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject * result = PyObject_CallMethod(this->cls, "getDescription",
				NULL);
		if (result != NULL)
		{
			this->description = PythonUCToQString(result);
			//			printf("Description: %s\n", ts(description));
			Py_DecRef(result);
		}
        PyGILState_Release(state);
	}
	return this->description;
}

/**
 * Returns a list of action names supported by this TPG.
 *
 * Note: if this TPG only does one thing then just return a list containing 'default'
 */
std::vector<QString> &TPGPython::getActions()
{
	PyObject *inst = getInst();
	if (inst != NULL)
	{
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject *result = PyObject_CallMethod(inst, "getActions", NULL);
		if (result != NULL)
		{
			if (PyList_Check(result))
			{
				int len = PyList_Size(result);
				for (int i = 0; i < len; i++)
				{
					PyObject *item = PyList_GetItem(result, i);
					this->actions.push_back(PythonUCToQString(item));
				}
			}
			Py_DecRef(result);
		}
        PyGILState_Release(state);
	}
	return this->actions;
}

/**
 * Get the settings for a given action
 * Note: this is a pointer to the internal storage so don't delete it.
 */
TPGSettings *TPGPython::getSettings(QString &action)
{
	TPGSettings *setting = NULL;
	std::map<QString, TPGSettings*>::iterator it = settings.find(action);
	PyObject *inst = getInst();
	if (inst != NULL)
	{
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject *arg = QStringToPythonUC(action);
		PyObject *result = PyObject_CallMethod(inst, "getSettings", "(O)", arg);
		if (result != NULL)
		{
			if (PyList_Check(result))
			{
				setting = new TPGSettings();
				int len = PyList_Size(result);

				for (int i = 0; i < len; i++)
				{
					PyObject *item = PyList_GetItem(result, i);
					if (PyTuple_Check(item))
					{
						char *name;
						char *label;
						char *type;
						char *defaultvalue;
						char *units;
						char *helptext;
//						int items = PyTuple_Size(item);


						if (PyArg_ParseTuple(item, "zzzzzz", &name, &label, &type,
								&defaultvalue, &units, &helptext))
						{
							setting->addSetting(
									new TPGSetting(name, label, type, defaultvalue,
											units, helptext));
						}
					}
					else
						printf("Not a Tuple!\n");
				}
				settings[action] = setting;
			}
			Py_DecRef(result);
		}
		Py_DecRef(arg);
        PyGILState_Release(state);
	}
	else
	{
		setting = it->second;
	}
	return setting;
}

/**
 * Run the TPG to generate the ToolPath code.
 *
 * Note: the return will change once the TP Language has been set in store
 */
void TPGPython::run(TPGSettings *settings, QString action)
{
	PyObject *inst = getInst();
	if (inst != NULL)
	{
        PyGILState_STATE state = PyGILState_Ensure();
		PyObject *actionArg = QStringToPythonUC(action);
		PyObject *settingsArg = PyList_New(0); //TODO: Populate the settings once the TPGSettings class is implemented
		PyObject *result = PyObject_CallMethod(inst, "run", "(OO)", actionArg,
				settingsArg);
		if (result != NULL)
		{
			Py_DecRef(result);
			printf("pyAction: %s Successful\n", action.toAscii().constData());
		}
		else
			printf("Failed to run pyAction: %s\n",
					action.toAscii().constData());

		Py_XDECREF(actionArg);
		Py_XDECREF(settingsArg);
        PyGILState_Release(state);
	}
	//TODO: make error
}

/**
 * Returns the toolpath from the last (or current) tool-path run
 */
ToolPath *TPGPython::getToolPath() {
    PyObject *inst = getInst();
    ToolPath *res = NULL;
    if (inst != NULL) {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject *result = PyObject_CallMethod(inst, "getToolPath", "");
        if (result != NULL) {
            if (typeid(result) == typeid(cam_PyToolPath)) {
                res = ((cam_PyToolPath*) result)->tp;
            }
            Py_DecRef(result);
        }
        PyGILState_Release(state);
    }
    return res;
}
