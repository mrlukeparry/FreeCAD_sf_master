/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
 *   Copyright (c) 2013 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#include <PreCompiled.h>
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>

//#include "Features/GCodeFeature.h"
#include "Features/TPGFeature.h"
//#include "Features/StockGeometry.h"
//#include "Features/CamPartsList.h"
//#include "Features/TPGList.h"
#include "Features/CamFeature.h"


#include "TPG/TPGFactory.h"
#include "TPG/TPG.h"
#include "TPG/PyTPGFactory.h"


// C++ TPG Plugins
#include "Plugins/MyPlugin.h"

extern struct PyMethodDef Cam_methods[];

PyDoc_STRVAR(module_Cam_doc,
"This module is the CAM module.");


#include "TPG/PyToolPath.cpp"
#include "TPG/PyTPGSettings.cpp"


/* Python entry */
extern "C" {
void CamExport initCam()
{
    // load dependent module
    try {
        Base::Interpreter().runString("import Part");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }

    PyObject* camModule = Py_InitModule3("Cam", Cam_methods, module_Cam_doc);   /* mod name, table ptr */

    // Add Types to module
    Py_INCREF(&PyToolPathType);
    PyModule_AddObject(camModule, "ToolPath", (PyObject *)&PyToolPathType);
    if (PyType_Ready(&PyToolPathType) < 0)
        return;

    Py_INCREF(&PyTPGSettingsType);
    PyModule_AddObject(camModule, "TPGSettings", (PyObject *)&PyTPGSettingsType);
    if (PyType_Ready(&PyTPGSettingsType) < 0)
        return;

    // Add the PyCam module to the Cam Module
    PyObject* pyCamMod = PyImport_ImportModule("PyCam");
    if (pyCamMod != NULL)
    {
        PyModule_AddObject(camModule, "PyCam", pyCamMod);
        Cam::PyTPGFactory().loadCallbackFromModule(pyCamMod);
    	Py_DecRef(pyCamMod);
    }
    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    Cam::CamFeature          ::init();
//    Cam::CamPartsList        ::init();
//    Cam::GCodeFeature        ::init();
//    Cam::StockGeometry       ::init();
    Cam::TPGFeature          ::init();
//    Cam::TPGList             ::init();

    // Perform initial scan to load all TPGDescriptors in the factory to ensure that documents can load these
    Cam::TPGFactory().scanPlugins();
    Base::Console().Log("Loading CAM module... done\n");

}

} // extern "C"
