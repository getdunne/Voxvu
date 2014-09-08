// Class CPixRect encapsulates 2D pixel array addressing.
// A CPixRect object describes a rectangular subregion within a
// (possibly much larger) array.  Several CPixRect objects may
// point into a single array, and hence these objects do not own
// (i.e. cannot delete or resize) the array they point to.
//
// The few assumptions concerning array implementation are:
//  - the pixel array is a 2D array stored in row-major order
//  - the first pixel in the array is at the top-left corner
//  - all pixels are the same size and type
//  - adjacent pixels in any row are adjacent in memory
//  - the offset (m_rowBytes) from row to row is constant
//
// The approach used is NOT type-safe.  Client code is responsible
// for proper casting of returned pointers.
#pragma once

class CPixRect
{
// Attributes
protected:
	WORD m_pixBytes;		// bytes per pixel
	WORD m_width, m_height;	// dimensions of rectangular subregion
	BYTE* m_pTopLeft;		// address of top left pixel of that region
	DWORD m_rowBytes;		// BYTE offset between adjacent rows

public:
	// get pointer to first pixel in a row
	BYTE* pRow (int row) const
		{ return m_pTopLeft + m_rowBytes*row; }

	// get pointer to a specific pixel
	BYTE* pPixel (int x, int y) const
		{ return pRow(y) + (DWORD)m_pixBytes*x; }

// Constructors
public:
	// default constructor establishes 0x0 PixRect
	CPixRect () { m_width = m_height = 0; m_pTopLeft = NULL; }

	// post-construction setup
	void Setup (WORD pixBytes, WORD w, WORD h, LPVOID pTL, DWORD rb)
		{ m_pixBytes = pixBytes; m_width = w; m_height = h;
		  m_pTopLeft = (BYTE*)pTL; m_rowBytes = rb; }
};
