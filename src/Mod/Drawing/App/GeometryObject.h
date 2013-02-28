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

#ifndef _DRAWING_GEOMETRYOBJECT_H
#define _DRAWING_GEOMETRYOBJECT_H

#include <TopoDS_Shape.hxx>
#include <Base/Vector3D.h>
#include <string>
#include <vector>

#include "Geometry.h"

class HLRBRep_Algo;
class Handle_HLRBRep_Data;
class HLRBRep_EdgeData;

namespace DrawingGeometry
{

class BaseGeom;
/** Algo class for projecting shapes and creating SVG output of it
 */
class DrawingExport GeometryObject
{
public:
    /// Constructor
    GeometryObject();
    virtual ~GeometryObject();

    void clear();

    void setTolerance(double value);
    const std::vector<BaseGeom *> & getEdgeGeometry() const { return edgeGeom; };
    const std::vector<BaseGeom *> & getFaceGeometry() const { return faceGeom; };
    
    const std::vector<int> & getEdgeRefs() const { return edgeReferences; };
    const std::vector<int> & getFaceRefs() const { return faceReferences; };

    void extractGeometry(const TopoDS_Shape &input,const Base::Vector3f &direction);

protected:
    // Reimplements HLRBRep Drawing Algorithms to satisfy Drawing Workbench requirements
    void drawFace (const int visible, const int typ, const int iface, Handle_HLRBRep_Data & DS, TopoDS_Shape& Result) const;
    void drawEdge(int ie, bool visible, bool inFace, int typ, HLRBRep_EdgeData& ed, TopoDS_Shape& Result) const;
    
    void extractEdges(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, int type, bool visible, ExtractionType extractionType);
    void extractFaces(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, int type, bool visible, ExtractionType extractionType);
    int calculateGeometry(const TopoDS_Shape &input, ExtractionType extractionType, const int ie,  std::vector<BaseGeom *> &geoms);
    
    void createWire(const TopoDS_Shape &input, TopoDS_Shape &result);    
    TopoDS_Shape invertY(const TopoDS_Shape& shape);
    
    // Geometry
    std::vector<BaseGeom *> edgeGeom;
    std::vector<BaseGeom *> faceGeom;
    
    // Linked Edges and Faces to base object
    std::vector<int> edgeReferences;
    std::vector<int> faceReferences;

    double Tolerance;
    HLRBRep_Algo *brep_hlr;
};

} //namespace DrawingGeometry

#endif
