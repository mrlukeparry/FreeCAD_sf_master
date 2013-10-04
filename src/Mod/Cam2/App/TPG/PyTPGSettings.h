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

#ifndef PYTPGSETTINGS_H_
#define PYTPGSETTINGS_H_

#include <Python.h>
#include <QString>

#include "TPGSettings.h"


#define QString_toPyString(__str__) PyString_FromString(((const char*)(__str__)).toStdString().c_str());
#define QStringPtr_toPyString(__str__) PyString_FromString(((const char*)(__str__))->toStdString().c_str());

extern PyTypeObject *PyTPGSettings_Type();
extern PyTypeObject *PyTPGSettingDefinition_Type();


#define PyCamTPGSettings_Check(__obj__) PyObject_TypeCheck((__obj__), PyTPGSettings_Type())
#define PyTPGSettingDefinition_Check(__obj__) PyObject_TypeCheck((__obj__), PyTPGSettingDefinition_Type())


// ----------------------------------------------------------------------------
// ----- TPGSettingDefinition -------------------------------------------------
// ----------------------------------------------------------------------------

/**
 * A wrapper for the TPGSettingDefinition class to allow access from PyTPGs
 */
typedef struct  {
    PyObject_HEAD
    Cam::Settings::Definition *setting;
} cam_PyTPGSettingDefinition;



// ----------------------------------------------------------------------------
// ----- TPGSettings ----------------------------------------------------------
// ----------------------------------------------------------------------------

/**
 * A wrapper for the TPGSettings class to allow access from PyTPGs
 */
typedef struct  {
    PyObject_HEAD
    Cam::Settings::TPGSettings *settings;
} cam_PyTPGSettings;

/**
 * Wrapper function to create new PyTPGSettings objects from c++
 */
extern PyObject* PyTPGSettings_New(Cam::Settings::TPGSettings* settings);

#endif /* PYTPGSETTINGS_H_ */
