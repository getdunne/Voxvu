
// TestDataFileMakerDlg.h : header file
//

#pragma once


// TestDataFileMakerDlg dialog
class TestDataFileMakerDlg : public CDialogEx
{
// Construction
public:
	TestDataFileMakerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTDATAFILEMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreate8Bit();
	afx_msg void OnBnClickedCreate16Bit();
	afx_msg void OnBnClickedCreate15Bit();
};
