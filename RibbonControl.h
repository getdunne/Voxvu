// class RibbonControl represents a long, skinny control which is meant to be
// placed along one edge of a window.  It is a continuous controller for a
// variable of type double, without inherent limits like a slider or scrollbar.
// It was inspired by (and is a "poor man's" version of) the thumbwheel controls
// in Silicon Graphics Inc's IRIX Interactive Desktop.
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OverlayControl.h"

class RibbonControl : public OverlayControl
{
// Attributes
public:
	COLORREF m_color;	// background color
	void SetColor (const COLORREF color) { m_color = color; }
	double m_value;		// numeric value
	void SetValue (const double val) { m_value = val; }
	double GetValue (void) { return m_value; }
	double m_sens;		// drag sensitivity
	void SetSens (const double sens) { m_sens = sens; }

	typedef void (*AdjProcPtr)(CWnd*,double);
	AdjProcPtr m_pAdjust;
	void SetAdjProc (AdjProcPtr pAdj) { m_pAdjust = pAdj; }

	typedef void (*ReleaseProcPtr)(CWnd*,UINT,CPoint);
	ReleaseProcPtr m_pReleaseProc;
	void SetReleaseProc (ReleaseProcPtr pRP) { m_pReleaseProc = pRP; }

// Constructors
public:
	RibbonControl ();

// Overridables
public:
	virtual void Draw (CDC* pDC);
	virtual BOOL Hit (const CPoint mouse);
	virtual void Click (UINT nFlags, CPoint point);
	virtual void Drag (UINT nFlags, CPoint point);
	virtual void Release (UINT nFlags, CPoint point);
	virtual void DoubleClick (UINT nFlags, CPoint point);

// Implementation
protected:
	CPoint m_lastMouse;
	int m_phase;
	void DrawIndicator (CDC* pDC);
};
