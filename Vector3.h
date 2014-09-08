// class Vector3 represents a 3-element vector
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix4x4.h"

class Vector3
{
// attributes
public:
	double x,y,z;

	// get vector length
	double Length () const;

// constructors
public:
	Vector3 () : x(0.0), y(0.0), z(0.0) {}
	Vector3 (const double xx, const double yy, const double zz)
		{ x = xx; y = yy; z = zz; }

// operations
public:
	// accumulating standard math operators
	void operator *= (const double s) { x *= s; y *= s; z *= s; }
	void operator /= (const double s);
	void operator += (const Vector3& v) { x += v.x; y += v.y; z += v.z; }
	void operator -= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; }

	// normalize to unit length
	void Normalize ();

	// dot and cross products with a second vector
	double Dot (const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
	Vector3 Cross (const Vector3& v) const;

	// (row) vector-by-matrix multiplications
	Vector3 Hom (const Matrix4x4& M) const;	// homogeneous (4x4)
	Vector3 Aff (const Matrix4x4& M) const;	// affine (4x3)
	Vector3 Lin (const Matrix4x4& M) const;	// linear (3x3)
};


/////////////////////////////////////////////////////////////////////////////
// Overloaded math operators using/returning Vector3 objects

// equality test
inline BOOL operator == (const Vector3& u, const Vector3& v)
	{ return u.x==v.x && u.y==v.y && u.z==v.z; }

// vector addition
inline Vector3 operator + (const Vector3& u, const Vector3& v)
	{ return Vector3(u.x+v.x, u.y+v.y, u.z+v.z); }

// vector subtraction
inline Vector3 operator - (const Vector3& u, const Vector3& v)
	{ return Vector3(u.x-v.x, u.y-v.y, u.z-v.z); }

// unary negation
inline Vector3 operator - (const Vector3& v)
	{ return Vector3(-v.x, -v.y, -v.z); }

// vector by scalar multiplication (either order)
inline Vector3 operator * (const Vector3& v, const double s)
	{ return Vector3(v.x*s, v.y*s, v.z*s); }
inline Vector3 operator * (const double s, const Vector3& v)
	{ return Vector3(v.x*s, v.y*s, v.z*s); }
