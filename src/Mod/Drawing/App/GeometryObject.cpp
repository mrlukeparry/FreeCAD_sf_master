/***************************************************************************
 *   Copyright (c) 2012 Luke Parry <l.parry@warwick.ac.uk>                 *
 *                                                                         *
 *   This file is Drawing of the FreeCAD CAx development system.           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A DrawingICULAR PURPOSE.  See the      *
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
# include <BRepAdaptor_Curve.hxx>
# include <Geom_Circle.hxx>
# include <gp_Circ.hxx>
# include <gp_Elips.hxx>
#endif

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <HLRBRep_Algo.hxx>

#include <TopoDS_Shape.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
//#include <BRepAPI_MakeOutLine.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
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
#include <BRepMesh.hxx>

#include <BRepAdaptor_CompCurve.hxx>
#include <Handle_BRepAdaptor_HCompCurve.hxx>
#include <Approx_Curve3d.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <Handle_BRepAdaptor_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Handle_Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <GeomConvert_BSplineCurveToBezierCurve.hxx>
#include <GeomConvert_BSplineCurveKnotSplitting.hxx>
#include <Geom2d_BSplineCurve.hxx>

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Tools.h>
#include <Mod/Part/App/PartFeature.h>

#include "GeometryObject.h"
#include "ProjectionAlgos.h"

using namespace DrawingGeometry;

GeometryObject::GeometryObject() : brep_hlr(0)
{ 

}

GeometryObject::~GeometryObject()
{
    this->clear();
}

void GeometryObject::setTolerance(double value)
{
    this->Tolerance = value; 
}

void GeometryObject::clear()
{
    for(std::vector<BaseGeom *>::iterator it = geometry.begin(); it != geometry.end(); ++it) {
        delete *it;
        *it = 0;
    }
    geometry.clear();
    if(brep_hlr)
        brep_hlr->Delete();
}

void GeometryObject::extractGeometry(const TopoDS_Shape &input, const Base::Vector3f &direction, double tolerance)
{
    // Clear previous Geometry that may have been stored
    this->clear();
    
    HLRBRep_Algo *brep_hlr = new HLRBRep_Algo();
    brep_hlr->Add(input);

    try {
        #if defined(__GNUC__) && defined (FC_OS_LINUX)
        Base::SignalException se;
        #endif
        gp_Ax2 transform(gp_Pnt(0,0,0),gp_Dir(direction.x,direction.y,direction.z));
        HLRAlgo_Projector projector( transform );
        brep_hlr->Projector(projector);
        brep_hlr->Update();
        brep_hlr->Hide();
    }
    catch (...) {
        Standard_Failure::Raise("Fatal error occurred while projecting shape");
    }

    // extracting the result sets:
    HLRBRep_HLRToShape shapes( brep_hlr );

    // V  = shapes.VCompound       ();// hard edge visibly
    // V1 = shapes.Rg1LineVCompound();// Smoth edges visibly
    // VN = shapes.RgNLineVCompound();// contour edges visibly
    // VO = shapes.OutLineVCompound();// contours apparents visibly
    // VI = shapes.IsoLineVCompound();// isoparamtriques   visibly
    // H  = shapes.HCompound       ();// hard edge       invisibly
    // H1 = shapes.Rg1LineHCompound();// Smoth edges  invisibly
    // HN = shapes.RgNLineHCompound();// contour edges invisibly
    // HO = shapes.OutLineHCompound();// contours apparents invisibly
    // HI = shapes.IsoLineHCompound();// isoparamtriques   invisibly

    calculateGeometry(shapes.HCompound(),  WithHidden);
    calculateGeometry(shapes.OutLineHCompound(), WithHidden);
    calculateGeometry(shapes.Rg1LineHCompound(), (ExtractionType)(WithSmooth | WithHidden)); // Smooth
    
    // Extract Visible Edges
    calculateGeometry(shapes.VCompound(), Plain);
    calculateGeometry(shapes.OutLineVCompound(), Plain);
    calculateGeometry(shapes.Rg1LineVCompound(), WithSmooth); // Smooth Edge    
}

void GeometryObject::calculateGeometry(const TopoDS_Shape &input, ExtractionType extractionType)
{
    if(input.IsNull())
        return; // There is no OpenCascade Geometry to be calculated
    
    // build a mesh to explore the shape
    BRepMesh::Mesh(input, Tolerance); 

    // Explore all edges of input and calculate base geometry representation
    TopExp_Explorer edges(input, TopAbs_EDGE); 
    for (int i = 1 ; edges.More(); edges.Next(),i++) {
        const TopoDS_Edge& edge = TopoDS::Edge(edges.Current());
        BRepAdaptor_Curve adapt(edge);
        
        switch(adapt.GetType()) {
          case GeomAbs_Circle: {
            double f = adapt.FirstParameter();
            double l = adapt.LastParameter();
            gp_Pnt s = adapt.Value(f);
            gp_Pnt e = adapt.Value(l);

            if (fabs(l-f) > 1.0 && s.SquareDistance(e) < 0.001) {
                  Circle *circle = new Circle(adapt);
                  circle->extractType = extractionType;
                  geometry.push_back(circle);
            } else {
                  AOC *aoc = new AOC(adapt);
                  aoc->extractType = extractionType;
                  geometry.push_back(aoc);
            }
        } break;
          case GeomAbs_Ellipse: {
            double f = adapt.FirstParameter();
            double l = adapt.LastParameter();
            gp_Pnt s = adapt.Value(f);
            gp_Pnt e = adapt.Value(l);
            if (fabs(l-f) > 1.0 && s.SquareDistance(e) < 0.001) {
                  Ellipse *ellipse = new Ellipse(adapt);
                  ellipse->extractType = extractionType;
                  geometry.push_back(ellipse);
            } else {
                  AOE *aoe = new AOE(adapt);
                  aoe->extractType = extractionType;
                  geometry.push_back(aoe);
            }
          } break;
          default: {            
            Generic *primitive = new Generic(adapt);
            primitive->extractType = extractionType;
            geometry.push_back(primitive);
            break;/*else if (adapt.GetType() == GeomAbs_Ellipse) {
            (adapt, i, result);
        }
        else if (adapt.GetType() == GeomAbs_BSplineCurve) {
            printBSpline(adapt, i, result);
        }
        // fallback
        else {
            printGeneric(adapt, i, result);
        }*/
      } 
    }
}

