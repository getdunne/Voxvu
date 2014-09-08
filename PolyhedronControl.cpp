#include "stdafx.h"
#include <math.h>

#include "PolyhedronControl.h"


//--------------------------------------------------------------
// Constructor
//--------------------------------------------------------------
PolyhedronControl::PolyhedronControl ()
{
	m_pModel = NULL;
	m_zoomFactor = 1.0;
	m_panHoriz = 0.;
	m_panVert = 0.;
	m_pFaceSelectProc = NULL;
	m_bABVisible = TRUE;
	m_bTwoPass = FALSE;
}

//--------------------------------------------------------------
// Destructor
//--------------------------------------------------------------
PolyhedronControl::~PolyhedronControl ()
{
}

//--------------------------------------------------------------
// SetPixelFormat must be called (after SetOwner!) to set up
// embedded SuccessiveRefinementBuffer object
//--------------------------------------------------------------
void PolyhedronControl::SetPixelFormat (int bytesPerPixel,
						int nColors, LPRGBQUAD lpQuadTable)
{
	ASSERT(m_pOwner);

	m_srBuffer.SetOwner(m_pOwner);
	m_srBuffer.SetPixelFormat(bytesPerPixel,nColors,lpQuadTable);
}

//--------------------------------------------------------------
// GetViewMatrix: return transformation matrix mapping object to
// screen coordinates.
//--------------------------------------------------------------
Matrix4x4 PolyhedronControl::GetViewMatrix ()
{
	// origin offset matrix
	Matrix4x4 O;
	O.MakeTranslation(-m_origin.x, -m_origin.y, -m_origin.z);

	// rotation matrix
	Matrix4x4 R = m_orientQ.RotMatrix();

	// scale factor: when m_zoomFactor=1.0, longest model diagonal
	// should be 95% of minimum display dimension
	double w = m_bbox.Width();
	double h = m_bbox.Height();
	double minWH = (w<h) ? w : h;
	double sf = (0.95*minWH*m_zoomFactor)/m_pModel->GetDiagonal();
	Matrix4x4 S;
	S.MakeDilation(sf);

	// screen-plane translation
	Matrix4x4 T;
	T.MakeTranslation(w*0.5 + m_panHoriz, h*0.5 + m_panVert, 0.);

	return O*R*S*T;
}

//--------------------------------------------------------------
// GetInvViewMatrix: return transformation matrix mapping screen
// to object coordinates (exact inverse of GetViewMatrix() above)
//--------------------------------------------------------------
Matrix4x4 PolyhedronControl::GetInvViewMatrix ()
{
	// origin offset matrix
	Matrix4x4 O;
	O.MakeTranslation(m_origin.x, m_origin.y, m_origin.z);

	// rotation matrix
	Matrix4x4 R = (m_orientQ.RotMatrix()).Transpose();

	// scale factor: when m_zoomFactor=1.0, longest model diagonal
	// should be 95% of minimum display dimension
	double w = m_bbox.Width();
	double h = m_bbox.Height();
	double minWH = (w<h) ? w : h;
	double sf = (0.95*minWH*m_zoomFactor)/m_pModel->GetDiagonal();
	Matrix4x4 S;
	S.MakeDilation(1./sf);

	// screen-plane translation
	Matrix4x4 T;
	T.MakeTranslation(-(w*0.5 + m_panHoriz), -(h*0.5 + m_panVert), 0.);

	return T*S*R*O;
}

//--------------------------------------------------------------
// SetRenderingEnvironment: a wrapper for SetRenderer() to make
// sure we get the right kind of renderer (a PolyhedronRenderer)
// and set it up properly.
//--------------------------------------------------------------
void PolyhedronControl::SetRenderingEnvironment (
							PolyhedronRenderer* pr,
							VoxelArray* pVoxArray,
							Matrix4x4* pObjectToTexture)
{
	m_srBuffer.SetRenderer(pr);
	m_pObjToTex = pObjectToTexture;
	pr->SetDrawBuffer(m_srBuffer.GetDrawBufferPtr());
	pr->SetVoxelArray(pVoxArray);
	pr->SetPolyhedron(&m_spoly);
	pr->SetTransformation(GetInvViewMatrix()*(*pObjectToTexture));
	// render once now
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Set rotational orientation to given absolute
//--------------------------------------------------------------
void PolyhedronControl::SetRotation (Quaternion& rotQ)
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_orientQ = rotQ;
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
		SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// As above but relative
//--------------------------------------------------------------
void PolyhedronControl::Rotate (Quaternion& deltaQ)
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_orientQ = deltaQ * m_orientQ;
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
		SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Set Zoom factor to given absolute
//--------------------------------------------------------------
void PolyhedronControl::Zoom (double zoomFactor)
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_zoomFactor = zoomFactor;
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
		SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Set horizontal panning factor to given absolute
//--------------------------------------------------------------
void PolyhedronControl::PanHoriz (double panH)
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_panHoriz = panH;
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
		SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Set vertical panning factor to given absolute
//--------------------------------------------------------------
void PolyhedronControl::PanVert (double panV)
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_panVert = panV;
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
		SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// SelectFace : given a point in screen space, return the 0-based
// index of the model face whose screen projection contains that
// point, or -1 if there is none.
//--------------------------------------------------------------
int PolyhedronControl::SelectFace (CPoint point)
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_pModel->SelectFace(-1);
	for (int i=0; i < m_spoly.GetFaceCount(); ++i)
		if (m_spoly.plane[i].n.z < 0. &&
			m_spoly.poly[i].GetVertexCount() > 2 &&
			m_spoly.poly[i].ContainsPoint2D(point)) {
			if (i > 5) m_pModel->SelectFace(i);
			else m_pModel->CloneFace(i);
			m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
			((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
				SetPolyhedron(&m_spoly);
			return m_pModel->GetSelectedFace();
		}
	return -1;
}

//--------------------------------------------------------------
// Model has been changed.  Update screen representation.
//--------------------------------------------------------------
void PolyhedronControl::ModelChanged ()
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
		SetPolyhedron(&m_spoly);
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Rendering conditions have changed.  Update screen.
//--------------------------------------------------------------
void PolyhedronControl::RenderingConditionsChanged ()
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.InterruptSync();
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// StartBackgroundRendering : Use after calls to SetRotation,
// Zoom, etc.  Just a wrapper for SuccessiveRefinementBuffer::StartBestPass().
//--------------------------------------------------------------
void PolyhedronControl::StartBackgroundRendering ()
{
	ASSERT(m_srBuffer.m_pRenderer);

	m_srBuffer.StartBestPass();
}

//--------------------------------------------------------------
// SetModel
//--------------------------------------------------------------
void PolyhedronControl::SetModel (PolyhedronModel* pm)
{
	m_pModel = pm;
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
}

//--------------------------------------------------------------
// Draw : displays polyhedron outlines and arcball
//--------------------------------------------------------------
inline CPoint ConvPt(const Vector3 &v)
{
	return CPoint(int(v.x+0.5),int(v.y+0.5));
}

void PolyhedronControl::Draw (CDC* pDC)
{
	// first draw from our rendering buffer
	m_srBuffer.Draw(pDC);

	// set up for drawing in this control's bounding box only
	CRgn rgn;
	rgn.CreateRectRgnIndirect(m_bbox);
	pDC->SelectClipRgn(&rgn);
	pDC->OffsetViewportOrg(m_bbox.left,m_bbox.top);

	int selectedFace = m_pModel->GetSelectedFace();

	// draw outlines of non-selected front faces of box in white
	HGDIOBJ pOldPen = pDC->SelectStockObject(WHITE_PEN);
	int side;
	for (side=0; side<m_spoly.GetFaceCount(); ++side) {
		if (m_spoly.poly[side].GetVertexCount() < 3) continue;
		if (m_spoly.plane[side].n.z > 0.) continue;
		if (side == selectedFace) continue;
		pDC->MoveTo(ConvPt(m_spoly.poly[side].v[0]));
		for (int vert=1; vert<m_spoly.poly[side].GetVertexCount(); ++vert)
			pDC->LineTo(ConvPt(m_spoly.poly[side].v[vert]));
		pDC->LineTo(ConvPt(m_spoly.poly[side].v[0]));
	}

	// outline selected face in red (if visible)
	CPen redPen(PS_SOLID,0,RGB(255,0,0));
	pDC->SelectObject(&redPen);
	side = selectedFace;
	if (side >= 0 && m_spoly.poly[side].GetVertexCount() > 2 &&
		m_spoly.plane[side].n.z < 0.) {
		pDC->MoveTo(ConvPt(m_spoly.poly[side].v[0]));
		for (int vert=1; vert<m_spoly.poly[side].GetVertexCount(); ++vert)
			pDC->LineTo(ConvPt(m_spoly.poly[side].v[vert]));
		pDC->LineTo(ConvPt(m_spoly.poly[side].v[0]));
	}

	// clean up
	pDC->SetViewportOrg(0,0);
	pDC->SelectObject(pOldPen);
	pDC->SelectClipRgn(NULL);
	rgn.DeleteObject();

	// draw arcball circle
	if (m_bABVisible) ArcballControl::Draw(pDC);
}

//--------------------------------------------------------------
// SetBBox
//--------------------------------------------------------------
void PolyhedronControl::SetBBox (const CRect& rect)
{
	// resize arcball
	ArcballControl::SetBBox(rect);
	
	// resize embedded SuccessiveRefinementBuffer
	m_srBuffer.SetBBox(rect);

	// if we're ready, resize displayed polyhedron
	if (m_srBuffer.m_pRenderer) {
		m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
		((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
			SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
		m_srBuffer.DoFirstPass();
	}
}

//--------------------------------------------------------------
// Click
//--------------------------------------------------------------
void PolyhedronControl::Click (UINT nFlags, CPoint point)
{
	m_lastMouse = point;
	m_saveQ = m_orientQ;
	int selFace = m_pModel->GetSelectedFace();
	m_bTiltingFace = (nFlags & MK_CONTROL) != 0 && (selFace >= 0);
	if (m_bTiltingFace) {
		m_saveFace = m_pModel->m_box.plane[selFace];
		Plane p = m_saveFace.Transform(GetViewMatrix());
		Vector3 pivot;
		pivot.x = point.x - m_bbox.left;
		pivot.y = point.y - m_bbox.top;
		pivot.z = p.ZfromXY(pivot.x,pivot.y);
		m_pivot = pivot.Aff(GetInvViewMatrix());
	}
}

//--------------------------------------------------------------
// Drag
//--------------------------------------------------------------
void PolyhedronControl::Drag (UINT nFlags, CPoint point)
{
	Vector3 p0,p1;
	ArcballTrans(m_lastMouse,&p0);
	ArcballTrans(point,&p1);
	Quaternion q(p0.Cross(p1),p0.Dot(p1));
	if (m_bTiltingFace) {
		int selFace = m_pModel->GetSelectedFace();
		Matrix4x4 VR = m_saveQ.RotMatrix();
		Matrix4x4 R = VR * q.RotMatrix() * (VR.Transpose());
		m_pModel->m_box.plane[selFace].n = (m_saveFace.n).Lin(R);
		(m_pModel->m_box.plane[selFace].n).Normalize();
		m_pModel->m_box.plane[selFace].d = (m_pModel->m_box.plane[selFace].n).Dot(m_pivot);
		m_pModel->RecomputePolygons();
		((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
			SetPolyhedron(&m_spoly);
	}
	else {
		m_orientQ = q * m_saveQ;
		((PolyhedronRenderer*)m_srBuffer.m_pRenderer)->
			SetTransformation(GetInvViewMatrix()*(*m_pObjToTex));
	}
	m_pModel->m_box.Transform(GetViewMatrix(),&m_spoly);
	m_srBuffer.DoFirstPass();
	Draw(m_pOwner->GetDC());
}

//--------------------------------------------------------------
// Release
//--------------------------------------------------------------
void PolyhedronControl::Release (UINT nFlags, CPoint point)
{
	if (m_bTiltingFace && m_pFaceSelectProc)
		(*m_pFaceSelectProc)(m_pOwner,m_pModel->GetSelectedFace());
	m_bTiltingFace = FALSE;
	m_srBuffer.StartBestPass();
}

//--------------------------------------------------------------
// DoubleClick
//--------------------------------------------------------------
void PolyhedronControl::DoubleClick (UINT nFlags, CPoint point)
{
	m_srBuffer.InterruptSync();
	point.Offset(-m_bbox.TopLeft());
	int nFace = SelectFace(point);
	if (m_pFaceSelectProc) (*m_pFaceSelectProc)(m_pOwner,nFace);
}

