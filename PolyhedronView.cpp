#include "stdafx.h"
#include <math.h>

#include "Voxvu.h"
#include "VoxelFilePair.h"

extern UINT NEAR uMSH_MOUSEWHEEL;
#include "PolyhedronView.h"

#include "PolyRend8BitGray.h"
#include "PolyRend16BitGray.h"
#include "PolyRend15BitRGB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// callback procedures

static void ResetBoxProc (CWnd* pOwner, ButtonControl* pBtn)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	pMe->m_model.Reinitialize();
	pMe->m_polyCtrl.ModelChanged();
	pMe->SetTimer(1,500,NULL);
}

static void ResetOrientProc (CWnd* pOwner, ButtonControl* pBtn)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	Quaternion orQ;	// constructor initializes to no rotation
	pMe->m_polyCtrl.SetRotation(orQ);
	pMe->SetTimer(1,500,NULL);
}

static void GeneralReleaseProc (CWnd* pOwner, UINT nFlags, CPoint point)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	pMe->SetTimer(1,500,NULL);
}

static void FaceSelectProc (CWnd* pOwner, int nFace)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	if (nFace < 0) return;
	pMe->m_wheelL.SetValue(pMe->m_model.m_box.plane[nFace].d);
}

static void SliceProc (CWnd* pOwner, double value)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	if (pMe->m_model.GetSelectedFace() < 0) return;
	pMe->m_model.TranslateSelectedFace(value);
	pMe->m_polyCtrl.ModelChanged();
}

static void ZoomProc (CWnd* pOwner, double value)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	pMe->m_polyCtrl.Zoom(pow(2.,value));
}

static void HorizPanProc (CWnd* pOwner, double value)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	pMe->m_polyCtrl.PanHoriz(value);
}

static void VertPanProc (CWnd* pOwner, double value)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	pMe->m_polyCtrl.PanVert(value);
}

static void LocatorProc (CWnd* pOwner, UINT nFlags, double fx, double fy)
{
	PolyhedronView* pMe = (PolyhedronView*)pOwner;
	pMe->m_pRenderer->SetWindowLevel(fx,fy);
	pMe->m_polyCtrl.RenderingConditionsChanged();
}

/////////////////////////////////////////////////////////////////////////////
// PolyhedronView

IMPLEMENT_DYNCREATE(PolyhedronView, VoxvuView)

BEGIN_MESSAGE_MAP(PolyhedronView, VoxvuView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PHVC_WLCROSSHAIR, OnContext_WLCrosshair)
	ON_COMMAND(ID_PHVC_ARCBALLCIRCLE, OnContext_ABCircle)
	ON_COMMAND(ID_PHVC_TWOPASS, OnContext_TwoPass)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


PolyhedronView::PolyhedronView()
{
	m_pRenderer = NULL;
    m_contextMenu.LoadMenu (IDR_PHV_CONTEXT);

	// set up controls list (but not layout)
	m_pChead = &m_wlCrosshairs;
	m_wlCrosshairs.SetLinks(NULL,&m_polyCtrl);
	m_polyCtrl.SetLinks(&m_wlCrosshairs,&m_buttonTL);
	m_buttonTL.SetLinks(&m_polyCtrl,&m_buttonTR);
	m_buttonTR.SetLinks(&m_buttonTL,&m_buttonBL);
	m_buttonBL.SetLinks(&m_buttonTR,&m_buttonBR);
	m_buttonBR.SetLinks(&m_buttonBL,&m_wheelT);
	m_wheelT.SetLinks(&m_buttonBR,&m_wheelL);
	m_wheelL.SetLinks(&m_wheelT,&m_wheelR);
	m_wheelR.SetLinks(&m_wheelL,&m_wheelB);
	m_wheelB.SetLinks(&m_wheelR,NULL);
	m_pCtail = &m_wheelB;

	// establish slicing control as preferred one for this view
	SetShiftControl(&m_wheelL);

	// set up control attributes
	m_wlCrosshairs.SetValue(1.0,0.5);
	m_buttonTL.SetLabel("Box");
	m_buttonTR.SetLabel("Orient");
	m_buttonBL.SetLabel("");
	m_buttonBR.SetLabel("");
	m_wlCrosshairs.SetCursor(IDC_SIZEALL);
	m_wheelT.SetCursor(IDC_SIZEWE);
	m_wheelL.SetCursor(IDC_SIZENS);
	m_wheelR.SetCursor(IDC_SIZENS);
	m_wheelB.SetCursor(IDC_SIZEWE);
	m_wheelT.SetSens(0.01);

	// this view owns all controls, and all are enabled and visible
	for (OverlayControl* p=m_pChead; p; p=p->m_pNext) {
		p->SetOwner(this);
		p->Enable();
		p->Show();
	}

	// set up callbacks
	m_wlCrosshairs.SetDragProc(LocatorProc);
	m_wlCrosshairs.SetReleaseProc(GeneralReleaseProc);
	m_polyCtrl.SetFaceSelectProc(FaceSelectProc);
	m_buttonTL.SetHitProc(ResetBoxProc);
	m_buttonTR.SetHitProc(ResetOrientProc);
	m_wheelT.SetAdjProc(ZoomProc);
	m_wheelT.SetReleaseProc(GeneralReleaseProc);
	m_wheelL.SetAdjProc(SliceProc);
	m_wheelL.SetReleaseProc(GeneralReleaseProc);
	m_wheelR.SetAdjProc(VertPanProc);
	m_wheelR.SetReleaseProc(GeneralReleaseProc);
	m_wheelB.SetAdjProc(HorizPanProc);
	m_wheelB.SetReleaseProc(GeneralReleaseProc);
}

PolyhedronView::~PolyhedronView()
{
	if (m_pRenderer != NULL) delete m_pRenderer;
}


void PolyhedronView::OnDraw(CDC* pDC)
{
	if (pDC->IsPrinting())   // printer DC
	{
		// get image dimensions from polyhedron control (the part we're going
		// to print) and set up a DrawBuffer that size
		int picWidth = m_polyCtrl.m_bbox.Width();
		int picHeight = m_polyCtrl.m_bbox.Height();
		DrawBuffer db;
		VERIFY(db.Create(GetDC(),picWidth,picHeight,32));

		// have polyhedron control draw itself into that buffer
		BOOL abwasvis = m_polyCtrl.m_bABVisible;
		m_polyCtrl.m_bABVisible = FALSE;
		CRect pcrs = m_polyCtrl.m_bbox;
		m_polyCtrl.m_bbox.OffsetRect(-m_polyCtrl.m_bbox.TopLeft());
		m_polyCtrl.m_srBuffer.m_bbox = m_polyCtrl.m_bbox;
		m_polyCtrl.Draw(db.GetMemoryDCPtr());
		m_polyCtrl.m_bbox = m_polyCtrl.m_srBuffer.m_bbox = pcrs;
		m_polyCtrl.m_bABVisible = abwasvis;

		// get size of printer page (in pixels) and resolution
		int cxPage = pDC->GetDeviceCaps(HORZRES);
		int cyPage = pDC->GetDeviceCaps(VERTRES);
		int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
		int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

		// create an output rectangle which preserves the image aspect ratio
		// and fills the page horizontally
		CRect rcDest;
		rcDest.top = rcDest.left = 0;
		rcDest.bottom = (int)(((double)picHeight * cxPage * cyInch)
				/ ((double)picWidth * cxInch));
		rcDest.right = cxPage;

		// now render our buffer to the page
		db.Draw(pDC,rcDest.TopLeft(),rcDest.Size());
	}
	else // redrawing to screen
		VoxvuView::OnDraw(pDC);
}

#ifdef _DEBUG
void PolyhedronView::AssertValid() const
{
	VoxvuView::AssertValid();
}

void PolyhedronView::Dump(CDumpContext& dc) const
{
	VoxvuView::Dump(dc);
}
#endif //_DEBUG

void PolyhedronView::OnSize(UINT nType, int cx, int cy) 
{
	VoxvuView::OnSize(nType, cx, cy);
	if (cx==0 && cy==0) return;

	// lay out controls for new size
	CRect vr;
	GetClientRect(&vr);
	m_polyCtrl.SetBBox(CRect(vr.left+20,vr.top+20,vr.right-19,vr.bottom-19));
	m_wlCrosshairs.SetBBox(CRect(vr.left+20,vr.top+20,vr.right-19,vr.bottom-19));
	m_buttonTL.SetBBox(CRect(vr.left,vr.top,vr.left+80,vr.top+20));
	m_buttonTR.SetBBox(CRect(vr.right-79,vr.top,vr.right,vr.top+20));
	m_buttonBL.SetBBox(CRect(vr.left,vr.bottom-19,vr.left+80,vr.bottom));
	m_buttonBR.SetBBox(CRect(vr.right-79,vr.bottom-19,vr.right,vr.bottom));
	m_wheelT.SetBBox(CRect(vr.left+80,vr.top,vr.right-79,vr.top+20));
	m_wheelL.SetBBox(CRect(vr.left,vr.top+20,vr.left+20,vr.bottom-19));
	m_wheelR.SetBBox(CRect(vr.right-19,vr.top+20,vr.right,vr.bottom-19));
	m_wheelB.SetBBox(CRect(vr.left+80,vr.bottom-19,vr.right-79,vr.bottom));

	if (m_model.isValid())
		m_wheelL.SetSens(m_model.GetDiagonal()/(vr.Height()-40));
}

void PolyhedronView::OnInitialUpdate() 
{
	// establish pixel format and renderer
	switch (GetDocument()->m_kind) {
	case k8BitGray:
		m_polyCtrl.SetPixelFormat(1,GetDocument()->m_colors,
									GetDocument()->m_color);
		m_pRenderer = new PolyRend8BitGray;
		break;
	case k16BitGray:
		m_polyCtrl.SetPixelFormat(1,GetDocument()->m_colors,
									GetDocument()->m_color);
		m_pRenderer = new PolyRend16BitGray;
		break;
	case k15BitRGB:
		m_polyCtrl.SetPixelFormat(2,GetDocument()->m_colors,
									GetDocument()->m_color);
		m_wlCrosshairs.Show(false);
		m_wlCrosshairs.Enable(false);
		m_contextMenu.GetSubMenu(0)->CheckMenuItem(0,MF_BYPOSITION|MF_UNCHECKED);
		m_pRenderer = new PolyRend15BitRGB;
		break;
	default:
		ASSERT(FALSE);	// shouldn't happen
	}

	// establish default polyhedron and view
	double w = GetDocument()->m_array.GetWidth();
	double h = GetDocument()->m_array.GetHeight();
	double d = GetDocument()->m_array.GetDepth();
	m_model.Initialize(w,h,d);
	m_polyCtrl.SetModel(&m_model);
	
	m_polyCtrl.SetRenderingEnvironment (m_pRenderer,
					&GetDocument()->m_array, &GetDocument()->m_A);

	// set sensitivity of slicing control
	m_wheelL.SetSens(m_model.GetDiagonal()/(m_wheelL.m_bbox.Height()));

	// make sure we get displayed
	VoxvuView::OnInitialUpdate();
}

BOOL PolyhedronView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int nFace = m_model.GetSelectedFace();
	if (nFace < 0) return 0;
	double value = m_wheelL.GetValue() - 5.*(double)zDelta/WHEEL_DELTA;
	m_wheelL.SetValue(value);
	m_model.TranslateSelectedFace(value);
	m_polyCtrl.ModelChanged();
	SetTimer(1,500,NULL);
	return 0;
}

void PolyhedronView::OnTimer(UINT nIDEvent) 
{
	KillTimer(nIDEvent);
	m_polyCtrl.StartBackgroundRendering();
}

void PolyhedronView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    CMenu* pContextMenu = m_contextMenu.GetSubMenu (0);

    pContextMenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_LEFTBUTTON |
        TPM_RIGHTBUTTON, point.x, point.y, this);
}

void PolyhedronView::OnContext_WLCrosshair() 
{
    CMenu* pContextMenu = m_contextMenu.GetSubMenu (0);

    BOOL vis = ((pContextMenu->GetMenuState(0,MF_BYPOSITION) & MF_CHECKED) == 0);
	pContextMenu->CheckMenuItem(0,MF_BYPOSITION|((vis)?MF_CHECKED:MF_UNCHECKED));
	m_wlCrosshairs.Show(vis);
	m_wlCrosshairs.Enable(vis);

	CRect rc;
	GetClientRect(&rc);
	InvalidateRect(&rc,FALSE);
}

void PolyhedronView::OnContext_ABCircle() 
{
    CMenu* pContextMenu = m_contextMenu.GetSubMenu (0);

    BOOL vis = ((pContextMenu->GetMenuState(1,MF_BYPOSITION) & MF_CHECKED) == 0);
	pContextMenu->CheckMenuItem(1,MF_BYPOSITION|((vis)?MF_CHECKED:MF_UNCHECKED));
	m_polyCtrl.ShowArcball(vis);

	CRect rc;
	GetClientRect(&rc);
	InvalidateRect(&rc,FALSE);
}

void PolyhedronView::OnContext_TwoPass() 
{
    CMenu* pContextMenu = m_contextMenu.GetSubMenu (0);

    BOOL vis = ((pContextMenu->GetMenuState(2,MF_BYPOSITION) & MF_CHECKED) == 0);
	pContextMenu->CheckMenuItem(2,MF_BYPOSITION|((vis)?MF_CHECKED:MF_UNCHECKED));
	m_polyCtrl.SetTwoPass(vis);

	CRect rc;
	GetClientRect(&rc);
	InvalidateRect(&rc,FALSE);
}
