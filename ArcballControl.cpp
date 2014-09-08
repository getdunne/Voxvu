#include "stdafx.h"
#include "Vector3.h"
#include <math.h>
#include "ArcballControl.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
ArcballControl::ArcballControl ()
{
	m_color = RGB(0,192,255);	// default color is light blue
	m_pDragProc = NULL;
	m_pDblClickProc = NULL;
	m_pReleaseProc = NULL;
}

//--------------------------------------------------------------
// Draw
//--------------------------------------------------------------
void ArcballControl::Draw (CDC* pDC)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(m_bbox);
	pDC->SelectClipRgn(&rgn);

	CPen pen(PS_SOLID,0,m_color);
	CPen* pOldPen = pDC->SelectObject(&pen);

	CRect abr(m_center,m_center);
	abr.InflateRect(int(m_radius),int(m_radius));
	pDC->Arc(abr,m_bbox.TopLeft(),m_bbox.TopLeft());

	pDC->SelectObject(pOldPen);
	pDC->SelectClipRgn(NULL);
	rgn.DeleteObject();
}

//--------------------------------------------------------------
// SetBBox
//--------------------------------------------------------------
void ArcballControl::SetBBox (const CRect& rect)
{
	m_bbox = rect;
	m_center = rect.CenterPoint();
	int maxDim = rect.Height();
	if (rect.Width() > maxDim) maxDim = rect.Width();
	m_radius = 0.45 * maxDim;
}

//--------------------------------------------------------------
// Helper: transform 2D screen coordinate pairs to 3D points on
// arcball surface.
//--------------------------------------------------------------
void ArcballControl::ArcballTrans(CPoint point, Vector3* result)
{
	Vector3 pt;
	double r,s;

	pt.x = (point.x - m_center.x) / m_radius;
	pt.y = (point.y - m_center.y) / m_radius;
	r = pt.x * pt.x + pt.y * pt.y;
	if (r > 1.0) {
		s = 1.0 / sqrt(r);
		pt.x *= s;
		pt.y *= s;
		pt.z = 0.0;
	}
	else pt.z = -sqrt(1.0 - r);
	*result = pt;
}

//--------------------------------------------------------------
// Click
//--------------------------------------------------------------
void ArcballControl::Click (UINT nFlags, CPoint point)
{
	m_lastMouse = point;
}

//--------------------------------------------------------------
// Drag
//--------------------------------------------------------------
void ArcballControl::Drag (UINT nFlags, CPoint point)
{
	CSize diff = point - m_lastMouse;
	if (m_pDragProc) {
		Vector3 p0,p1;
		ArcballTrans(m_lastMouse,&p0);
		ArcballTrans(point,&p1);
		(*m_pDragProc)(m_pOwner,nFlags,Quaternion(p0.Cross(p1),p0.Dot(p1)));
	}
	m_lastMouse = point;
}

//--------------------------------------------------------------
// Release: pass on to owner
//--------------------------------------------------------------
void ArcballControl::Release (UINT nFlags, CPoint point)
{
	if (m_pReleaseProc) (*m_pReleaseProc)(m_pOwner,nFlags,point);
}

//--------------------------------------------------------------
// Double Click: pass on to owner
//--------------------------------------------------------------
void ArcballControl::DoubleClick (UINT nFlags, CPoint point)
{
	if (m_pDblClickProc) (*m_pDblClickProc)(m_pOwner,nFlags,point);
}
