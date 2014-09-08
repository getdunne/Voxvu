#include "stdafx.h"

#include "Matrix4x4.h"

//--------------------------------------------------------------
// Establish identity
//--------------------------------------------------------------
void Matrix4x4::MakeIdentity ()
{
	a = 1.0; b = 0.0; c = 0.0; d = 0.0;
	e = 0.0; f = 1.0; g = 0.0; h = 0.0;
	i = 0.0; j = 0.0; k = 1.0; l = 0.0;
	m = 0.0; n = 0.0; o = 0.0; p = 1.0;
}

//--------------------------------------------------------------
// Establish anisotropic dilation
//--------------------------------------------------------------
void Matrix4x4::MakeDilation (const double sx, const double sy, const double sz)
{
	a = sx;  b = 0.0; c = 0.0; d = 0.0;
	e = 0.0; f = sy;  g = 0.0; h = 0.0;
	i = 0.0; j = 0.0; k = sz;  l = 0.0;
	m = 0.0; n = 0.0; o = 0.0; p = 1.0;
}

//--------------------------------------------------------------
// Establish translation
//--------------------------------------------------------------
void Matrix4x4::MakeTranslation (const double dx, const double dy, const double dz)
{
	a = 1.0; b = 0.0; c = 0.0; d = 0.0;
	e = 0.0; f = 1.0; g = 0.0; h = 0.0;
	i = 0.0; j = 0.0; k = 1.0; l = 0.0;
	m = dx;  n = dy;  o = dz;  p = 1.0;
}

//--------------------------------------------------------------
// Establish simultaneous dilation and translation
//--------------------------------------------------------------
void Matrix4x4::MakeDilTrans (
	const double sx, const double sy, const double sz,
	const double dx, const double dy, const double dz
	)
{
	a = sx;  b = 0.0; c = 0.0; d = 0.0;
	e = 0.0; f = sy;  g = 0.0; h = 0.0;
	i = 0.0; j = 0.0; k = sz;  l = 0.0;
	m = dx;  n = dy;  o = dz;  p = 1.0;
}

//--------------------------------------------------------------
// Transpose rows and columns
//--------------------------------------------------------------
Matrix4x4 Matrix4x4::Transpose ()
{
	Matrix4x4 tmp;
	tmp.a = a; tmp.b = e; tmp.c = i; tmp.d = m;
	tmp.e = b; tmp.f = f; tmp.g = j; tmp.h = n;
	tmp.i = c; tmp.j = g; tmp.k = k; tmp.l = o;
	tmp.m = d; tmp.n = h; tmp.o = l; tmp.p = p;
	return tmp;
}


/////////////////////////////////////////////////////////////////////////////
// Overloaded math operators using/returning Matrix4x4 objects

// matrix multiplication
Matrix4x4 operator * (const Matrix4x4& M1, const Matrix4x4& M2)
{
	Matrix4x4 P;
	
	P.a = M1.a*M2.a + M1.b*M2.e + M1.c*M2.i + M1.d*M2.m;
	P.b = M1.a*M2.b + M1.b*M2.f + M1.c*M2.j + M1.d*M2.n;
	P.c = M1.a*M2.c + M1.b*M2.g + M1.c*M2.k + M1.d*M2.o;
	P.d = M1.a*M2.d + M1.b*M2.h + M1.c*M2.l + M1.d*M2.p;
	P.e = M1.e*M2.a + M1.f*M2.e + M1.g*M2.i + M1.h*M2.m;
	P.f = M1.e*M2.b + M1.f*M2.f + M1.g*M2.j + M1.h*M2.n;
	P.g = M1.e*M2.c + M1.f*M2.g + M1.g*M2.k + M1.h*M2.o;
	P.h = M1.e*M2.d + M1.f*M2.h + M1.g*M2.l + M1.h*M2.p;
	P.i = M1.i*M2.a + M1.j*M2.e + M1.k*M2.i + M1.l*M2.m;
	P.j = M1.i*M2.b + M1.j*M2.f + M1.k*M2.j + M1.l*M2.n;
	P.k = M1.i*M2.c + M1.j*M2.g + M1.k*M2.k + M1.l*M2.o;
	P.l = M1.i*M2.d + M1.j*M2.h + M1.k*M2.l + M1.l*M2.p;
	P.m = M1.m*M2.a + M1.n*M2.e + M1.o*M2.i + M1.p*M2.m;
	P.n = M1.m*M2.b + M1.n*M2.f + M1.o*M2.j + M1.p*M2.n;
	P.o = M1.m*M2.c + M1.n*M2.g + M1.o*M2.k + M1.p*M2.o;
	P.p = M1.m*M2.d + M1.n*M2.h + M1.o*M2.l + M1.p*M2.p;
	
	return P;
}
