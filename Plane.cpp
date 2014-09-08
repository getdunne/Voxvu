#include "stdafx.h"

#include "Plane.h"


//--------------------------------------------------------------
// assignment operator
//--------------------------------------------------------------
Plane& Plane::operator = (const Plane& p)
{
	n = p.n;
	d = p.d;
	return *this;
}


//--------------------------------------------------------------
// return a transformed version of this plane
//--------------------------------------------------------------
Plane Plane::Transform (const Matrix4x4& M)
{
	Plane out;
	out.n = n.Lin(M);
	out.n.Normalize();
	out.d = (n*d).Aff(M).Dot(out.n);
	return out;
}

