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

#ifndef DRAWING_GEOMETRY_H
#define DRAWING_GEOMETRY_H

#include <Base/Tools2D.h>
class BRepAdaptor_Curve;

namespace DrawingGeometry {

enum ExtractionType {
      Plain = 0,
      WithHidden = 1,
      WithSmooth = 2
};

enum GeomType {
    CIRCLE,
    ARCOFCIRCLE,
    ELLIPSE,
    ARCOFELLIPSE,
    BSPLINE,
    GENERIC
};

class DrawingExport BaseGeom
{
public:
   BaseGeom() {}
   ~BaseGeom() {}
public:
   GeomType geomType;
   ExtractionType extractType;
};

class DrawingExport Circle: public BaseGeom
{
public:
  Circle(const BRepAdaptor_Curve &c);
  ~Circle() {}
public:
  double radius;
  double x;
  double y;
};

class DrawingExport Ellipse: public BaseGeom
{
public:
  Ellipse(const BRepAdaptor_Curve &c);
  ~Ellipse() {}
public:
  double radius;
  double x;
  double y;
  double minor;
  double major;
  double angle;
};

class DrawingExport AOE: public Ellipse
{
public:
  AOE(const BRepAdaptor_Curve &c);
  ~AOE() {}
public:
  double startAngle;
  double endAngle;
};

class DrawingExport AOC: public Circle
{
public:
  AOC(const BRepAdaptor_Curve &c);
  ~AOC() {}
public:
  double startAngle;
  double endAngle;
};

struct BezierSegment
{
   int poles;
   Base::Vector2D pnts[4];
};

class DrawingExport BSpline: public BaseGeom
{
public:
  BSpline(const BRepAdaptor_Curve &c);
  ~BSpline(){}
public:
  std::vector<BezierSegment> segments;
};

class Generic: public BaseGeom
{
public:
  Generic(const BRepAdaptor_Curve& c);
  ~Generic() {}
  std::vector<Base::Vector2D> points;

};

/// Simple Collection of geometric features based on BaseGeom inherited classes in order
struct DrawingExport Wire
{
  Wire();
  ~Wire();
  std::vector<BaseGeom *> geoms;
};

/// Simple Collection of geometric features based on BaseGeom inherited classes in order
struct DrawingExport Face
{
  Face();
  ~Face();
  std::vector<Wire *> wires;
};

}

#endif //DRAWING_GEOMETRY_H