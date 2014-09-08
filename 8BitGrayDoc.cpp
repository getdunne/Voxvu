#include "stdafx.h"
#include "Voxvu.h"

#include "8BitGrayDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(C8BitGrayDoc, VoxelFilePair)

C8BitGrayDoc::C8BitGrayDoc()
{
	m_kind = k8BitGray;
}

BOOL C8BitGrayDoc::OnNewDocument()
{
	m_array.m_voxBytes = 1;

	if (!VoxelFilePair::OnNewDocument())
		return FALSE;

	// set up color table with 256 entries
	m_colors = 256;
	for (int i=0; i<256; i++) {
		m_color[i].rgbRed = m_color[i].rgbGreen = m_color[i].rgbBlue = i;
		m_color[i].rgbReserved = 0;
	}

	return TRUE;
}

C8BitGrayDoc::~C8BitGrayDoc()
{
}


BEGIN_MESSAGE_MAP(C8BitGrayDoc, VoxelFilePair)
END_MESSAGE_MAP()

#ifdef _DEBUG
void C8BitGrayDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void C8BitGrayDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void C8BitGrayDoc::Serialize(CArchive& ar)
{
	VoxelFilePair::Serialize(ar);
}
