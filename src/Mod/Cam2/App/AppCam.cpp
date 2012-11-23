/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
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

#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "GCodeFeature.h"
#include "TPGFeature.h"
#include "Feature.h"
#include "StockGeometry.h"
#include "CamPartsList.h"
#include "TPGList.h"


#include "TPG/TPGFactory.h"
#include "TPG/TPG.h"
#include "TPG/PyTPGManager.h"

#include "CamFeature.h"

// C++ TPG Plugins
#include "Plugins/MyPlugin.h"

extern struct PyMethodDef Cam_methods[];

PyDoc_STRVAR(module_Cam_doc,
"This module is the CAM module.");


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

    // Add the PyCam module to the Cam Module
    PyObject* pyCamMod = PyImport_ImportModule("PyCam");
    if (pyCamMod != NULL)
    {
        PyModule_AddObject(camModule, "PyCam", pyCamMod);
        Cam::PyTPGManager().loadCallbackFromModule(pyCamMod);
    	Py_DecRef(pyCamMod);
    }

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    Cam::Feature             ::init();
    Cam::CamFeature          ::init();
    Cam::CamPartsList        ::init();
    Cam::GCodeFeature        ::init();
    Cam::StockGeometry       ::init();
    Cam::TPGFeature          ::init();
    Cam::TPGList             ::init();

    //Initialise and Register and C++ TPG Plugins [TODO SHOULD WE INITIALISE THESE IN A SEPERATE FILE FOR READABILITY]
    Cam::MyPlugin            ::init();
    Cam::TPGFactory().registerPlugin<Cam::MyPlugin>(new Cam::LibTPGDescriptor("MyPlugin", "My CAM Plugin", "My Plugin's Description"));

    Base::Console().Log("Loading CAM module... done\n");

}

} // extern "C"
