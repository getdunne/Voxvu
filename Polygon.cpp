#include "stdafx.h"

#include "Polygon.h"


//--------------------------------------------------------------
// copy constructor
//--------------------------------------------------------------
CPolygon::CPolygon (const CPolygon& p)
{
	int n = p.v.GetSize();
	v.SetSize(n);
	for (int i=0; i < n; ++i) v[i] = p.v[i];
}

//--------------------------------------------------------------
// assignment operator
//--------------------------------------------------------------
CPolygon& CPolygon::operator = (const CPolygon& p)
{
	int n = p.v.GetSize();
	v.SetSize(n);
	for (int i=0; i < n; ++i) v[i] = p.v[i];
	return *this;
}

//--------------------------------------------------------------
// return a transformed version of this polygon
//--------------------------------------------------------------
CPolygon CPolygon::Transform (const Matrix4x4& M)
{
	int n = GetVertexCount();
	CPolygon out(n);
	for (int i=0; i < n; ++i) out.v[i] = v[i].Aff(M);
	return out;
}

//--------------------------------------------------------------
// 2D point-in-polygon test.  This is a simple test which works
// only for convex polygons, and assumes polygon vertices are
// sorted in counter-clockwise order.
//--------------------------------------------------------------
BOOL CPolygon::ContainsPoint2D (const CPoint point)
{
	Vector3 vp;
	vp.x = point.x;
	vp.y = point.y;
	vp.z = 0.;
	BOOL inside=true;
	int iMinusOne=GetVertexCount()-1;
	for (int i=0; i < GetVertexCount(); ++i) {
		Vector3 a = v[i] - v[iMinusOne];
		Vector3 b = vp - v[iMinusOne];
		if (a.x*b.y - a.y*b.x < 0.) {
			inside = false;
			break;
		}
		iMinusOne = i;
	}
	return inside;
}
