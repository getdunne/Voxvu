// class PolyhedronRenderer renders a polyhedron with solid texturing.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Renderer.h"
#include "Polyhedron.h"
#include "PolygonFiller.h"
#include "Matrix4x4.h"

class PolyhedronRenderer : public Renderer
{
// Attributes
public:
	Polyhedron* m_pPoly;
	void SetPolyhedron (Polyhedron* pPoly)	{ m_pPoly = pPoly; }
	int m_toDraw;	// set to 0-based index to draw just one polygon
					//  -1 to draw all visible polygons
	void SetPolyToDraw (int n) { m_toDraw = n; }

	PolygonFiller m_PolyFiller;

	void SetTransformation (const Matrix4x4& scrnToTex)
		{ m_PolyFiller.m_ScrnToTex = scrnToTex; }

// Constructor
	PolyhedronRenderer () { m_pPoly = NULL; m_toDraw = -1; }

// Overrides
public:
	virtual void RenderQuick ();
	virtual BOOL RenderBest (BOOL* pbContinue);

// Overridables (pure virtual)
public:
	virtual void SetWindowLevel (double fw, double fl) = 0;
	virtual LineProcPtr GetFastLineProcPtr () = 0;
	virtual LineProcPtr GetBestLineProcPtr () = 0;
};
