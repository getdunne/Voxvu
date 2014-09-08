#pragma once

#include "VoxelFilePair.h"

class C8BitGrayDoc : public VoxelFilePair
{
protected:
	C8BitGrayDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C8BitGrayDoc)

// Overrides
public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
protected:
	virtual BOOL OnNewDocument();

// Implementation
public:
	virtual ~C8BitGrayDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
