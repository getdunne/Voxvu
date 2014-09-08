#include "stdafx.h"
#include "TestDataFileMaker.h"
#include "TestDataFileMakerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


TestDataFileMakerDlg::TestDataFileMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(TestDataFileMakerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void TestDataFileMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TestDataFileMakerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &TestDataFileMakerDlg::OnBnClickedCreate8Bit)
	ON_BN_CLICKED(IDC_BUTTON2, &TestDataFileMakerDlg::OnBnClickedCreate16Bit)
	ON_BN_CLICKED(IDC_BUTTON3, &TestDataFileMakerDlg::OnBnClickedCreate15Bit)
END_MESSAGE_MAP()


BOOL TestDataFileMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void TestDataFileMakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR TestDataFileMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void TestDataFileMakerDlg::OnBnClickedCreate8Bit()
{
	CFileDialog dlg(FALSE, "8gy", "test_8bit", OFN_OVERWRITEPROMPT, "8-bit Gray (*.8gy)|*.8gy|All Files (*.*)|*.*||");
	if (dlg.DoModal() != IDOK) return;

	CString path(dlg.GetPathName());

	CFile headerFile(path, CFile::modeCreate|CFile::modeWrite);
	CArchive hdr(&headerFile, CArchive::store);

	const WORD bytesPerVoxel = 1;
	const WORD arrayWidth = 128;
	const WORD arrayHeight = 64;
	const WORD arrayDepth = 32;		// aka number of slices
	const double voxelWidth = 1.0;
	const double voxelHeight = 1.0;
	const double voxelDepth = 2.0;	// aka slice thickness

	hdr << bytesPerVoxel;
	hdr << arrayWidth;
	hdr << arrayHeight;
	hdr << arrayDepth;
	hdr << voxelWidth;
	hdr << voxelHeight;
	hdr << voxelDepth;

	// 256-entry color table
	hdr << (int)256;
	for (int i=0; i<256; ++i)
	{
		hdr << (BYTE)i;
		hdr << (BYTE)i;
		hdr << (BYTE)i;
	}

	hdr.Close();
	headerFile.Close();

	CFile dataFile(path.Left(path.ReverseFind('.')) + ".dat", CFile::modeCreate|CFile::modeWrite);
	CArchive dat(&dataFile, CArchive::store);
	for (int slice=0; slice < arrayDepth; slice++)
	{
		for (int row=0; row < arrayHeight; row++)
		{
			for (int col=0; col < arrayWidth; col++)
			{
				dat << (BYTE)(col & 0xFF);
			}
		}
	}
	dat.Close();
	dataFile.Close();

	MessageBox(".dat and .8gy files created", "Confirmation", MB_OK);
}

void TestDataFileMakerDlg::OnBnClickedCreate16Bit()
{
	CFileDialog dlg(FALSE, "16g", "test_16bit", OFN_OVERWRITEPROMPT, "16-bit Gray (*.16g)|*.16g|All Files (*.*)|*.*||");
	if (dlg.DoModal() != IDOK) return;

	CString path(dlg.GetPathName());

	CFile headerFile(path, CFile::modeCreate|CFile::modeWrite);
	CArchive hdr(&headerFile, CArchive::store);

	const WORD bytesPerVoxel = 2;
	const WORD arrayWidth = 128;
	const WORD arrayHeight = 64;
	const WORD arrayDepth = 32;		// aka number of slices
	const double voxelWidth = 1.0;
	const double voxelHeight = 1.0;
	const double voxelDepth = 2.0;	// aka slice thickness

	hdr << bytesPerVoxel;
	hdr << arrayWidth;
	hdr << arrayHeight;
	hdr << arrayDepth;
	hdr << voxelWidth;
	hdr << voxelHeight;
	hdr << voxelDepth;

	// 256-entry color table
	hdr << (int)256;
	for (int i=0; i<256; ++i)
	{
		hdr << (BYTE)i;
		hdr << (BYTE)i;
		hdr << (BYTE)i;
	}

	hdr.Close();
	headerFile.Close();

	CFile dataFile(path.Left(path.ReverseFind('.')) + ".dat", CFile::modeCreate|CFile::modeWrite);
	CArchive dat(&dataFile, CArchive::store);
	for (int slice=0; slice < arrayDepth; slice++)
	{
		for (int row=0; row < arrayHeight; row++)
		{
			for (int col=0; col < arrayWidth; col++)
			{
				WORD value = (col % 0xFF) << 8;
				// 16-bit data are stored in big-endian order
				dat << HIBYTE(value);
				dat << LOBYTE(value);
			}
		}
	}
	dat.Close();
	dataFile.Close();

	MessageBox(".dat and .16g files created", "Confirmation", MB_OK);
}

void TestDataFileMakerDlg::OnBnClickedCreate15Bit()
{
	CFileDialog dlg(FALSE, "15c", "test_rgb", OFN_OVERWRITEPROMPT, "15-bit RGB (*.15c)|*.15c|All Files (*.*)|*.*||");
	if (dlg.DoModal() != IDOK) return;

	CString path(dlg.GetPathName());

	CFile headerFile(path, CFile::modeCreate|CFile::modeWrite);
	CArchive hdr(&headerFile, CArchive::store);

	const WORD bytesPerVoxel = 2;
	const WORD arrayWidth = 128;
	const WORD arrayHeight = 64;
	const WORD arrayDepth = 32;		// aka number of slices
	const double voxelWidth = 1.0;
	const double voxelHeight = 1.0;
	const double voxelDepth = 2.0;	// aka slice thickness

	hdr << bytesPerVoxel;
	hdr << arrayWidth;
	hdr << arrayHeight;
	hdr << arrayDepth;
	hdr << voxelWidth;
	hdr << voxelHeight;
	hdr << voxelDepth;

	// no color table required, #colors = 0
	hdr << (int)0;

	hdr.Close();
	headerFile.Close();

	CFile dataFile(path.Left(path.ReverseFind('.')) + ".dat", CFile::modeCreate|CFile::modeWrite);
	CArchive dat(&dataFile, CArchive::store);
	for (int slice=0; slice < arrayDepth; slice++)
	{
		for (int row=0; row < arrayHeight; row++)
		{
			for (int col=0; col < arrayWidth; col++)
			{
				int r = col & 0x1F;
				int g = row & 0x1F;
				int b = 0x10;
				WORD rgb = (WORD)((r << 10) | (g << 5) | (b << 0));
				// 16-bit data are stored in big-endian order
				dat << HIBYTE(rgb);
				dat << LOBYTE(rgb);
			}
		}
	}
	dat.Close();
	dataFile.Close();

	MessageBox(".dat and .15c files created", "Confirmation", MB_OK);
}
