// Circle.h
// Copyright 2011, Dan Heeks
// This program is released under the New BSD license. See the file COPYING for details.

#pragma once

#include <PreCompiled.h>

#include "Point.h"

namespace area
{

class LibAreaExport Circle{
public:
	Point m_c;
	double m_radius;

	Circle(const Point& c, double radius):m_c(c), m_radius(radius){}
	Circle(const Point& p0, const Point& p1, const Point& p2); // circle through three points
	
	bool PointIsOn(const Point& p, double accuracy);
	bool LineIsOn(const Point& p0, const Point& p1, double accuracy);
};

} // End namespace area

