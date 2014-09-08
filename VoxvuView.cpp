#include "stdafx.h"
#include "Voxvu.h"

#include "VoxvuView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VoxvuView

IMPLEMENT_DYNCREATE(VoxvuView, CView)

BEGIN_MESSAGE_MAP(VoxvuView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VoxvuView construction/destruction

VoxvuView::VoxvuView()
{
	m_pChead = m_pCtail = NULL;
	m_pChit = NULL;
	m_pShiftCtrl = NULL;
	m_bDragging = FALSE;
}

VoxvuView::~VoxvuView()
{
}

BOOL VoxvuView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// VoxvuView drawing

void VoxvuView::OnDraw(CDC* pDC)
{
	VoxelFilePair* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	for (OverlayControl* pc=m_pCtail; pc; pc=pc->m_pPrev)
		if (pc->isVisible()) pc->Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// VoxvuView printing

BOOL VoxvuView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void VoxvuView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void VoxvuView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// VoxvuView diagnostics

#ifdef _DEBUG
void VoxvuView::AssertValid() const
{
	CView::AssertValid();
}

void VoxvuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

VoxelFilePair* VoxvuView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VoxelFilePair)));
	return (VoxelFilePair*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// VoxvuView message handlers

void VoxvuView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
	m_bDragging = TRUE;
	SetCapture();
	if (m_pShiftCtrl!=NULL && (nFlags & MK_SHIFT)!=0) {
		m_pChit = m_pShiftCtrl;
		m_pChit->Click(nFlags,point);
	}
	else {
		m_pChit = NULL;
		for (OverlayControl* pc=m_pChead; pc; pc=pc->m_pNext) {
			if (pc->isEnabled() && pc->Hit(point)) {
				m_pChit = pc;
				pc->Click(nFlags,point);
				break;
			}
		}
	}
}

void VoxvuView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CView::OnMouseMove(nFlags, point);
	if (m_bDragging && m_pChit!=NULL)
		m_pChit->Drag(nFlags,point);
	else {
		OverlayControl* pChit = NULL;
		for (OverlayControl* pc=m_pChead; pc; pc=pc->m_pNext) {
			if (pc->isEnabled() && pc->Hit(point)) {
				pChit = pc;
				break;
			}
		}
		if (pChit) ::SetCursor(::LoadCursor(NULL,pChit->GetCursor()));
		else ::SetCursor(::LoadCursor(NULL,IDC_ARROW));
	}
}

void VoxvuView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDragging = FALSE;
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
	if (m_pChit==NULL) return;
	m_pChit->Release(nFlags,point);
	if (m_pChit->isVisible()) InvalidateRect(m_pChit->m_bbox,FALSE);
}

void VoxvuView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
	if (m_pChit!=NULL)
		m_pChit->DoubleClick(nFlags,point);
}

// The default OnSetCursor() tries to re-establish the arrow
// cursor all the time.  We override this function to allow
// controls to set up their own cursors.
BOOL VoxvuView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest==HTCLIENT) return FALSE;
	else return CView::OnSetCursor(pWnd, nHitTest, message);
}
