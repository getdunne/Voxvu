// class Matrix4x4 represents a 4x4 matrix
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class Matrix4x4
{
// attributes
public:
	double	a,b,c,d,
			e,f,g,h,
			i,j,k,l,
			m,n,o,p;

// constructors
public:
	// default constructor: does nothing
	Matrix4x4 () {};

	// create various kinds of transformation matrices
	void MakeIdentity ();
	void MakeDilation (const double sx, const double sy, const double z);
	void MakeDilation (const double s) { MakeDilation(s,s,s); }
	void MakeTranslation (const double dx, const double dy, const double dz);
	void MakeDilTrans (const double sx, const double sy, const double sz,
					   const double dx, const double dy, const double dz);

// operations
public:
	Matrix4x4 Transpose ();
};


/////////////////////////////////////////////////////////////////////////////
// Overloaded math operators using/returning Matrix4x4 objects

// matrix multiplication
Matrix4x4 operator * (const Matrix4x4& M1, const Matrix4x4& M2);
