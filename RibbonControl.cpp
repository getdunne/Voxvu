#include "stdafx.h"

#include "RibbonControl.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
RibbonControl::RibbonControl ()
{
	// set up some defaults
	m_color = RGB(192,192,192);
	m_pAdjust = NULL;
	m_value = 0.;
	m_sens = 1.;
	m_phase = 0;
}

//--------------------------------------------------------------
// Redraw the wheel
//--------------------------------------------------------------
void RibbonControl::Draw (CDC* pDC)
{
	CBrush br(m_color);
	pDC->FillRect(m_bbox,&br);
	pDC->DrawEdge(m_bbox,EDGE_RAISED,BF_RECT);
	CRect whr = m_bbox; whr.DeflateRect(4,4);
	pDC->DrawEdge(whr,EDGE_SUNKEN,BF_RECT);
	DrawIndicator(pDC);
}

//--------------------------------------------------------------
// Hit test: return true only if mouse in indicator area
//--------------------------------------------------------------
BOOL RibbonControl::Hit (const CPoint mouse)
{
	CRect whr = m_bbox;
	if (m_bEnabled) whr.DeflateRect(6,6);
	return whr.PtInRect(mouse);
}

//--------------------------------------------------------------
// Redraw just the indicator part
//--------------------------------------------------------------
void RibbonControl::DrawIndicator (CDC* pDC)
{
	CRect whr = m_bbox; whr.DeflateRect(6,6);
	CPoint obr;
	if (m_bbox.Width() > m_bbox.Height())
	{ // horizontal wheel
		CBrush hbr(HS_VERTICAL,RGB(0,0,0));
		obr = pDC->SetBrushOrg(m_phase,0);
		pDC->FillRect(whr,&hbr);
		pDC->SetBrushOrg(obr);
	}
	else
	{ // vertical wheel
		CBrush hbr(HS_HORIZONTAL,RGB(0,0,0));
		obr = pDC->SetBrushOrg(0,m_phase);
		pDC->FillRect(whr,&hbr);
		pDC->SetBrushOrg(obr);
	}
	pDC->SetBrushOrg(obr);
}

//--------------------------------------------------------------
// Click
//--------------------------------------------------------------
void RibbonControl::Click (UINT nFlags, CPoint point)
{
	m_lastMouse = point;
}

//--------------------------------------------------------------
// Drag == adjust
//--------------------------------------------------------------
void RibbonControl::Drag (UINT nFlags, CPoint point)
{
	CSize diff = point - m_lastMouse;
	m_lastMouse = point;
	int delta = (m_bbox.Width()>m_bbox.Height()) ? diff.cx : diff.cy;
	m_value += delta*m_sens;
	m_phase += delta; while (m_phase<0) m_phase += 8; m_phase %= 8;
	if (m_bVisible) DrawIndicator(m_pOwner->GetDC());
	if (m_pAdjust) (*m_pAdjust)(m_pOwner,m_value);
}

//--------------------------------------------------------------
// Release: notify owner
//--------------------------------------------------------------
void RibbonControl::Release (UINT nFlags, CPoint point)
{
	if (m_pReleaseProc) (*m_pReleaseProc)(m_pOwner,nFlags,point);
}

//--------------------------------------------------------------
// Double Click == reset
//--------------------------------------------------------------
void RibbonControl::DoubleClick (UINT nFlags, CPoint point)
{
	m_value = 0.;
	m_phase = 0;
	if (m_bVisible) DrawIndicator(m_pOwner->GetDC());
	if (m_pAdjust) (*m_pAdjust)(m_pOwner,m_value);
}
