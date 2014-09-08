// class PolyhedronModel represents a polyhedral model presented in a
// PolyhedronControl.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector3.h"
#include "Polyhedron.h"
#include "Polygon.h"
#include "Matrix4x4.h"


class PolyhedronModel
{
// Attributes
public:
	Vector3 tlf,trf,blf,brf,tlb,trb,blb,brb;	// 8 exterior vertices
	Polyhedron m_box;		// the polyhedron itself in object space
	BOOL m_bFaceSelected;	// true if there is a defined "current" face
							//  (it will be last one in m_box arrays)
	Vector3 m_Up;			// indicates "up" direction for display

	Polyhedron* GetBoxPtr () { return &m_box; }
	Vector3 GetUpVector () { return m_Up; }
	void SetUpVector (const Vector3& up) { m_Up = up; }
	double GetDiagonal ();
	int GetSelectedFace ()
		{ return (m_bFaceSelected) ? m_box.GetFaceCount()-1 : -1; }
	BOOL isValid () { return m_bValid; }

// Constructors
public:
	PolyhedronModel() { m_bValid = FALSE; m_bFaceSelected = FALSE; }

// Operations
public:
	// initialize to 6-sided box with given dimensions
	void Initialize (double width, double height, double depth);
	// reset to 6-sided box with previously-given dimensions
	void Reinitialize ();
	// select new current face
	void SelectFace (int face);
	// create and select new face, same as given face
	void CloneFace (int face);
	// compute intersection of given plane w/ current box
	CPolygon ComputePoly (const Plane& plane);
	// recompute all face polygons following a change to selected face
	void RecomputePolygons ();
	// translate currently selected face to new D value
	void TranslateSelectedFace (const double newD);
	// rotate selected face by given quaternion about given point
	void RotateSelectedFace (const Matrix4x4& R, const Vector3& pivot);

// Overridables

// Implementation
protected:
	BOOL m_bValid;

};
