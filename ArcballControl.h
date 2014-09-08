// class ArcballControl represents an "arcball" 3D rotation control according
// to Ken Shoemake.  This control has no state; it passes incremental
// rotation quaternions to its owner via its "drag" callback.  An arcball
// control will normally be overlaid onto some other control.  Double
// clicks are passed on the the owner via the "dblclick" callback.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "OverlayControl.h"
#include "Quaternion.h"

class ArcballControl : public OverlayControl
{
// Attributes
public:
	COLORREF m_color;
	void SetColor (const COLORREF color) { m_color = color; }

	CPoint m_center;
	double m_radius;

	typedef void (*DragProcPtr)(CWnd*,UINT,Quaternion&);
	DragProcPtr m_pDragProc;
	void SetDragProc (DragProcPtr pDP) { m_pDragProc = pDP; }

	typedef void (*DblClickProcPtr)(CWnd*,UINT,CPoint);
	DblClickProcPtr m_pDblClickProc;
	void SetDblClickProc (DblClickProcPtr pDCP) { m_pDblClickProc = pDCP; }

	typedef void (*ReleaseProcPtr)(CWnd*,UINT,CPoint);
	ReleaseProcPtr m_pReleaseProc;
	void SetReleaseProc (ReleaseProcPtr pRP) { m_pReleaseProc = pRP; }

// Constructors
public:
	ArcballControl ();

// Overrides
public:
	void SetBBox (const CRect& rect);
	virtual void Draw (CDC* pDC);
	virtual void Click (UINT nFlags, CPoint point);
	virtual void Drag (UINT nFlags, CPoint point);
	virtual void Release (UINT nFlags, CPoint point);
	virtual void DoubleClick (UINT nFlags, CPoint point);

// Implementation
protected:
	CPoint m_lastMouse;
	void ArcballTrans(CPoint point, Vector3* result);
};
