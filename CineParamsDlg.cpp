#include "stdafx.h"
#include "Voxvu.h"
#include "CineParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CineParmsDlg::CineParmsDlg(CWnd* pParent) : CDialog(CineParmsDlg::IDD, pParent)
{
	m_width = 0;
	m_height = 0;
	m_depth = 0;
	m_voxWidth = 0.0;
	m_voxHeight = 0.0;
	m_voxDepth = 0.0;
	m_fileNamePattern = _T("");
	m_fileNumLast = 0;
	m_fileNumFirst = 0;
	m_fileNumInc = 0;
	m_skipBytes = 0;
}


void CineParmsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ADX, m_width);
	DDV_MinMaxUInt(pDX, m_width, 1, 1000);
	DDX_Text(pDX, IDC_ADY, m_height);
	DDV_MinMaxUInt(pDX, m_height, 1, 1000);
	DDX_Text(pDX, IDC_ADZ, m_depth);
	DDV_MinMaxUInt(pDX, m_depth, 1, 1000);
	DDX_Text(pDX, IDC_VDX, m_voxWidth);
	DDV_MinMaxDouble(pDX, m_voxWidth, 1.e-006, 1000000.);
	DDX_Text(pDX, IDC_VDY, m_voxHeight);
	DDV_MinMaxDouble(pDX, m_voxHeight, 1.e-006, 1000000.);
	DDX_Text(pDX, IDC_VDZ, m_voxDepth);
	DDV_MinMaxDouble(pDX, m_voxDepth, 1.e-006, 1000000.);
	DDX_Text(pDX, IDC_FNP, m_fileNamePattern);
	DDV_MaxChars(pDX, m_fileNamePattern, 40);
	DDX_Text(pDX, IDC_FSE, m_fileNumLast);
	DDX_Text(pDX, IDC_FSS, m_fileNumFirst);
	DDX_Text(pDX, IDC_FSI, m_fileNumInc);
	DDX_Text(pDX, IDC_HDRLEN, m_skipBytes);
}


BEGIN_MESSAGE_MAP(CineParmsDlg, CDialog)
END_MESSAGE_MAP()
