#include "stdafx.h"
#include "Voxvu.h"
#include "15BitRGBDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(C15BitRGBDoc, VoxelFilePair)

C15BitRGBDoc::C15BitRGBDoc()
{
	m_kind = k15BitRGB;
}

BOOL C15BitRGBDoc::OnNewDocument()
{
	m_array.m_voxBytes = 2;

	if (!VoxelFilePair::OnNewDocument())
		return FALSE;

	m_colors = 0;

	return TRUE;
}

C15BitRGBDoc::~C15BitRGBDoc()
{
}


BEGIN_MESSAGE_MAP(C15BitRGBDoc, VoxelFilePair)
END_MESSAGE_MAP()

#ifdef _DEBUG
void C15BitRGBDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void C15BitRGBDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void C15BitRGBDoc::Serialize(CArchive& ar)
{
	VoxelFilePair::Serialize(ar);
}
