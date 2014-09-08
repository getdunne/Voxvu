// PolyhedronRenderer.cpp		implementation of class PolyhedronRenderer
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PolyhedronRenderer.h"

inline CPoint ConvPt(const Vector3 &v)
{
	return CPoint(int(v.x+0.5),int(v.y+0.5));
}


#define BACKGROUND_COLOR RGB(40,40,40)
#define PLANE_BACK_COLOR RGB(100,100,100)

//--------------------------------------------------------------
// Fast interactive renderer
//--------------------------------------------------------------
void PolyhedronRenderer::RenderQuick ()
{
	ASSERT(m_pPoly != NULL);

	// set up for drawing into our DrawContext
	CDC* pDC = m_pDB->GetMemoryDCPtr();

	// erase background
	CRect rc(CPoint(0,0),m_pDB->GetDimensions());
	CBrush bgb(BACKGROUND_COLOR);
	pDC->FillRect(rc,&bgb);

	// set up our PolyFiller
	int maxVerts = 0;
	for (int side=0; side<m_pPoly->GetFaceCount(); ++side) {
		if (m_pPoly->plane[side].n.z > 0.) continue;
		int nv = m_pPoly->poly[side].GetVertexCount();
		if (nv > maxVerts) maxVerts = nv;
	}
	m_PolyFiller.Setup(maxVerts);
	m_PolyFiller.m_pLineProc = GetFastLineProcPtr();
	m_PolyFiller.m_clipRect = rc;
	m_PolyFiller.m_pbContinue = NULL;
	m_PolyFiller.m_pUserData = (LPVOID)this;

	if (m_toDraw == -1)
		// render front faces of box
		for (int side=0; side<m_pPoly->GetFaceCount(); ++side) {
			if (m_pPoly->poly[side].GetVertexCount() < 3) continue;
			if (m_pPoly->plane[side].n.z > 0.) continue;
			m_PolyFiller.Fill(m_pPoly->poly[side], m_pPoly->plane[side]);
		}
	else {
		ASSERT(m_toDraw >=0 && m_toDraw < m_pPoly->GetFaceCount());
		m_PolyFiller.Fill(m_pPoly->poly[m_toDraw],m_pPoly->plane[m_toDraw]);
	}
}

//--------------------------------------------------------------
// Slower high-quality renderer
//--------------------------------------------------------------
BOOL PolyhedronRenderer::RenderBest (BOOL* pbContinue)
{
	ASSERT(m_pPoly != NULL);

	// set up for drawing into our DrawContext
	CDC* pDC = m_pDB->GetMemoryDCPtr();

	// erase background
	CRect rc(CPoint(0,0),m_pDB->GetDimensions());
	CBrush bgb(BACKGROUND_COLOR);
	pDC->FillRect(rc,&bgb);

	// set up our PolyFiller
	int maxVerts = 0;
	for (int side=0; side<m_pPoly->GetFaceCount(); ++side) {
		if (m_pPoly->plane[side].n.z > 0.) continue;
		int nv = m_pPoly->poly[side].GetVertexCount();
		if (nv > maxVerts) maxVerts = nv;
	}
	m_PolyFiller.Setup(maxVerts);
	m_PolyFiller.m_pLineProc = GetBestLineProcPtr();
	m_PolyFiller.m_clipRect = rc;
	m_PolyFiller.m_pbContinue = pbContinue;
	m_PolyFiller.m_pUserData = (LPVOID)this;

	if (m_toDraw == -1) {
		// render front faces of box
		for (int side=0; side<m_pPoly->GetFaceCount(); ++side) {
			if (m_pPoly->poly[side].GetVertexCount() < 3) continue;
			if (m_pPoly->plane[side].n.z > 0.) continue;
			if (!m_PolyFiller.Fill(m_pPoly->poly[side], m_pPoly->plane[side]))
				return FALSE;
		}
		return TRUE;
	}
	else {
		ASSERT(m_toDraw >=0 && m_toDraw < m_pPoly->GetFaceCount());
		return m_PolyFiller.Fill(m_pPoly->poly[m_toDraw],
									m_pPoly->plane[m_toDraw]);
	}
}
