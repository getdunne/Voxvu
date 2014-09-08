
// TestDataFileMaker.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// TestDataFileMakerApp:
// See TestDataFileMaker.cpp for the implementation of this class
//

class TestDataFileMakerApp : public CWinApp
{
public:
	TestDataFileMakerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern TestDataFileMakerApp theApp;