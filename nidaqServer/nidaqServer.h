
// nidaqServer.h : main header file for the nidaqServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CnidaqServerApp:
// See nidaqServer.cpp for the implementation of this class
//

class CnidaqServerApp : public CWinApp
{
public:
	CnidaqServerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CnidaqServerApp theApp;
