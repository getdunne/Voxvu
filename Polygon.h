// class CPolygon represents a polygon in 3-space
//
// Class name is "CPolygon" to avoid conflict with class "Polygon" defined
// somewhere in the AFX headers 
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

#include "afxtempl.h"
typedef CArray <Vector3,Vector3&> CVertexArray;

class CPolygon
{
// attributes
public:
	CVertexArray v;

	int GetVertexCount () const { return v.GetSize(); }
	void SetVertexCount (int n) { v.SetSize(n); }

// constructors
public:
	CPolygon (int n=0) { v.SetSize(n); }
	CPolygon (const CPolygon& p);
	CPolygon& operator = (const CPolygon&);

// operations
public:
	// return a transformed version of this polygon
	CPolygon Transform (const Matrix4x4& M);
	// 2D point-in-polygon test
	BOOL ContainsPoint2D (const CPoint point);
};
