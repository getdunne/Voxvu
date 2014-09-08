// class DrawBuffer represents an offscreen drawing buffer which
// is associated with a window.  It is basically a wrapper for a
// Win32 DIB Section plus a memory DC and palette.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "PixRect.h"

class DrawBuffer : public CPixRect
{
// Attributes
public:
	CSize GetDimensions () { return CSize(m_width,m_height); }
	HBITMAP GetBitmapHandle () { return m_hbm; }
	CDC* GetMemoryDCPtr () { return m_pmDC; }

// Constructors
public:
	// default constructor for DrawBuffer objects embedded in
	// objects of other classes: it does nearly nothing
	DrawBuffer ();
	~DrawBuffer () { Empty(); };

	// Create() allocates everything, returns TRUE on success
	BOOL Create (CDC* pDC, int width, int height, int nBitsPerPixel,
					int nColors=0, LPRGBQUAD lpQuadTable=NULL);
	// Empty() deallocates everything Create() allocates
	void Empty ();

// Operations
public:
	// blit image to screen at given coordinates
	void Draw (CDC* pDC, int left=0, int top=0);

	// stretch image to some DC (e.g. for printing)
	void Draw (CDC* pDC, CPoint topLeft, CSize outputSize);

	// resize buffer: return TRUE on success
	BOOL Resize (int width, int height);

// Implementation
protected:
	BITMAPINFO* m_pbmi;		// pointer to bitmap descriptor struct
	HBITMAP m_hbm;			// handle to DIB section
	CDC* m_pmDC;			// pointer to memory DC compatible with
							//  associated window, into which the
							//  DIB is always selected
};
