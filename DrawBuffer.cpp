#include "stdafx.h"

#include "DrawBuffer.h"


//--------------------------------------------------------------
// default constructor: establish "invalid" state
//--------------------------------------------------------------
DrawBuffer::DrawBuffer ()
{
	m_pbmi = NULL;
	m_hbm = NULL;
	m_pmDC = NULL;
}

//--------------------------------------------------------------
// draw image to given DC with top-left corner at given coords
//--------------------------------------------------------------
void DrawBuffer::Draw (CDC* pDC, int left, int top)
{
	ASSERT(m_pbmi!=NULL && m_hbm!=NULL && m_pmDC!=NULL);
	pDC->BitBlt(left,top,m_width,m_height,m_pmDC,0,0,SRCCOPY);
}

//--------------------------------------------------------------
// stretch image to given DC with top-left corner at given coords
// and total output size as given
//--------------------------------------------------------------
void DrawBuffer::Draw (CDC* pDC, CPoint topLeft, CSize size)
{
	ASSERT(m_pbmi!=NULL && m_hbm!=NULL && m_pmDC!=NULL);
	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	pDC->StretchBlt(topLeft.x,topLeft.y,size.cx,size.cy,m_pmDC,
		0,0,m_width,m_height,SRCCOPY);
}

//--------------------------------------------------------------
// post-construction Create() function: establishes memory DC
// and DIB section
//--------------------------------------------------------------
BOOL DrawBuffer::Create (
	CDC* pDC, int width, int height,
	int nBitsPerPixel,
	int nColors /*=0*/, LPRGBQUAD lpQuadTable/*=NULL*/)
{
	ASSERT(m_pbmi==NULL && m_hbm==NULL && m_pmDC==NULL);
	ASSERT(!(nColors!=0 && lpQuadTable==NULL));
	ASSERT(nBitsPerPixel==8 || nBitsPerPixel==16 || nBitsPerPixel==32);

	// set up our memory DC
	m_pmDC = new CDC;
	if (m_pmDC->CreateCompatibleDC(pDC) == FALSE) return FALSE;

	// set up our DIB info header and color table
	m_pbmi = (BITMAPINFO*) new char
				[sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];
	if (m_pbmi==NULL) return FALSE;
	m_pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pbmi->bmiHeader.biWidth = width;
	m_pbmi->bmiHeader.biHeight = -height;	// origin at top left
	m_pbmi->bmiHeader.biPlanes = 1;
	m_pbmi->bmiHeader.biBitCount = nBitsPerPixel;
	m_pbmi->bmiHeader.biCompression = BI_RGB;
	m_pbmi->bmiHeader.biSizeImage = 0;
	m_pbmi->bmiHeader.biXPelsPerMeter = 0;
	m_pbmi->bmiHeader.biYPelsPerMeter = 0;
	m_pbmi->bmiHeader.biClrUsed = nColors;
	m_pbmi->bmiHeader.biClrImportant = nColors;
	for (int i=0; i<nColors; ++i)
		m_pbmi->bmiColors[i] = lpQuadTable[i];

	// create DIB section itself
	m_hbm = ::CreateDIBSection(m_pmDC->GetSafeHdc(),m_pbmi,
								DIB_RGB_COLORS,(VOID**)&m_pTopLeft,NULL,0);
	if (m_hbm == NULL) return FALSE;

	// select the DIB section as the bitmap of the memory DC
	m_pmDC->SelectObject(m_hbm);

	// set up the CPixRect
	int nBytesPerPixel = nBitsPerPixel/8;
	int rowBytes = (width*nBytesPerPixel+3)&~3;
	Setup(nBytesPerPixel,width,height,m_pTopLeft,rowBytes);

	return TRUE;
}

//--------------------------------------------------------------
// Deallocate all attached structures
//--------------------------------------------------------------
void DrawBuffer::Empty ()
{
	if (m_pbmi != NULL) delete[] m_pbmi;
	if (m_hbm != NULL) ::DeleteObject(m_hbm);
	if (m_pmDC != NULL) delete m_pmDC;
}

//--------------------------------------------------------------
// after Create() has been called, Resize() may be called any
// number of times to change the size of the DIB
//--------------------------------------------------------------
BOOL DrawBuffer::Resize (int width, int height)
{
	ASSERT(m_pbmi!=NULL && m_hbm!=NULL && m_pmDC!=NULL);

	::DeleteObject(m_hbm);
	m_pbmi->bmiHeader.biWidth = width;
	m_pbmi->bmiHeader.biHeight = -height;	// origin at top left
	m_hbm = ::CreateDIBSection(m_pmDC->GetSafeHdc(),m_pbmi,
								DIB_RGB_COLORS,(VOID**)&m_pTopLeft,NULL,0);
	m_pmDC->SelectObject(m_hbm);
	int rowBytes = (width*m_pixBytes+3)&~3;
	Setup(m_pixBytes,width,height,m_pTopLeft,rowBytes);
	return (m_hbm != NULL);
}
