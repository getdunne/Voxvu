#include "stdafx.h"

#include "ButtonControl.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
ButtonControl::ButtonControl ()
{
	// set up some defaults
	m_label = "Button";
	m_color = RGB(192,192,192);
	m_pHitProc = NULL;

	// button hasn't been touched yet
	m_bPressed = FALSE;
}

//--------------------------------------------------------------
// Redraw the button with its proper color, with centered label
//--------------------------------------------------------------
void ButtonControl::DrawNormal (CDC* pDC)
{
	CBrush br(m_color);
	pDC->FillRect(m_bbox,&br);
	pDC->DrawEdge(m_bbox,EDGE_RAISED,BF_RECT);
	COLORREF crOld = pDC->SetTextColor(RGB(0,0,0));
	int oldMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_label,m_bbox,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	pDC->SetTextColor(crOld);
	pDC->SetBkMode(oldMode);
}

//--------------------------------------------------------------
// Redraw "pushed in"
//--------------------------------------------------------------
void ButtonControl::DrawPressed (CDC* pDC)
{
	CBrush br(m_color);
	pDC->FillRect(m_bbox,&br);
	pDC->DrawEdge(m_bbox,EDGE_SUNKEN,BF_RECT);
	COLORREF crOld = pDC->SetTextColor(RGB(0,0,0));
	int oldMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_label,m_bbox+CSize(1,1),DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	pDC->SetTextColor(crOld);
	pDC->SetBkMode(oldMode);
}

//--------------------------------------------------------------
// Button clicked
//--------------------------------------------------------------
void ButtonControl::Click (UINT nFlags, CPoint point)
{
	m_bPressed = TRUE;
	if (m_bVisible) DrawPressed(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Drag after click: track mouse
//--------------------------------------------------------------
void ButtonControl::Drag (UINT nFlags, CPoint point)
{
	BOOL bPressed = m_bbox.PtInRect(point);
	if (m_bVisible && !m_bPressed && bPressed)
		DrawPressed(m_pOwner->GetDC());
	else if (m_bVisible && m_bPressed && !bPressed)
		DrawNormal(m_pOwner->GetDC());
	m_bPressed = bPressed;
}

//--------------------------------------------------------------
// Button released: redraw as usual
//--------------------------------------------------------------
void ButtonControl::Release (UINT nFlags, CPoint point)
{
	BOOL bWasPressed = m_bPressed;
	if (m_bVisible && bWasPressed) DrawNormal(m_pOwner->GetDC());
	m_bPressed = FALSE;
	if (bWasPressed && m_pHitProc!=NULL)
		(*m_pHitProc)(m_pOwner,this);
}
