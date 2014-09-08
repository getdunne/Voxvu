// class PolyhedronControl represents a control for manipulating a
// polyhedral geometry defined by (at least six) bounding planes.
// It inherits from ArcballControl, because it is most like an arcball
// control. The basic concept comes from the original "voxvu" program
// included (with source code!) with the Trancept Systems TAAC-1
// "Application Accelerator" board for the Sun/360 workstation.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ArcballControl.h"
#include "PolyhedronModel.h"
#include "Polyhedron.h"
#include "PolyhedronRenderer.h"
#include "SRBuffer.h"
#include "VoxelArray.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

class PolyhedronControl : public ArcballControl
{
// Attributes
public:
	PolyhedronModel* m_pModel;	// the geometry, in object coordinates
	void SetModel (PolyhedronModel* pm);

	Polyhedron m_spoly;	//  and in screen coordinates
	Polyhedron* GetScreenPolyPtr () { return &m_spoly; }

	Matrix4x4 GetViewMatrix ();	// object->screen mapping
	Matrix4x4 GetInvViewMatrix ();	// screen->object mapping

	// callback for when user double-clicks to select a face
	//  also called after completing a face-tilt.  Allows owner
	//  view to update sibling controls relating to current face
	typedef void (*FaceSelectProcPtr)(CWnd*,int);
	FaceSelectProcPtr m_pFaceSelectProc;
	void SetFaceSelectProc (FaceSelectProcPtr pFSP)
		{ m_pFaceSelectProc = pFSP; }

	BOOL m_bABVisible;
	void ShowArcball (const BOOL bVis=TRUE) { m_bABVisible = bVis; }

	BOOL m_bTwoPass;
	void SetTwoPass (const BOOL b2p) { m_srBuffer.SetTwoPass(b2p); }

// Constructors
public:
	PolyhedronControl ();
	~PolyhedronControl ();

	// must be called to set up embedded SuccessiveRefinementBuffer
	void SetPixelFormat (int bytesPerPixel, int nColors=0,
						 LPRGBQUAD lpQuadTable=NULL);

	// Operations
public:
	void SetRenderingEnvironment (PolyhedronRenderer* pr,
			VoxelArray* pVoxArray,Matrix4x4* pObjectToTexture);

	void Zoom (double zoomFactor);
	void PanHoriz (double panH);
	void PanVert (double panV);

	void SetRotation (Quaternion& rotQ);
	void Rotate (Quaternion& deltaQ);

	int SelectFace (CPoint point);
	void ModelChanged ();
	void RenderingConditionsChanged ();
	void StartBackgroundRendering ();

// Overrides
public:
	void SetBBox (const CRect& rect);
	virtual void Draw (CDC* pDC);
	virtual void Click (UINT nFlags, CPoint point);
	virtual void Drag (UINT nFlags, CPoint point);
	virtual void Release (UINT nFlags, CPoint point);
	virtual void DoubleClick (UINT nFlags, CPoint point);

// Implementation
	SuccessiveRefinementBuffer m_srBuffer;	// offscreen buffer for rendering
protected:
	BOOL m_bTiltingFace;	// true when tilting selected face
	Matrix4x4* m_pObjToTex;// points to A matrix for volume we're showing
	double m_zoomFactor;	// magnification factor
	double m_panHoriz;		// panning in X
	double m_panVert;		//  and Y
	Quaternion m_orientQ;	// current rotational orientation
	Quaternion m_saveQ;	// saves m_orientQ during rotations
	Vector3 m_origin;		// (possibly shifted) origin of model
	Vector3 m_pivot;		// pivot for face tilting
	Plane m_saveFace;		// saves plane specs during face tilting
};
