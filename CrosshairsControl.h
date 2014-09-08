// class CrosshairsControl represents a control which allows a small marker box,
// shown at the intersection of vertical and horizontal "crosshairs" lines,
// to be dragged to any location within its bounding rectangle.  Its
// value at any time is a 2-element vector (fx,fy), being the marker
// coordinates normalized relative to the bounding rectangle.  Such
// a control will normally be overlaid onto some other control.
// Drag actions clicks are passed on the the owner via a callback.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "OverlayControl.h"

class CrosshairsControl : public OverlayControl
{
// Attributes
public:
	COLORREF m_color;
	void SetColor (const COLORREF color) { m_color = color; }

	double m_fx,m_fy;
	void GetValue (double* pfx, double* pfy) { *pfx = m_fx; *pfy = m_fy; }
	void SetValue (double fx, double fy) { m_fx = fx; m_fy = fy; }

	typedef void (*DragProcPtr)(CWnd*,UINT,double,double);
	DragProcPtr m_pDragProc;
	void SetDragProc (DragProcPtr pDP) { m_pDragProc = pDP; }

	typedef void (*ReleaseProcPtr)(CWnd*,UINT,CPoint);
	ReleaseProcPtr m_pReleaseProc;
	void SetReleaseProc (ReleaseProcPtr pRP) { m_pReleaseProc = pRP; }

// Constructors
public:
	CrosshairsControl ();

// Overrides
public:
	virtual void Draw (CDC* pDC);
	virtual BOOL Hit (const CPoint mouse);
	virtual void Click (UINT nFlags, CPoint point);
	virtual void Drag (UINT nFlags, CPoint point);
	virtual void Release (UINT nFlags, CPoint point);

// Implementation
protected:
	CPoint m_lastMouse;
};
