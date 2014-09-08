#include "stdafx.h"
#include "Voxvu.h"

#include "VoxelFilePair.h"

#include "VolParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileIOProgress dialog

CFileIOProgress::CFileIOProgress(CWnd* pParent) : CDialog(CFileIOProgress::IDD, pParent)
{
}


void CFileIOProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileIOProgress, CDialog)
END_MESSAGE_MAP()


void CFileIOProgress::Create (char* title)
{
	CDialog::Create(IDD);
	SetWindowText(title);
}

void CFileIOProgress::Destroy (void)
{
	DestroyWindow();
}

void CFileIOProgress::SetMaxPos (int max)
{
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetRange(0,max);
}

void CFileIOProgress::SetPos (int pos)
{
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetPos(pos);
}

BOOL CFileIOProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS1))->SetPos(0);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// VoxelFilePair

IMPLEMENT_DYNCREATE(VoxelFilePair, CDocument)

BEGIN_MESSAGE_MAP(VoxelFilePair, CDocument)
END_MESSAGE_MAP()

VoxelFilePair::VoxelFilePair()
{
}

VoxelFilePair::~VoxelFilePair()
{
}

BOOL VoxelFilePair::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// solicit voxel file name from user
	CFileDialog fdlg(TRUE);
	if (fdlg.DoModal()!=IDOK) return FALSE;
	CString voxelFile = fdlg.GetPathName();

	WORD width,height,depth;
	double voxWidth,voxHeight,voxDepth;
	VolParamsDlg dlg;
	dlg.DoModal();
	width = dlg.m_width;
	height = dlg.m_height;
	depth = dlg.m_depth;
	voxWidth = dlg.m_voxWidth;
	voxHeight = dlg.m_voxHeight;
	voxDepth = dlg.m_voxDepth;
	VERIFY(m_array.Allocate(m_array.m_voxBytes,width,height,depth,
			voxWidth,voxHeight,voxDepth));
	CFile file; DWORD nBytesRead;
	VERIFY(file.Open(voxelFile,CFile::modeRead));
	nBytesRead = file.Read(m_array.pSlice(0),m_array.GetArrayLength());
	ASSERT(nBytesRead == m_array.GetArrayLength());
	file.Close();
	m_A.MakeDilTrans(1.0/voxWidth,1.0/voxHeight,1.0/voxDepth,
					 (width-1.)/2.,(height-1.)/2.,(depth-1.)/2.);

	return TRUE;
}

void VoxelFilePair::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{ // saving
		// set up progress dialog
		CFileIOProgress progressDlg;
		CString fname = ar.GetFile()->GetFileTitle();
		char title[100];
		sprintf(title,"Writing %s.dat...",fname.Left(fname.GetLength()-4));
		progressDlg.Create(title);

		ar << m_array.GetBytesPerVoxel();
		ar << m_array.GetArrayWidth();
		ar << m_array.GetArrayHeight();
		ar << m_array.GetArrayDepth();
		ar << m_array.GetVoxelWidth();
		ar << m_array.GetVoxelHeight();
		ar << m_array.GetVoxelDepth();
		ar << (WORD)m_colors;
		for (int i=0; i<m_colors; ++i) {
			ar << m_color[i].rgbRed;
			ar << m_color[i].rgbGreen;
			ar << m_color[i].rgbBlue;
		}
		// form voxel (.dat) file path based on archive path
		CString voxelFile = ar.GetFile()->GetFilePath();
		int nDotInd = voxelFile.ReverseFind('.');
		voxelFile = voxelFile.Left(nDotInd) + ".dat";
		// save voxel data
		CFile file;
		VERIFY(file.Open(voxelFile,CFile::modeCreate|CFile::modeWrite));
		file.SetLength(m_array.GetArrayLength());
		file.SeekToBegin();
		progressDlg.SetMaxPos(m_array.GetArrayDepth());
		DWORD sliceBytes = (DWORD)m_array.GetBytesPerVoxel()
							* m_array.GetArrayWidth() * m_array.GetArrayHeight();
		for (int slice=0; slice < m_array.GetArrayDepth(); ++slice) {
			file.Write(m_array.pSlice(slice),sliceBytes);
			progressDlg.SetPos(slice);
		}
		file.Close();
	}
	else
	{ // loading
		// set up progress dialog
		CFileIOProgress progressDlg;
		CString fname = ar.GetFile()->GetFileTitle();
		char title[100];
		sprintf(title,"Reading %s.dat...",fname.Left(fname.GetLength()-4));
		progressDlg.Create(title);

		WORD bytesPerVoxel;
		WORD width,height,depth;
		double voxWidth,voxHeight,voxDepth;
		WORD colors;

		ar >> bytesPerVoxel;
		ar >> width;
		ar >> height;
		ar >> depth;
		ar >> voxWidth;
		ar >> voxHeight;
		ar >> voxDepth;
		
		MEMORYSTATUS ms;
		ms.dwLength = sizeof(MEMORYSTATUS);
		::GlobalMemoryStatus(&ms);
		char szMsg[500];
		sprintf(szMsg,
			"Total RAM: %.1f Mb\n"\
			"Available: %.1f Mb\n"\
			" Required: %.1f Mb\n",
			ms.dwTotalPhys/1048576.,
			ms.dwAvailPhys/1048576.,
			((DWORD)bytesPerVoxel*width*height*depth)/1048576.);
		//MessageBox(NULL,szMsg,"Loading",MB_OK|MB_ICONINFORMATION);

		VERIFY(m_array.Allocate(bytesPerVoxel,width,height,depth,
				voxWidth,voxHeight,voxDepth));
		ar >> colors; m_colors = colors;
		for (int i=0; i<m_colors; ++i) {
			ar >> m_color[i].rgbRed;
			ar >> m_color[i].rgbGreen;
			ar >> m_color[i].rgbBlue;
			m_color[i].rgbReserved = 0;
		}

		// form voxel (.dat) file path based on archive path
		CString voxelFile = ar.GetFile()->GetFilePath();
		int nDotInd = voxelFile.ReverseFind('.');
		voxelFile = voxelFile.Left(nDotInd) + ".dat";
		// load voxel data
		progressDlg.SetMaxPos(depth);
		DWORD sliceBytes = (DWORD)bytesPerVoxel * width * height;
		CFile file; DWORD nBytesRead;
		VERIFY(file.Open(voxelFile,CFile::modeRead));
		for (int slice=0; slice < depth; ++slice) {
			nBytesRead = file.Read(m_array.pSlice(slice),sliceBytes);
			ASSERT(nBytesRead == sliceBytes);
			progressDlg.SetPos(slice);
		}
		file.Close();
		m_A.MakeDilTrans(1.0/voxWidth,1.0/voxHeight,1.0/voxDepth,
			(float(width)-1.)/2.,(float(height)-1.)/2.,
			(float(depth)-1.)/2.);
	}
}

#ifdef _DEBUG
void VoxelFilePair::AssertValid() const
{
	CDocument::AssertValid();
}

void VoxelFilePair::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
