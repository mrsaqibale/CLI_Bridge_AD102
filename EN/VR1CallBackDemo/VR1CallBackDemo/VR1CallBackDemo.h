// VR1CallBackDemo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CVR1CallBackDemoApp:
// See VR1CallBackDemo.cpp for the implementation of this class
//

class CVR1CallBackDemoApp : public CWinApp
{
public:
	CVR1CallBackDemoApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CVR1CallBackDemoApp theApp;