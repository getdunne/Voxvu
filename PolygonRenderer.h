// PolygonRenderer.h
//
// class CPolygonRenderer renders a single polygon with solid texturing.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __PolygonRenderer
#define __PolygonRenderer

#include "Renderer.h"
#include "Polygon.h"
#include "PolyFiller.h"
#include "Matrix4x4.h"

class CPolygonRenderer : public CRenderer {

// Attributes
public:
	CPolygon* m_pPoly;
	CPlane* m_pPlane;
	CPolyFiller m_PolyFiller;

	void SetPolygon (CPolygon* pPoly, CPlane* pPlane)
		{ m_pPoly = pPoly; m_pPlane = pPlane; }
	void SetTransformation (const CMatrix4x4& scrnToTex)
		{ m_PolyFiller.m_ScrnToTex = scrnToTex; }

// Constructors
	CPolygonRenderer () { m_pPoly = NULL; m_pPlane = NULL; }

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

#endif
