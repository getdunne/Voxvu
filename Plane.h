// class Plane represents a mathematical plane in 3-space
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

class Plane
{
// attributes
public:
	Vector3 n;		// unit normal vector
	double d;		// perpendicular distance to coordinate origin


// constructors
public:
	// default constructor creates a plane congruent with XY axis
	Plane () : n(0.,0.,1.), d(0.) {}
	Plane (const Vector3& nn, const double dd) { n = nn; d = dd; }
	Plane (const Plane& p) : n(p.n), d(p.d) {}
	Plane& operator = (const Plane&p);

// operations
public:
	// return a transformed version of this plane
	Plane Transform (const Matrix4x4& M);
	// given point coordinates x and y, solve for z on plane
	double ZfromXY (const double x, const double y) const
		{ return (d - n.x*x - n.y*y)/n.z; }
};
