// class SuccessiveRefinementBuffer represents a 2D image buffer used for successive
// refinement via an associated Renderer object.  The intent is
// that during user interaction (typically mouse dragging), the
// first rendering pass will be performed repeatedly within the
// main application thread, with the results drawn to the screen
// each time rendering completes; when user interaction ceases,
// the second rendering pass is performed once in a background
// thread which updates the screen just before terminating.  This
// class encapsulates all the thread scheduling code.
//
// SuccessiveRefinementBuffer wraps a DrawBuffer, and associates it with a specific
// rectangle within a particular window, similar to OverlayControl.
//
// The present code assumes exactly two rendering passes ("first"
// and "best").  At some stage it might be appropriate to generalize
// to a list of passes.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DrawBuffer.h"
#include "Renderer.h"

class SuccessiveRefinementBuffer
{
// Attributes
public:
	// "owner" window : where this buffer is always drawn
	CWnd* m_pOwner;
	void SetOwner (CWnd* pView) { m_pOwner = pView; }
	CWnd* GetOwner () { return m_pOwner; }

	// bounding box, defined in owner window's coordinate system
	CRect m_bbox;
	void SetBBox (const CRect& rect);

	// associated Renderer
	Renderer* m_pRenderer;
	void SetRenderer (Renderer* pr) { m_pRenderer = pr; }
	Renderer* GetRenderer () { return m_pRenderer; }

	// return pointer to embedded DrawBuffer
	DrawBuffer* GetDrawBufferPtr () { return &m_DB; }

	BOOL m_bTwoPass;
	void SetTwoPass (BOOL b2p) { m_bTwoPass = b2p; }

// Constructors
public:
	SuccessiveRefinementBuffer ();
	~SuccessiveRefinementBuffer ();

	// must be called to set up embedded DrawBuffer
	void SetPixelFormat (int bytesPerPixel, int nColors=0,
						 LPRGBQUAD lpQuadTable=NULL);

	// Operations
public:
	void Draw ();			// (re)draw buffer to owner window
	void Draw (CDC* pDC);	// use when already have owner DC
	void Draw (CDC* pDC, CPoint topLeft, CSize size)
		{ m_DB.Draw(pDC,topLeft,size); }	// general draw

	void DoFirstPass ();	// do first pass and update screen now
							//  (interrupts any background thread)
	void StartBestPass ();	// start best pass thread

	void InterruptAsync ()	// force termination of BG thread if
							//  any, but don't wait for it
		{ m_bContinue = FALSE; }	// (so simple we inline it)

	void InterruptSync ();	// force termination and wait until BG
							//  thread actually finishes

// Overridables
public:

// Implementation
protected:
	// embedded DrawBuffer
	DrawBuffer m_DB;
	BOOL m_bDBValid;	// true after m_DB.Create() called

	// items related to background threading
	friend UINT BRThreadFunc(LPVOID pParam);	// thread function
	CWinThread* m_pBRThread;					// thread pointer
	HANDLE m_hBRThread;							//  and handle
	BOOL m_bContinue;		// true while background thread running,
							//  set false to interrupt
};
