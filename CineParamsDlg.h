#pragma once

class CineParmsDlg : public CDialog
{
// Construction
public:
	CineParmsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_CINEDLG };
	UINT	m_width;
	UINT	m_height;
	UINT	m_depth;
	double	m_voxWidth;
	double	m_voxHeight;
	double	m_voxDepth;
	CString	m_fileNamePattern;
	UINT	m_fileNumLast;
	UINT	m_fileNumFirst;
	UINT	m_fileNumInc;
	UINT	m_skipBytes;
	//}}AFX_DATA


// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
};
