// Circle.cpp
// Copyright 2011, Dan Heeks
// This program is released under the New BSD license. See the file COPYING for details.

#include "Circle.h"

Circle::Circle(const Point& p0, const Point& p1, const Point& p2)
{
	// from TangentCircles in http://code.google.com/p/heekscad/source/browse/trunk/src/Geom.cpp

	// set default values, in case this fails
	m_radius = 0.0;
	m_c = Point(0, 0);

	double x1 = p0.x;
	double y1 = p0.y;
	double x2 = p1.x;
	double y2 = p1.y;
	double x3 = p2.x;
	double y3 = p2.y;

	double a = 2 * (x1 - x2);
	double b = 2 * (y1 - y2);
	double d = (x1 * x1 + y1 * y1) - (x2 * x2 + y2 * y2);

	double A = 2 * (x1 - x3);
	double B = 2 * (y1 - y3);
	double D = (x1 * x1 + y1 * y1) - (x3 * x3 + y3 * y3);

	double aBmbA = (a*B - b*A); // aB - bA

	// x = k + Kr where
	double k = (B*d - b*D) / aBmbA;

	// y = l + Lr where
	double l = (-A*d + a*D)/ aBmbA;

	double qa = -1;
	double qb = 0.0;
	double qc = k*k + x1*x1 -2*k*x1 + l*l + y1*y1 - 2*l*y1;

	// solve the quadratic equation, r = (-b +- sqrt(b*b - 4*a*c))/(2 * a)
	for(int qs = 0; qs<2; qs++){
		double bb = qb*qb;
		double ac4 = 4*qa*qc;
		if(ac4 <= bb){
			double r = (-qb + ((qs == 0) ? 1 : -1) * sqrt(bb - ac4))/(2 * qa);
			double x = k;
			double y = l;

			// set the circle
			if(r >= 0.0){
				m_c = Point(x, y);
				m_radius = r;
			}
		}
	}
}

bool Circle::PointIsOn(const Point& p, double accuracy)
{
	double rp = p.dist(m_c);
	bool on = fabs(m_radius - rp) < (2.0 * accuracy);	// Each of the two points can be 'accuracy' away from where it really is.
	return on;
}

bool Circle::LineIsOn(const Point& p0, const Point& p1, double accuracy)
{
	// checks the points are on the arc, to the given accuracy, and the mid point of the line.

	if(!PointIsOn(p0, accuracy))return false;
	if(!PointIsOn(p1, accuracy))return false;

	Point mid = Point((p0 + p1)/2);
	if(!PointIsOn(mid, accuracy))return false;

	return true;
}