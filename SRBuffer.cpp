#include "stdafx.h"

#include "SRBuffer.h"


//--------------------------------------------------------------
// Background rendering thread function
//--------------------------------------------------------------
UINT BRThreadFunc (LPVOID pParam)
{
	SuccessiveRefinementBuffer* p = (SuccessiveRefinementBuffer*)pParam;
	if (!p->m_bContinue) return 0;
	if (p->m_pRenderer->RenderBest(&p->m_bContinue))
		// ensure screen gets repainted (but no hurry)
		p->m_pOwner->InvalidateRect(&p->m_bbox,FALSE);
	p->m_bContinue = FALSE;
	return 0;
}


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
SuccessiveRefinementBuffer::SuccessiveRefinementBuffer ()
{
	m_pRenderer = NULL;
	m_pOwner = NULL;
	m_bContinue = FALSE;
	m_bDBValid = FALSE;
	m_bTwoPass = FALSE;
}

//--------------------------------------------------------------
// Destructor: note DrawBuffer destructor will deallocate owned
// storage.
//--------------------------------------------------------------
SuccessiveRefinementBuffer::~SuccessiveRefinementBuffer ()
{
	// make sure BG thread is dead before we go
	InterruptSync();
}

//--------------------------------------------------------------
// SetPixelFormat() is just a wrapper for DrawBuffer::Create()
// which also sets m_bDBValid.
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::SetPixelFormat (
	int bytesPerPixel, int nColors, LPRGBQUAD lpQuadTable)
{
	ASSERT(m_bDBValid==FALSE);
	ASSERT(m_pOwner);

	VERIFY(m_DB.Create(m_pOwner->GetDC(),m_bbox.Width(),m_bbox.Height(),
						8*bytesPerPixel,nColors,lpQuadTable));
	m_bDBValid = TRUE;
}

//--------------------------------------------------------------
// SetBBox : if draw buffer has been set up, resize it.
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::SetBBox (const CRect& rect)
{
	m_bbox = rect;

	if (m_bDBValid)
		VERIFY(m_DB.Resize(m_bbox.Width(),m_bbox.Height()));
}

//--------------------------------------------------------------
// Draw : case where we don't have a DC
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::Draw ()
{
	ASSERT(m_pOwner);
	ASSERT(m_bDBValid);

	CDC* pDC = m_pOwner->GetDC();
	m_DB.Draw(pDC,m_bbox.left,m_bbox.top);
	m_pOwner->ReleaseDC(pDC);
}

//--------------------------------------------------------------
// Draw : case where we already have owner DC in hand
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::Draw (CDC* pDC)
{
	ASSERT(m_pOwner);
	ASSERT(m_bDBValid);

	m_DB.Draw(pDC,m_bbox.left,m_bbox.top);
}

//--------------------------------------------------------------
// DoFirstPass
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::DoFirstPass ()
{
	ASSERT(m_pRenderer);
	ASSERT(m_bDBValid);

	InterruptSync();
	if (m_bTwoPass) m_pRenderer->RenderQuick();
	else m_pRenderer->RenderBest(NULL);
}

//--------------------------------------------------------------
// StartBestPass
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::StartBestPass ()
{
	// do nothing if already started
	if (m_bContinue) return;

	// spawn background thread
	m_bContinue = TRUE;
	m_pBRThread = AfxBeginThread(BRThreadFunc, (LPVOID)this);
	m_hBRThread = m_pBRThread->m_hThread;
}

//--------------------------------------------------------------
// InterruptSync
//--------------------------------------------------------------
void SuccessiveRefinementBuffer::InterruptSync ()
{
	// if background rendering is going on, interrupt it and do
	// nothing till it has really stopped
	if (m_bContinue) {
		m_bContinue = FALSE;
		::WaitForSingleObject(m_hBRThread,INFINITE);
	}
}

