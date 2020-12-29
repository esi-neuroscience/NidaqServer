// Log.cpp: Implementierung der Klasse CLog.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{

}



CLog::~CLog()
{

}

void CLog::AddToLog(CString LogEntry)
{
	CString strTemp;
	m_Edit->GetWindowText (strTemp);
	strTemp += CTime::GetCurrentTime().Format(_T("\r\n%H:%M:%S ")) + LogEntry;
	m_Edit->SetWindowText (strTemp);
	m_Edit->LineScroll(m_Edit->GetLineCount(),0);

}

