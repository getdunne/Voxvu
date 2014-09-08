#include "stdafx.h"
#include "CrosshairsControl.h"
#include "Macros.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
CrosshairsControl::CrosshairsControl ()
{
	m_color = RGB(0,192,255);	// default color is light blue
	m_fx = m_fy = 0.5;
	m_pDragProc = NULL;
	m_pReleaseProc = NULL;
}

//--------------------------------------------------------------
// Draw
//--------------------------------------------------------------
void CrosshairsControl::Draw (CDC* pDC)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(m_bbox);
	pDC->SelectClipRgn(&rgn);
	pDC->OffsetViewportOrg(m_bbox.left,m_bbox.top);

	CPen pen(PS_SOLID,0,m_color);
	CPen* pOldPen = pDC->SelectObject(&pen);
	int x = ROUND(m_fx * m_bbox.Width());
	int y = ROUND(m_fy * m_bbox.Height());
	pDC->MoveTo(x,0); pDC->LineTo(x,m_bbox.Height());
	pDC->MoveTo(0,y); pDC->LineTo(m_bbox.Width(),y);
	pDC->SelectObject(pOldPen);

	CBrush brush(m_color);
	CRect mr(x,y,x,y);
	mr.InflateRect(5,5);
	pDC->FrameRect(mr,&brush);

	pDC->SetViewportOrg(0,0);
	pDC->SelectClipRgn(NULL);
	rgn.DeleteObject();
}

//--------------------------------------------------------------
// Hit test: return true only if mouse in indicator area
//--------------------------------------------------------------
BOOL CrosshairsControl::Hit (const CPoint mouse)
{
	int x = ROUND(m_fx * m_bbox.Width()) + m_bbox.left;
	int y = ROUND(m_fy * m_bbox.Height()) + m_bbox.top;
	CRect mr(x,y,x,y);
	mr.InflateRect(5,5);
	return mr.PtInRect(mouse);
}

//--------------------------------------------------------------
// Click
//--------------------------------------------------------------
void CrosshairsControl::Click (UINT nFlags, CPoint point)
{
	m_lastMouse = point;
	m_fx = (double)(point.x - m_bbox.left)/m_bbox.Width();
	m_fy = (double)(point.y - m_bbox.top)/m_bbox.Height();
	if (m_fx < 0.) m_fx = 0.;
	if (m_fx > 1.) m_fx = 1.;
	if (m_fy < 0.) m_fy = 0.;
	if (m_fy > 1.) m_fy = 1.;
	if (m_bVisible) m_pOwner->InvalidateRect(m_bbox,FALSE);
}

//--------------------------------------------------------------
// Drag
//--------------------------------------------------------------
void CrosshairsControl::Drag (UINT nFlags, CPoint point)
{
	CSize diff = point - m_lastMouse;
	m_fx = (double)(point.x - m_bbox.left)/m_bbox.Width();
	m_fy = (double)(point.y - m_bbox.top)/m_bbox.Height();
	if (m_fx < 0.) m_fx = 0.;
	if (m_fx > 1.) m_fx = 1.;
	if (m_fy < 0.) m_fy = 0.;
	if (m_fy > 1.) m_fy = 1.;
	if (m_bVisible) m_pOwner->InvalidateRect(m_bbox,FALSE);
	if (m_pDragProc) (*m_pDragProc)(m_pOwner,nFlags,m_fx,m_fy);
	m_lastMouse = point;
}

//--------------------------------------------------------------
// Release: pass on to owner
//--------------------------------------------------------------
void CrosshairsControl::Release (UINT nFlags, CPoint point)
{
	if (m_pReleaseProc) (*m_pReleaseProc)(m_pOwner,nFlags,point);
}
