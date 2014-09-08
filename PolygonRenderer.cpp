// PolygonRenderer.cpp		implementation of class CPolygonRenderer
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PolygonRenderer.h"

inline CPoint ConvPt(const CVector3 &v)
{
	return CPoint(int(v.x+0.5),int(v.y+0.5));
}


#define BACKGROUND_COLOR RGB(40,40,40)

//--------------------------------------------------------------
// Fast interactive renderer
//--------------------------------------------------------------
void CPolygonRenderer::RenderQuick ()
{
	ASSERT(m_pPoly);
	ASSERT(m_pPlane);

	// set up for drawing into our DrawContext
	CDC* pDC = m_pDB->GetMemoryDCPtr();

	// erase background
	CRect rc(CPoint(0,0),m_pDB->GetDimensions());
	CBrush bgb(BACKGROUND_COLOR);
	pDC->FillRect(rc,&bgb);

	// set up our PolyFiller
	m_PolyFiller.Setup(m_pPoly->GetVertexCount());
	m_PolyFiller.m_pLineProc = GetFastLineProcPtr();
	m_PolyFiller.m_clipRect = rc;
	m_PolyFiller.m_pbContinue = NULL;
	m_PolyFiller.m_pUserData = (LPVOID)this;

	m_PolyFiller.Fill(*m_pPoly,*m_pPlane);
}

//--------------------------------------------------------------
// Slower high-quality renderer
//--------------------------------------------------------------
BOOL CPolygonRenderer::RenderBest (BOOL* pbContinue)
{
	ASSERT(m_pPoly);
	ASSERT(m_pPlane);

	// set up for drawing into our DrawContext
	CDC* pDC = m_pDB->GetMemoryDCPtr();

	// erase background
	CRect rc(CPoint(0,0),m_pDB->GetDimensions());
	CBrush bgb(BACKGROUND_COLOR);
	pDC->FillRect(rc,&bgb);

	// set up our PolyFiller
	m_PolyFiller.Setup(m_pPoly->GetVertexCount());
	m_PolyFiller.m_pLineProc = GetBestLineProcPtr();
	m_PolyFiller.m_clipRect = rc;
	m_PolyFiller.m_pbContinue = NULL;
	m_PolyFiller.m_pUserData = (LPVOID)this;

	return m_PolyFiller.Fill(*m_pPoly,*m_pPlane);
}
