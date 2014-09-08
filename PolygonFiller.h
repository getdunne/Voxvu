// class PolygonFiller encapsulates a scan-line polgon-filling routine
// with support for solid texturing.  The primary function, Fill(),
// takes only those parameters likely to be different on every call.
// Other parameters are implemented as member variables.  This approach
// avoids enormous stack frames, but requires a bit of care when using
// multiple threads -- each PolygonFiller object should be used by only
// one thread at a time.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Polygon.h"
#include "Plane.h"
#include "FixedPt.h"

// The actual pixel-painting is done by an external "LineProc" with
// signature defined by the following typedef.  The first parameter is
// normally a pointer to some object, typically the "this" pointer of
// some calling object's member function.  NOTE: the LineProc main loop
// should be for (; xl<xr; ++xl), i.e. let X increment up to but not
// including xl.
typedef void (*LineProcPtr) (
	const LPVOID pUserData,	// pointer to caller-supplied data
	const int y,			// scan line number
	const int xl,			// minimum X coordinate
	const int xr,			// one greater than maximum X coordinate
	const double z,			// Z value at point (xl,y)
	const double delta_z,	// change in Z on moving right 1 pixel
	const Vector3 &tex,	// texture (u,v,w) at point (xl,y)
	const Vector3 &dtex	// change in texture on moving right 1 pixel
	);

class PolygonFiller
{
// Attributes
public:
	LineProcPtr m_pLineProc;	// user-supplied LineProc
	Matrix4x4 m_ScrnToTex;		// maps screen to texture coordinates
	CRect m_clipRect;			// clipping rectangle
	BOOL* m_pbContinue;			// points to user's "continue" variable
	LPVOID m_pUserData;			// arbitrary user-supplied pointer

// Constructors
public:
	PolygonFiller ();
	~PolygonFiller ();

	// define max expected polygon size (call at least once before use)
	void Setup (const int nMaxVerts);

// Operations
public:
	// Fill() is the only operation.  Returns TRUE only if polygon-fill
	// operation runs to completion, FALSE if interrupted due to
	// *m_pbContinue going FALSE.
	BOOL Fill (const CPolygon& poly, const Plane& plane);

	// Implementation
private:
	// owned storage for vertex and edge lists
	int m_nMaxVerts;		// size of 2 following workspace arrays
	struct VLE {
		int x,y;			// rounded screen x,y coordinates
		Vector3 scrn;		// screen (x,y,z) coordinates (x, y rounded)
		Vector3 tex;		// texture (u,v,w) coords which go with (x,y,z)
		} *m_pVertices;
	struct ELE {
		Fixed x_int;		// X-intercept; X value which goes with ymin
		Fixed x_inc;		// X change per unit change in Y (inverse slope)
		int ymin;			// minimum Y-coordinate
		int delta_y;		// total Y-extent of this line (ymax-ymin+1)
		double z;			// Z value which goes with (x_int,ymin)
		Vector3 tex;		// texture (u,v,w) at (x_int,ymin,z)
		} *m_pEdges;
	// deltas related to plane parameters
	double m_deltaZx;		// Z change on moving right 1 pixel
	double m_deltaZy;		// Y change on moving down 1 scan line
	Vector3 m_deltaTx;		// UVW change on moving right 1 pixel
	Vector3 m_deltaTy;		// UVW change on moving down 1 scan line
	// helper functions
	void PaintLine(const int y, const int x1, const int x2,
					 const double z1, const double z2,
					 Vector3 tex1, Vector3 tex2);
	void AddEdge (int entry, int p1, int p2, int next_y);
};
