#include "stdafx.h"
#include "Voxvu.h"

#include "16BitGrayDoc.h"

#include "CineParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(C16BitGrayDoc, VoxelFilePair)

C16BitGrayDoc::C16BitGrayDoc()
{
	m_kind = k16BitGray;
}

BOOL C16BitGrayDoc::OnNewDocument()
{
	m_array.m_voxBytes = 2;

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

C16BitGrayDoc::~C16BitGrayDoc()
{
}


BEGIN_MESSAGE_MAP(C16BitGrayDoc, VoxelFilePair)
END_MESSAGE_MAP()

#ifdef _DEBUG
void C16BitGrayDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void C16BitGrayDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void C16BitGrayDoc::Serialize(CArchive& ar)
{
	VoxelFilePair::Serialize(ar);
}
