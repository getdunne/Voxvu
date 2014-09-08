#pragma once

class VolParamsDlg : public CDialog
{
// Construction
public:
	VolParamsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_VOLDLG };
	UINT	m_width;
	UINT	m_height;
	UINT	m_depth;
	double	m_voxWidth;
	double	m_voxHeight;
	double	m_voxDepth;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
};
