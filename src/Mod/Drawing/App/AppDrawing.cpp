/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   J�rgen Riegel 2007                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

# include <Base/Console.h>
# include <Base/Interpreter.h>

#include "FeaturePage.h"
#include "FeatureSVGTemplate.h"
#include "FeatureParametricTemplate.h"
#include "FeatureView.h"
#include "FeatureViewCollection.h"
#include "FeatureViewPart.h"
#include "FeatureViewSection.h"

#include "FeatureViewAnnotation.h"
#include "FeatureViewDimension.h"
#include "FeatureOrthoView.h"
#include "FeatureViewOrthographic.h"
#include "FeatureProjection.h"
#include "FeatureClip.h"
#include "PageGroup.h"

extern struct PyMethodDef Drawing_methods[];

PyDoc_STRVAR(module_drawing_doc,
"This module is the Drawing module.");


/* Python entry */
extern "C" {
void DrawingExport initDrawing()
{
    // load dependent module
    try {
        Base::Interpreter().loadModule("Part");
        Base::Interpreter().loadModule("Measure");
        //Base::Interpreter().loadModule("Mesh");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }
    Py_InitModule3("Drawing", Drawing_methods, module_drawing_doc);   /* mod name, table ptr */
    Base::Console().Log("Loading Drawing module... done\n");


    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    Drawing::FeaturePage            ::init();
    Drawing::FeatureTemplate        ::init();
    Drawing::FeatureSVGTemplate     ::init();
    Drawing::FeatureParametricTemplate::init();
    Drawing::FeatureView            ::init();
    Drawing::FeatureViewCollection  ::init();

    Drawing::FeatureViewPart        ::init();
    Drawing::FeatureViewSection     ::init();
    Drawing::FeatureViewAnnotation  ::init();
    Drawing::FeatureViewDimension   ::init();
    Drawing::FeatureViewOrthographic::init();
    Drawing::FeatureOrthoView       ::init();

    // Python Types
    Drawing::FeatureTemplatePython  ::init();

    Drawing::FeatureProjection      ::init();
    Drawing::FeatureViewPartPython  ::init();
    Drawing::FeatureViewPython      ::init();

    Drawing::FeatureClip            ::init();
}

} // extern "C"
