#include "stdafx.h"

#include "Polyhedron.h"


//--------------------------------------------------------------
// copy constructor
//--------------------------------------------------------------
Polyhedron::Polyhedron (const Polyhedron& p)
{
	int n = p.plane.GetSize();
	plane.SetSize(n);
	poly.SetSize(n);
	for (int i=0; i < n; ++i) {
		plane[i] = p.plane[i];
		poly[i] = p.poly[i];
	}
}

//--------------------------------------------------------------
// return a transformed version of this polyhedron
//--------------------------------------------------------------
void Polyhedron::Transform (const Matrix4x4& M, Polyhedron* result)
{
	int n = GetFaceCount();
	result->SetFaceCount(n);
	for (int i=0; i < n; ++i) {
		result->plane[i] = plane[i].Transform(M);
		result->poly[i] = poly[i].Transform(M);
	}
}

//--------------------------------------------------------------
// add a new face
//--------------------------------------------------------------
void Polyhedron::AddFace (Plane& pln, CPolygon& pol)
{
	int n = GetFaceCount();
	SetFaceCount(n+1);
	plane[n] = pln;
	poly[n] = pol;
}

//--------------------------------------------------------------
// swap 2 entries in the arrays
//--------------------------------------------------------------
void Polyhedron::SwapFaces (int i1, int i2)
{
	ASSERT(i1>=0 && i1<GetFaceCount());
	ASSERT(i2>=0 && i2<GetFaceCount());
	if (i1 == i2) return;

	Plane tempPlane(plane[i1]);
	CPolygon tempPoly(poly[i1]);
	plane[i1] = plane[i2];
	poly[i1] = poly[i2];
	plane[i2] = tempPlane;
	poly[i2] = tempPoly;
}

