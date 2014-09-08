#pragma once

#include "VoxelFilePair.h"

class C15BitRGBDoc : public VoxelFilePair
{
protected:
	C15BitRGBDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C15BitRGBDoc)

// Overrides
public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
protected:
	virtual BOOL OnNewDocument();

// Implementation
public:
	virtual ~C15BitRGBDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
