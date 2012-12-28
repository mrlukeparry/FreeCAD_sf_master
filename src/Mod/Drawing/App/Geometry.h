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
   
   GeomType geomType;
   ExtractionType extractType;   
};

class DrawingExport Circle: public BaseGeom
{
public:
  Circle(const BRepAdaptor_Curve &c);
  ~Circle() {}
  
  double radius;
  double x;
  double y;
};

class DrawingExport Ellipse: public BaseGeom
{
public:
  Ellipse(const BRepAdaptor_Curve &c);
  ~Ellipse() {}
  
  double radius;
  double x;
  double y;
  double minor;
  double major;
};

class DrawingExport AOE: public Ellipse
{
public:
  AOE(const BRepAdaptor_Curve &c);
  ~AOE() {}
  
  double startAngle;
  double endAngle;
};

class DrawingExport AOC: public Circle
{
public:
  AOC(const BRepAdaptor_Curve &c);
  ~AOC() {}

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
  ~BSpline() {}  
  std::vector<BezierSegment> segments;
};

class Generic: public BaseGeom
{
public:
  Generic(const BRepAdaptor_Curve& c);
  ~Generic() {}
  std::vector<Base::Vector2D> points;
  
};

} 

#endif //DRAWING_GEOMETRY_H