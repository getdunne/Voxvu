#include "stdafx.h"

#include <math.h>
#include "Vector3.h"


//--------------------------------------------------------------
// Return length of this vector
//--------------------------------------------------------------
double Vector3::Length () const
{
	return sqrt(x*x + y*y + z*z);
}

//--------------------------------------------------------------
// Normalize a vector to unit length
//--------------------------------------------------------------
void Vector3::Normalize ()
{
	ASSERT(Length() != 0.0);

	double s = 1.0 / Length();
	x *= s; y *= s; z *= s;
}

//--------------------------------------------------------------
// Accumulating division by a scalar
//--------------------------------------------------------------
void Vector3::operator /= (const double s)
{
	ASSERT(s != 0.0);

	x /= s; y /= s; z /= s;
}

//--------------------------------------------------------------
// Compute the cross product of this vector with a second vector.
//--------------------------------------------------------------
Vector3 Vector3::Cross (const Vector3& v) const
{
	Vector3 tmp;
	
	tmp.x = y * v.z - z * v.y;
	tmp.y = z * v.x - x * v.z;
	tmp.z = x * v.y - y * v.x;
	return tmp;
}

//--------------------------------------------------------------
// Homogeneous (row) vector-by-matrix multiplication.
// The source vector is in effect extended to homogeneous form
// by appending 1.0 as a fourth coordinate.
//--------------------------------------------------------------
Vector3 Vector3::Hom (const Matrix4x4& M) const
{
	double s;
	Vector3 v;

	ASSERT((x*M.d + y*M.h + z*M.l + M.p) != 0.0);

	s = 1.0 / (x*M.d + y*M.h + z*M.l + M.p);
	v.x = s * (x*M.a + y*M.e + z*M.i + M.m);
	v.y = s * (x*M.b + y*M.f + z*M.j + M.n);
	v.z = s * (x*M.c + y*M.g + z*M.k + M.o);
	return v;
}


//--------------------------------------------------------------
// Affine (row) vector-by-matrix multiplication.
// Handles the case where the homogeneous scale factor (s in
// Vector3::X4x4() above) would be unity.
//--------------------------------------------------------------
Vector3 Vector3::Aff (const Matrix4x4& M) const
{
	Vector3 v;

	ASSERT(M.d==0.0 && M.h==0.0 && M.l==0.0 && M.p==1.0);

	v.x = x*M.a + y*M.e + z*M.i + M.m;
	v.y = x*M.b + y*M.f + z*M.j + M.n;
	v.z = x*M.c + y*M.g + z*M.k + M.o;
	return v;
}


//--------------------------------------------------------------
// Linear (row) vector-by-matrix multiplication.
// This allows postmultiplying a 3-vector by the upper left 3x3
// submatrix of a 4x4 matrix.  Use this when you want to ignore
// the translation component of a transformation, e.g. when
// applying the transformation to displacement vectors instead
// of points in 3-space.
//--------------------------------------------------------------
Vector3 Vector3::Lin (const Matrix4x4& M) const
{
	Vector3 v;

	v.x = x*M.a + y*M.e + z*M.i;
	v.y = x*M.b + y*M.f + z*M.j;
	v.z = x*M.c + y*M.g + z*M.k;
	return v;
}
