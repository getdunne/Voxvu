// class Polyhedron represents a polyhedron, or simply a list of
// (possibly intersecting) polygons in 3-space.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix4x4.h"
#include "Plane.h"
#include "Polygon.h"

#include "afxtempl.h"
typedef CArray <CPolygon,CPolygon&> CCPolygonArray;
typedef CArray <Plane,Plane&> CPlaneArray;

class Polyhedron
{
// attributes
public:
	// parallel arrays, one element per polygon
	CPlaneArray plane;	// plane parameters
	CCPolygonArray poly;	// vertices

	int GetFaceCount () { return plane.GetSize(); }
	void SetFaceCount (int n) { plane.SetSize(n); poly.SetSize(n); }

// constructors
public:
	Polyhedron (int n=0) { plane.SetSize(n); poly.SetSize(n); }
	Polyhedron (const Polyhedron& p);

// operations
public:
	// transform this polyhedron
	void Transform (const Matrix4x4& M, Polyhedron* result);
	// add a new face
	void AddFace (Plane& pln, CPolygon& pol);
	// swap 2 entries in the arrays
	void SwapFaces (int i1, int i2);
};
