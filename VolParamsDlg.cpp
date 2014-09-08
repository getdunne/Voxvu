#include "stdafx.h"
#include "Voxvu.h"
#include "VolParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

VolParamsDlg::VolParamsDlg(CWnd* pParent) : CDialog(VolParamsDlg::IDD, pParent)
{
	m_width = 0;
	m_height = 0;
	m_depth = 0;
	m_voxWidth = 0.0;
	m_voxHeight = 0.0;
	m_voxDepth = 0.0;
}


void VolParamsDlg::DoDataExchange(CDataExchange* pDX)
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
}


BEGIN_MESSAGE_MAP(VolParamsDlg, CDialog)
END_MESSAGE_MAP()
