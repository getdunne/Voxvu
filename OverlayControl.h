// class OverlayControl is a base class for various kinds of "controls"
// which live in "views" of class CMyView.  
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class OverlayControl
{
// Attributes
public:
	CWnd* m_pOwner;		// reference to drawing context i.e. window
						//  in which this control lives.  Defines
						//  coordinate system for...
	CRect m_bbox;		// ... this bounding box

	OverlayControl* m_pPrev;	// to facilitate linked lists of
	OverlayControl* m_pNext;	//  CControl objects

	LPCTSTR m_cursor;	// cursor to use when mouse is over this control
	BOOL m_bVisible;	// no drawing done unless this is TRUE
	BOOL m_bEnabled;	// control won't be activated unless this TRUE

	void SetOwner (CWnd* pView) { m_pOwner = pView; }
	CWnd* GetOwner () { return m_pOwner; }
	void SetBBox (const CRect& rect) { m_bbox = rect; }
	void SetLinks (OverlayControl* prev, OverlayControl* next)
		{ m_pPrev = prev; m_pNext = next; }
	BOOL isVisible () { return m_bVisible; }
	void Show (const BOOL bVis=TRUE) { m_bVisible = bVis; }
	BOOL isEnabled () { return m_bEnabled; }
	void Enable (const BOOL bEna=TRUE) { m_bEnabled = bEna; }
	void SetCursor (LPCTSTR cur) { m_cursor = cur; }
	LPCTSTR GetCursor () { return m_cursor; }

// Constructors
	OverlayControl () : m_cursor(IDC_ARROW), m_bVisible(FALSE),
		m_bEnabled(FALSE), m_bbox(CRect(0,0,0,0)) {}

// Operations
	virtual void Draw (CDC* pDC) {}
	virtual BOOL Hit (const CPoint mouse) { return m_bbox.PtInRect(mouse); }
	virtual void Click (UINT nFlags, CPoint point) {}
	virtual void Drag (UINT nFlags, CPoint point) {}
	virtual void Release (UINT nFlags, CPoint point) {}
	virtual void DoubleClick (UINT nFlags, CPoint point) {}
};
