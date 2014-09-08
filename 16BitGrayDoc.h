#pragma once

#include "VoxelFilePair.h"

class C16BitGrayDoc : public VoxelFilePair
{
protected:
	C16BitGrayDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C16BitGrayDoc)

// Overrides
public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
protected:
	virtual BOOL OnNewDocument();

// Implementation
public:
	virtual ~C16BitGrayDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
