/***************************************************************************
 *   Copyright (c) Juergen Riegel          (juergen.riegel@web.de) 2002    *
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

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include <Mod/Part/App/PartFeature.h>

#include "Geometry.h"
#include "FeatureViewPart.h"
#include "ProjectionAlgos.h"

using namespace Drawing;
using namespace std;

//===========================================================================
// FeatureViewPart
//===========================================================================

App::PropertyFloatConstraint::Constraints FeatureViewPart::floatRange = {0.01f,5.0f,0.05f};
PROPERTY_SOURCE(Drawing::FeatureViewPart, Drawing::FeatureView)

FeatureViewPart::FeatureViewPart(void) : geometryObject(0)
{
    static const char *group = "Shape view";
    static const char *vgroup = "Drawing view";

    ADD_PROPERTY_TYPE(Direction ,(0,0,1.0)    ,group,App::Prop_None,"Projection normal direction");
    ADD_PROPERTY_TYPE(XAxisDirection ,(0,0,0) ,group,App::Prop_None,"X-Axis direction");
    ADD_PROPERTY_TYPE(Source    ,(0)          ,group,App::Prop_None,"Shape to view");
    ADD_PROPERTY_TYPE(ShowHiddenLines ,(false),group,App::Prop_None,"Control the appearance of the dashed hidden lines");
    ADD_PROPERTY_TYPE(LineWidth,(0.7f)         ,vgroup,App::Prop_None,"The thickness of the resulting lines");
    ADD_PROPERTY_TYPE(Tolerance,(0.05f)       ,vgroup,App::Prop_None,"The tessellation tolerance");
    Tolerance.setConstraints(&floatRange);

    geometryObject = new DrawingGeometry::GeometryObject();
}

short FeatureViewPart::mustExecute() const
{
    // If Tolerance Property is touched
    if(Direction.isTouched() ||
       XAxisDirection.isTouched() ||
       Source.isTouched() ||
       Scale.isTouched())
          return 1;
    else
        return Drawing::FeatureView::mustExecute();
}

FeatureViewPart::~FeatureViewPart()
{
    delete geometryObject;
}

const std::vector<DrawingGeometry::Vertex *> & FeatureViewPart::getVertexGeometry() const
{
    return geometryObject->getVertexGeometry();
}

const std::vector<int> & FeatureViewPart::getVertexReferences() const
{
    return geometryObject->getVertexRefs();
}

const std::vector<DrawingGeometry::Face *> & FeatureViewPart::getFaceGeometry() const
{
    return geometryObject->getFaceGeometry();
}

const std::vector<int> & FeatureViewPart::getFaceReferences() const
{
    return geometryObject->getFaceRefs();
}

const std::vector<DrawingGeometry::BaseGeom  *> & FeatureViewPart::getEdgeGeometry() const
{
    return geometryObject->getEdgeGeometry();
}

const std::vector<int> & FeatureViewPart::getEdgeReferences() const
{
    return geometryObject->getEdgeRefs();
}

DrawingGeometry::BaseGeom *FeatureViewPart::getCompleteEdge(int idx) const
{
   //## Get the Part Link ##/
    App::DocumentObject* link = Source.getValue();

    if (!link || !link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return 0;

    const Part::TopoShape &topoShape = static_cast<Part::Feature*>(link)->Shape.getShape();
    std::stringstream str;
    str << "Edge" << idx;
    const TopoDS_Shape shape = topoShape.getSubShape(str.str().c_str());

    const TopoDS_Shape &support = static_cast<Part::Feature*>(link)->Shape.getValue();
    DrawingGeometry::BaseGeom *prjShape = geometryObject->projectEdge(shape, support, Direction.getValue());

    return prjShape;
}

DrawingGeometry::Vertex * FeatureViewPart::getVertex(int idx) const
{
   //## Get the Part Link ##/
    App::DocumentObject* link = Source.getValue();

    if (!link || !link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return 0;

    const Part::TopoShape &topoShape = static_cast<Part::Feature*>(link)->Shape.getShape();
    std::stringstream str;
    str << "Vertex" << idx;
    TopoDS_Shape shape = topoShape.getSubShape(str.str().c_str());

    const TopoDS_Shape &support = static_cast<Part::Feature*>(link)->Shape.getValue();
    DrawingGeometry::Vertex *prjShape = geometryObject->projectVertex(shape, support, Direction.getValue());
    Base::Console().Log("vert %f, %f \n", prjShape->pnt.fX,  prjShape->pnt.fY);
    return prjShape;
}

App::DocumentObjectExecReturn *FeatureViewPart::execute(void)
{
    //## Get the Part Link ##/
    App::DocumentObject* link = Source.getValue();

    Base::Console().Log("execute view feat");
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");

    if (!link->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
        return new App::DocumentObjectExecReturn("Linked object is not a Part object");

    TopoDS_Shape shape = static_cast<Part::Feature*>(link)->Shape.getShape()._Shape;
    if (shape.IsNull())
        return new App::DocumentObjectExecReturn("Linked shape object is empty");

    try {
        geometryObject->setTolerance(Tolerance.getValue());
        geometryObject->setScale(Scale.getValue());
        geometryObject->extractGeometry(shape, Direction.getValue(), ShowHiddenLines.getValue(), XAxisDirection.getValue());

        this->touch();
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }
}

// Python Drawing feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Drawing::FeatureViewPartPython, Drawing::FeatureViewPart)
template<> const char* Drawing::FeatureViewPartPython::getViewProviderName(void) const {
    return "DrawingGui::ViewProviderDrawingView";
}
/// @endcond

// explicit template instantiation
template class DrawingExport FeaturePythonT<Drawing::FeatureViewPart>;
}
