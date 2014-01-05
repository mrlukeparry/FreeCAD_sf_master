/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
 *   Copyright (c) Luke Parry             (l.parry@warwick.ac.uk) 2013     *
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
# include <sstream>
#endif

#include <HLRBRep_Algo.hxx>
#include <TopoDS_Shape.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
//#include <BRepAPI_MakeOutLine.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <Geom_Plane.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepMesh.hxx>

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepPrim_FaceBuilder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <boost/concept_check.hpp>

#include <Base/BoundBox.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/Geometry.h>

#include "FeatureViewSection.h"
#include "ProjectionAlgos.h"

using namespace Drawing;
using namespace std;

//===========================================================================
// FeatureViewSection
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureViewSection, Drawing::FeatureViewPart)

FeatureViewSection::FeatureViewSection()
{
    static const char *group = "Shape view";
    static const char *vgroup = "Drawing view";

    ADD_PROPERTY_TYPE(SectionNormal ,(0,0,1.0)    ,group,App::Prop_None,"Section Plane normal direction");
    ADD_PROPERTY_TYPE(SectionOrigin ,(0,0,0) ,group,App::Prop_None,"Section Plane Origin");
    ADD_PROPERTY_TYPE(ShowCutSurface ,(true),group,App::Prop_None,"Show the cut surface");

    geometryObject = new DrawingGeometry::GeometryObject();
}

short FeatureViewSection::mustExecute() const
{
    // If Tolerance Property is touched
    if(SectionNormal.isTouched() ||
       SectionOrigin.isTouched())
          return 1;

    return Drawing::FeatureViewPart::mustExecute();
}

FeatureViewSection::~FeatureViewSection()
{
    delete geometryObject;
}

App::DocumentObjectExecReturn *FeatureViewSection::execute(void)
{
    //## Get the Part Link ##/
    App::DocumentObject* link = Source.getValue();

    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");

    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");

    const Part::TopoShape &partTopo = static_cast<Part::Feature*>(link)->Shape.getShape();

    if (partTopo._Shape.IsNull())
        return new App::DocumentObjectExecReturn("Linked shape object is empty");

    gp_Pln pln = getSectionPlane();
    // Get the Axis Directions for the Plane to transform UV components again
    gp_XYZ xAxis = pln.XAxis().Direction().XYZ();
    gp_XYZ yAxis = pln.YAxis().Direction().XYZ();
    gp_XYZ origin = pln.Location().XYZ();

    Base::BoundBox3d bb = partTopo.getBoundBox();

    Base::Vector3d tmp1 = SectionOrigin.getValue();
    Base::Vector3d tmp2 = SectionNormal.getValue();

    Base::Vector3d plnPnt(tmp1.x, tmp1.y, tmp1.z);
    Base::Vector3d plnNorm(tmp2.x, tmp2.y, tmp2.z);

    if(!bb.IsCutPlane(plnPnt, plnNorm))
        return new App::DocumentObjectExecReturn("Section Plane doesn't intersect part");

    bb.Enlarge(1.0); // Enlarge the bounding box to prevent any clipping

    // Gather the points
    std::vector<Base::Vector3d> pnts;

    pnts.push_back(Base::Vector3d(bb.MinX,bb.MinY,bb.MinZ));
    pnts.push_back(Base::Vector3d(bb.MaxX,bb.MinY,bb.MinZ));
    pnts.push_back(Base::Vector3d(bb.MinX,bb.MaxY,bb.MinZ));
    pnts.push_back(Base::Vector3d(bb.MaxX,bb.MaxY,bb.MinZ));
    pnts.push_back(Base::Vector3d(bb.MinX,bb.MinY,bb.MaxZ));
    pnts.push_back(Base::Vector3d(bb.MaxX,bb.MinY,bb.MaxZ));
    pnts.push_back(Base::Vector3d(bb.MinX,bb.MaxY,bb.MaxZ));
    pnts.push_back(Base::Vector3d(bb.MaxX,bb.MaxY,bb.MaxZ));

    double uMax = 0, vMax = 0, wMax;
    for(std::vector<Base::Vector3d>::const_iterator it = pnts.begin(); it != pnts.end(); ++it) {
        // Project each bounding box point onto projection plane and find larges u,v values

        Base::Vector3d pnt = (*it);
        pnt.ProjToPlane(plnPnt, plnNorm);

        uMax = std::max(uMax, std::abs(plnPnt[0] - pnt[0]));
        vMax = std::max(vMax, std::abs(plnPnt[1] - pnt[1]));

        //wMax is the bounding box point furthest away used for determining extrusion length
        double dist = (*it).DistanceToPlane(plnPnt, plnNorm);
        wMax = std::max(wMax, dist);
    }

    // Build face directly onto plane
    BRepBuilderAPI_MakePolygon mkPoly;
    gp_Pnt pn1(origin + xAxis *  uMax  + yAxis *  vMax);
    gp_Pnt pn2(origin + xAxis *  uMax  + yAxis * -vMax);
    gp_Pnt pn3(origin + xAxis * -uMax  + yAxis  * -vMax);
    gp_Pnt pn4(origin + xAxis * -uMax  + yAxis  * +vMax);
    mkPoly.Add(pn1);
    mkPoly.Add(pn2);
    mkPoly.Add(pn3);
    mkPoly.Add(pn4);
    mkPoly.Close();

    // Make the extrusion face
    BRepBuilderAPI_MakeFace mkFace(mkPoly.Wire());
    TopoDS_Face aProjFace = mkFace.Face();
    if(aProjFace.IsNull())
        return new App::DocumentObjectExecReturn("Failed to cut");
    // Create an infinite projection (investigate if infite extrusion necessary)
//     BRepPrimAPI_MakePrism PrismMaker(from, Ltotal*gp_Vec(dir), 0,1); // finite prism
    TopoDS_Shape prism = BRepPrimAPI_MakePrism(aProjFace, wMax * gp_Vec(pln.Axis().Direction()), 0, 1).Shape();

    // We need to copy the shape to not modify the BRepstructure
    BRepBuilderAPI_Copy BuilderCopy(partTopo._Shape);
    TopoDS_Shape myShape = BuilderCopy.Shape();

    BRepAlgoAPI_Cut mkCut(myShape, prism);
    // Let's check if the fusion has been successful
    if (!mkCut.IsDone())
        return new App::DocumentObjectExecReturn("Section cut has failed");

    // Cache the result
    this->result = mkCut.Shape();

    try {
        geometryObject->setTolerance(Tolerance.getValue());
        geometryObject->extractGeometry(result, Direction.getValue(), ShowHiddenLines.getValue(), XAxisDirection.getValue());
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }
}

gp_Pln FeatureViewSection::getSectionPlane() const
{
    Base::Vector3d plnPnt = SectionOrigin.getValue();
    Base::Vector3d plnNorm = SectionNormal.getValue();

    return gp_Pln(gp_Pnt(plnPnt.x, plnPnt.y, plnPnt.z), gp_Dir(plnNorm.x, plnNorm.y, plnNorm.z));
}

void FeatureViewSection::getSectionSurface(std::vector<DrawingGeometry::Face *> &sectionFace) const {

    if(result.IsNull())
        throw Base::Exception("Sectional View Result is Empty");

    gp_Pln pln = getSectionPlane();
    BRep_Builder builder;
    TopoDS_Compound comp;
    builder.MakeCompound(comp);

    // Iterate through all faces
    TopExp_Explorer edges(result, TopAbs_FACE);
    for ( ; edges.More(); edges.Next()) {
        const TopoDS_Face& face = TopoDS::Face(edges.Current());

        BRepAdaptor_Surface adapt(face);
        if (adapt.GetType() == GeomAbs_Plane){
            gp_Pln plane = adapt.Plane();
            if(plane.Contains(pln.Location(), Precision::Confusion()) && plane.Axis().IsParallel(pln.Axis(), Precision::Angular())) {
                builder.Add(comp, face);
            }
        }
    }
    geometryObject->projectSurfaces(comp, result, Direction.getValue(), XAxisDirection.getValue(), sectionFace);
}
// Python Drawing feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Drawing::FeatureViewSectionPython, Drawing::FeatureViewSection)
template<> const char* Drawing::FeatureViewSectionPython::getViewProviderName(void) const {
    return "DrawingGui::ViewProviderDrawingView";
}
/// @endcond

// explicit template instantiation
template class DrawingExport FeaturePythonT<Drawing::FeatureViewSection>;
}
