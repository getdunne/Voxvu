// MyView.cpp : implementation of the CMyView class
//

#include "stdafx.h"
#include "ControlView.h"

#include "MyDoc.h"
#include "MyControl.h"
#include "MyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyView

IMPLEMENT_DYNCREATE(CMyView, CView)

BEGIN_MESSAGE_MAP(CMyView, CView)
	//{{AFX_MSG_MAP(CMyView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyView construction/destruction

CMyView::CMyView()
{
	m_pChead = m_pCtail = NULL;
	m_pChit = NULL;
	m_bDragging = FALSE;
}

CMyView::~CMyView()
{
}

BOOL CMyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMyView drawing

void CMyView::OnDraw(CDC* pDC)
{
	CMyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	for (CMyControl* pc=m_pCtail; pc; pc=pc->m_pPrev)
		if (pc->isVisible()) pc->Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CMyView printing

BOOL CMyView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMyView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMyView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMyView diagnostics

#ifdef _DEBUG
void CMyView::AssertValid() const
{
	CView::AssertValid();
}

void CMyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyDoc* CMyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyDoc)));
	return (CMyDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyView message handlers


void CMyView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
	m_bDragging = TRUE;
	SetCapture();
	m_pChit = NULL;
	for (CMyControl* pc=m_pChead; pc; pc=pc->m_pNext) {
		if (pc->isEnabled() && pc->Hit(point)) {
			m_pChit = pc;
			pc->Click(nFlags,point);
			break;
		}
	}
}

void CMyView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CView::OnMouseMove(nFlags, point);
	if (m_bDragging && m_pChit!=NULL)
		m_pChit->Drag(nFlags,point);
	else {
		CMyControl* pChit = NULL;
		for (CMyControl* pc=m_pChead; pc; pc=pc->m_pNext) {
			if (pc->isEnabled() && pc->Hit(point)) {
				pChit = pc;
				break;
			}
		}
		if (pChit) ::SetCursor(::LoadCursor(NULL,pChit->GetCursor()));
		else ::SetCursor(::LoadCursor(NULL,IDC_ARROW));
	}
}

void CMyView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDragging = FALSE;
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
	if (m_pChit==NULL) return;
	m_pChit->Release(nFlags,point);
	if (m_pChit->isVisible()) InvalidateRect(m_pChit->m_bbox,FALSE);
}

void CMyView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
	if (m_pChit!=NULL)
		m_pChit->DoubleClick(nFlags,point);
}

// The default OnSetCursor() tries to re-establish the arrow
// cursor all the time.  We override this function to allow
// controls to set up their own cursors.
BOOL CMyView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest==HTCLIENT) return FALSE;
	else return CView::OnSetCursor(pWnd, nHitTest, message);
}
