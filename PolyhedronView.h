#pragma once

#include "VoxvuView.h"
#include "PolyhedronControl.h"
#include "ButtonControl.h"
#include "RibbonControl.h"
#include "CrosshairsControl.h"
#include "PolyhedronRenderer.h"


class PolyhedronView : public VoxvuView
{
protected:
	PolyhedronView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(PolyhedronView)

// Attributes
public:
	// model
	PolyhedronModel m_model;
	PolyhedronRenderer* m_pRenderer;

	// controls
	PolyhedronControl m_polyCtrl;	// main control in center
	CrosshairsControl m_wlCrosshairs;	// overlaid window/level crosshairs
	ButtonControl m_buttonTL;	// button at top-left corner (restore original box bounds)
	ButtonControl m_buttonTR;	// button at top-right corner (restore default orientation)
	ButtonControl m_buttonBL;	// button at top-left corner (unused)
	ButtonControl m_buttonBR;	// button at top-right corner (unused)
	RibbonControl m_wheelT;		// ribbon control at top (zoom in/out)
	RibbonControl m_wheelL;		// ribbon control at left (push/pull selected plane)
	RibbonControl m_wheelR;		// ribbon control at right (scroll up/down)
	RibbonControl m_wheelB;		// ribbon control at bottom (scroll left/right)

	// pop-up context menu
	CMenu m_contextMenu;

// Operations
public:

// Overrides
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

// Implementation
protected:
	virtual ~PolyhedronView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnMshMouseWheel(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnContext_WLCrosshair();
	afx_msg void OnContext_ABCircle();
	afx_msg void OnContext_TwoPass();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	DECLARE_MESSAGE_MAP()
};
