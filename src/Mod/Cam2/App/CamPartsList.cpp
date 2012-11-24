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

#include <Mod/Part/App/Geometry.h>

#include <vector>

#include "CamPartsList.h"
// #include "CamPartsListPy.h"

using namespace Cam;
using namespace Base;

const char *partsGroup = "CAM Parts Feature";

PROPERTY_SOURCE(Cam::CamPartsList, App::DocumentObject)

CamPartsList::CamPartsList()
{
    ADD_PROPERTY_TYPE(Tolerance,(0.01)    , partsGroup, (App::PropertyType)(App::Prop_Output) ,"Tolerance");
    ADD_PROPERTY_TYPE(ExternalGeometry,(0), partsGroup, (App::PropertyType)(App::Prop_None) ,"External geometry");
}

CamPartsList::~CamPartsList()
{
}

short CamPartsList::mustExecute() const
{
    const std::vector<App::DocumentObject *> vals = ExternalGeometry.getValues();
    bool PartsTouched = false;
    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        if((*it)->isTouched())
            PartsTouched = true;
    }

    return (PartsTouched) ? 1 : App::DocumentObject::mustExecute();
}

int CamPartsList::addPart(App::DocumentObject *docObj)
{
    if(!docObj->isDerivedFrom(Part::Feature::getClassTypeId()))
        return -1; //Doc Object mst be derived from a Part Feature

    const std::vector<App::DocumentObject *> vals = ExternalGeometry.getValues();

    std::vector<App::DocumentObject *> newVals(vals);
    newVals.push_back(docObj);
    ExternalGeometry.setValues(newVals);

    return ExternalGeometry.getSize();
}

int CamPartsList::addPart(const char *name)
{
    App::DocumentObject *docObj = getDocument()->getObject(name);
    if(!docObj)
        return -1; //No document object could be found in current document

    return addPart(docObj);
}

int CamPartsList::removePart(const char *name)
{
    App::DocumentObject *docObj = getDocument()->getObject(name);
    if(!docObj)
        return 0;

    const std::vector<App::DocumentObject *> vals = ExternalGeometry.getValues();
    std::vector<App::DocumentObject *> newVals(vals);

    for(std::vector<App::DocumentObject *>::iterator it = newVals.begin(); it < newVals.end(); ++it) {
        if((*it) == docObj) {
            newVals.erase(it);
            ExternalGeometry.setValues(newVals);
            return ExternalGeometry.getSize();
        }
    }
    return -1;

}

void CamPartsList::generateGeometry()
{
    const std::vector<App::DocumentObject *> vals = ExternalGeometry.getValues();

    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        Part::Feature *feat = static_cast<Part::Feature *>(*it);
        const TopoDS_Shape &Shape = feat->Shape.getValue();
        TopExp_Explorer ex;
        BRepMesh_IncrementalMesh MESH(Shape, this->Tolerance.getValue());

        //Generate each face
        for (ex.Init(Shape, TopAbs_FACE); ex.More(); ex.Next()) {
            const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
             // this block mesh the face and transfers it in a C array of vertices and face indexes
            Standard_Integer nbNodesInFace,nbTriInFace;
            gp_Vec* verts=0;
            gp_Vec* vertNorms=0;
            long* cons=0;

            transferToArray(aFace,&verts,&vertNorms,&cons,nbNodesInFace,nbTriInFace);

            if (!verts)
              return; // Failed to compute vertex, normals

            // Somehow use this data // perhaps build STL file
        }
    }
}

void CamPartsList::transferToArray(const TopoDS_Face& aFace,gp_Vec** vertices,gp_Vec** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace )
{
    TopLoc_Location aLoc;

    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if (aPoly.IsNull()) {
        Base::Console().Log("Empty face trianglutaion\n");
        nbNodesInFace = 0;
        nbTriInFace = 0;
        vertices = 0l;
        cons = 0l;
        return;
    }
    // geting the transformation of the shape/face
    gp_Trsf myTransf;
    bool identity = true;

    if (!aLoc.IsIdentity()) {
        identity = false;
        myTransf = aLoc.Transformation();
    }

    Standard_Integer i;
    // geting size and create the array
    nbNodesInFace = aPoly->NbNodes();
    nbTriInFace = aPoly->NbTriangles();
    *vertices = new gp_Vec[nbNodesInFace];
    *vertexnormals = new gp_Vec[nbNodesInFace];

    for (i=0; i < nbNodesInFace; i++) {
        (*vertexnormals)[i]= gp_Vec(0.0,0.0,0.0);
    }

    *cons = new long[3*(nbTriInFace)+1];

    // check orientation
    TopAbs_Orientation orient = aFace.Orientation();

    // cycling through the poly mesh
    const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
    const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
    for (i=1; i<=nbTriInFace; i++) {
        // Get the triangle
        Standard_Integer N1,N2,N3;
        Triangles(i).Get(N1,N2,N3);

        // change orientation of the triangles
        if ( orient != TopAbs_FORWARD )
        {
            Standard_Integer tmp = N1;
            N1 = N2;
            N2 = tmp;
        }

        gp_Pnt V1 = Nodes(N1);
        gp_Pnt V2 = Nodes(N2);
        gp_Pnt V3 = Nodes(N3);

        // transform the vertices to the place of the face
        if (!identity) {
            V1.Transform(myTransf);
            V2.Transform(myTransf);
            V3.Transform(myTransf);
        }

        // Calculate triangle normal
        gp_Vec v1(V1.X(),V1.Y(),V1.Z()),v2(V2.X(),V2.Y(),V2.Z()),v3(V3.X(),V3.Y(),V3.Z());
        gp_Vec Normal = (v2-v1)^(v3-v1);

        //Standard_Real Area = 0.5 * Normal.Magnitude();

        // add the triangle normal to the vertex normal for all points of this triangle
        (*vertexnormals)[N1-1] += gp_Vec(Normal.X(),Normal.Y(),Normal.Z());
        (*vertexnormals)[N2-1] += gp_Vec(Normal.X(),Normal.Y(),Normal.Z());
        (*vertexnormals)[N3-1] += gp_Vec(Normal.X(),Normal.Y(),Normal.Z());

        (*vertices)[N1-1].SetX((float)(V1.X()));
        (*vertices)[N1-1].SetY((float)(V1.Y()));
        (*vertices)[N1-1].SetZ((float)(V1.Z()));

        (*vertices)[N2-1].SetX((float)(V2.X()));
        (*vertices)[N2-1].SetY((float)(V2.Y()));
        (*vertices)[N2-1].SetZ((float)(V2.Z()));

        (*vertices)[N3-1].SetX((float)(V3.X()));
        (*vertices)[N3-1].SetY((float)(V3.Y()));
        (*vertices)[N3-1].SetZ((float)(V3.Z()));

        int j = i - 1;
        N1--;
        N2--;
        N3--;
        (*cons)[3*j] = N1;
        (*cons)[3*j+1] = N2;
        (*cons)[3*j+2] = N3;
    }


    // normalize all vertex normals
    for (i=0; i < nbNodesInFace; i++) {

        gp_Dir clNormal;

        // The Majority of time, the vertex normal is correct, otherwise recalculate
        if ((*vertexnormals)[i].Magnitude() < gp::Resolution())
        {
            // Invalid Vertex Normal Attempt to recalculate
            try {
                Handle_Geom_Surface Surface = BRep_Tool::Surface(aFace);

                gp_Pnt vertex((*vertices)[i].XYZ());
                GeomAPI_ProjectPointOnSurf ProPntSrf(vertex, Surface);
                Standard_Real fU, fV;
                ProPntSrf.Parameters(1, fU, fV);

                GeomLProp_SLProps clPropOfFace(Surface, fU, fV, 2, gp::Resolution());

                clNormal = clPropOfFace.Normal();
                gp_Vec temp = clNormal;
                if ( temp * (*vertexnormals)[i] < 0 )
                    temp = -temp;
                (*vertexnormals)[i] = temp;
            }
            catch (...) {
                continue;
            }
        }

        (*vertexnormals)[i].Normalize();
    }
}

App::DocumentObjectExecReturn *CamPartsList::execute(void)
{
    this->touch();
    return App::DocumentObject::StdReturn;
}

// PyObject *CamPartsList::getPyObject(void)
// {
//     if (PythonObject.is(Py::_None())) {
//         // ref counter is set to 1
//         PythonObject = Py::Object(new CamPartsListPy(this),true);
//     }
//     return Py::new_reference_to(PythonObject);
// }

unsigned int CamPartsList::getMemSize(void) const
{
    return 0;
}

void CamPartsList::Save(Writer &writer) const
{
    // save the father classes

}

void CamPartsList::Restore(XMLReader &reader)
{
    // read the father classes

}

void CamPartsList::onChanged(const App::Property* prop)
{
}

void CamPartsList::onDocumentRestored()
{
    try {
    }
    catch (...) {
    }
}

void CamPartsList::onFinishDuplicating()
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
