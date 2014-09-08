#pragma once

#include "VoxelArray.h"
#include "Matrix4x4.h"

/////////////////////////////////////////////////////////////////////////////
// CFileIOProgress dialog

class CFileIOProgress : public CDialog
{
// Construction
public:
	CFileIOProgress(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_READ_PROGRESS };

// Operations
	void Create (char* title);
	void Destroy (void);
	void SetMaxPos (int max);
	void SetPos (int pos);

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

typedef enum {
	k8BitGray,
	k8BitColor,
	k8BitHybrid,
	k12BitGray,
	k16BitGray,
	k15BitRGB
} EVoxelKind;

class VoxelFilePair : public CDocument
{
protected: // create from serialization only
	VoxelFilePair();
	DECLARE_DYNCREATE(VoxelFilePair)

// Attributes
public:
	VoxelArray m_array;	// voxel array
	EVoxelKind m_kind;		// voxel value interpretation
	Matrix4x4 m_A;			// coordinate transformation matrix
	int m_colors;			// number of significant entries
	RGBQUAD m_color[256];	//  in this color table

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~VoxelFilePair();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
