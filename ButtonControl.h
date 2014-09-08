// class ButtonControl represents a simple button control.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "OverlayControl.h"

class ButtonControl : public OverlayControl
{
// Attributes
public:
	CString m_label;
	COLORREF m_color;
	void SetLabel (const CString& label) { m_label = label; }
	void SetColor (const COLORREF color) { m_color = color; }

	typedef void (*HitProcPtr)(CWnd*,ButtonControl*);
	HitProcPtr m_pHitProc;
	void SetHitProc (HitProcPtr pHit) { m_pHitProc = pHit; }

// Constructors
public:
	ButtonControl ();

// Operations
public:
	virtual void Draw (CDC* pDC)
		{ if (m_bPressed) DrawPressed(pDC); else DrawNormal(pDC); }
	virtual void Click (UINT nFlags, CPoint point);
	virtual void Drag (UINT nFlags, CPoint point);
	virtual void Release (UINT nFlags, CPoint point);

// Implementation
protected:
	BOOL m_bPressed;
	void DrawNormal (CDC* pDC);
	void DrawPressed (CDC* pDC);
};
