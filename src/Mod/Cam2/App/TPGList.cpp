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
# include <BRep_Tool.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <Poly_Triangulation.hxx>
# include <TopExp_Explorer.hxx>
# include <sstream>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
#endif

#include <App/ComplexGeoData.h>

# include <App/Document.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Tools.h>
#include <Base/Console.h>
#include <Base/Exception.h>

#include <Mod/Part/App/Geometry.h>

#include <vector>

#include "CamPartsList.h"
#include "StockGeometry.h"
#include "TPGFeature.h"
#include "TPGList.h"
#include "TPG/TPG.h"
// #include "TPGListPy.h"

/*
 * This class needs to manage the list of INPUT geometry that is used by each TPG
 * and provide methods of extracting geometry shapes from the TPG that are independant of OpenCASCADE
 **/

using namespace Cam;
using namespace Base;

const char *group = "CAM Parts Feature";

PROPERTY_SOURCE(Cam::TPGList, App::DocumentObject)

TPGList::TPGList() : stockGeometry(NULL), partsList(NULL)
{
    ADD_PROPERTY_TYPE(TPGObjects,(0), group, (App::PropertyType)(App::Prop_None) ,"TPG Features");
}

TPGList::~TPGList()
{
}

void TPGList::clear()
{
    const std::vector<App::DocumentObject *> &vals = TPGObjects.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        getDocument()->remObject((*it)->getNameInDocument()); // NOTE this doesn't seem right
    }

    std::vector<App::DocumentObject *> emptyVec;
    TPGObjects.setValues(emptyVec);
}

std::vector<TPGFeature *> TPGList::getTPGFeatureList()
{
    const std::vector<App::DocumentObject *> &vals = TPGObjects.getValues();
    std::vector<TPGFeature *> tpgFeatureVals;
    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        tpgFeatureVals.push_back(static_cast<TPGFeature *>(*it));
    }
    return tpgFeatureVals;
}

int TPGList::addTPG(App::DocumentObject *docObj)
{
    if(!docObj || !docObj->isDerivedFrom(TPGFeature::getClassTypeId()))
        return -1; //Doc Object mst be derived from a Part Feature

    const std::vector<App::DocumentObject *> vals = TPGObjects.getValues();

    std::vector<App::DocumentObject *> newVals(vals);
    newVals.push_back(docObj);
    TPGObjects.setValues(newVals);

    return TPGObjects.getSize();
}

int TPGList::addTPG(const char *name)
{
    App::DocumentObject *docObj = getDocument()->getObject(name);
    return addTPG(docObj);
}

int TPGList::removeTPG(const char *name)
{
    App::DocumentObject *docObj = getDocument()->getObject(name);
    if(!docObj)
        return 0;

    const std::vector<App::DocumentObject *> vals = TPGObjects.getValues();
    std::vector<App::DocumentObject *> newVals(vals);

    for(std::vector<App::DocumentObject *>::iterator it = newVals.begin(); it < newVals.end(); ++it) {
        if((*it) == docObj) {
            newVals.erase(it);
            TPGObjects.setValues(newVals);
            return TPGObjects.getSize();
        }
    }
    return -1;

}

void TPGList::updateAll() {
    Base::Console().Log("Update All TPGList \n");
    const std::vector<App::DocumentObject *> vals = TPGObjects.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        TPGFeature *tpgFeat = static_cast<TPGFeature *>(*it);
        tpgFeat->run();
    }
}

App::DocumentObjectExecReturn *TPGList::execute(void)
{
    // Help Log exectue
    std::stringstream ss;
    ss << "Executing TPGList: " << getNameInDocument() << "\n";
    Base::Console().Log(ss.str().c_str());
    updateAll();
    return App::DocumentObject::StdReturn;
}

// PyObject *TPGList::getPyObject(void)
// {
//     if (PythonObject.is(Py::_None())) {
//         // ref counter is set to 1
//         PythonObject = Py::Object(new TPGListPy(this),true);
//     }
//     return Py::new_reference_to(PythonObject);
// }

unsigned int TPGList::getMemSize(void) const
{
    return 0;
}

// void TPGList::Save(Writer &writer) const
// {
//     // save the father classes
//     App::DocumentObject::Save(writer);
// }
// 
// void TPGList::Restore(XMLReader &reader)
// {
//     // read the father classes
//     App::DocumentObject::Restore(reader);
// }


// void TPGList::onDocumentRestored()
// {
//     try {
//     }
//     catch (...) {
//     }
// }

void TPGList::onFinishDuplicating()
{
}

// Python Cam feature ---------------------------------------------------------

// namespace App {
// /// @cond DOXERR
// PROPERTY_SOURCE_TEMPLATE(Sketcher::CamPartsPython, Sketcher::CamParts)
// template<> const char* Sketcher::CamPartsPython::getViewProviderName(void) const {
//     return "SketcherGui::ViewProviderPython";
// }
// /// @endcond
//
// // explicit template instantiation
// template class SketcherExport FeaturePythonT<Sketcher::CamParts>;
// }
//
