// class Quaternion represents a (unit) quaternion
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

class Quaternion
{
// attributes
public:
	Vector3 V;
	double s;


// constructors
public:
	// default constructor creates a "no rotation" quaternion
	Quaternion () : s(1.0) {}
	Quaternion (const Vector3 &v, const double ss) { V = v; s = ss; }
	// this form of constructor takes 3 Euler angles in degrees
	Quaternion (const double ex, const double ey, const double ez);

// operations
public:
	// return a 4x4 rotation matrix according to this quaternion
	Matrix4x4 RotMatrix() const;

	// multiply by a second quaternion
	Quaternion operator * (const Quaternion& q) const;
};


/////////////////////////////////////////////////////////////////////////////
// Overloaded math operators using/returning Quaternion objects

// equality test
inline BOOL operator == (const Quaternion& q1, const Quaternion& q2)
	{ return q1.V==q2.V && q1.s==q2.s; }
