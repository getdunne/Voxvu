#include "OverlayControl.h"
#include "VoxelFilePair.h"

class VoxvuView : public CView
{
protected: // create from serialization only
	VoxvuView();
	DECLARE_DYNCREATE(VoxvuView)

// Attributes
public:
	OverlayControl* m_pChead;	// controls list head (frontmost)
	OverlayControl* m_pCtail;	// controls list tail (rearmost)
	OverlayControl* m_pChit;	// last control hit
	OverlayControl* m_pShiftCtrl;	// control to activate on shift-click
	void SetShiftControl (OverlayControl* pC) { m_pShiftCtrl = pC; }
	BOOL m_bDragging;		// TRUE while dragging

	VoxelFilePair* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VoxvuView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VoxvuView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(VoxvuView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MyView.cpp
inline VoxelFilePair* VoxvuView::GetDocument()
   { return (VoxelFilePair*)m_pDocument; }
#endif
