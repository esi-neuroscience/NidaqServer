#include "StdAfx.h"
#include "Lever.h"


CLever::CLever(void)
{
//	VERIFY(m_hPressEvent = CreateEvent(NULL, FALSE, FALSE, _T("LeverPress")));
	VERIFY(m_hReleaseEvent = CreateEvent(NULL, FALSE, TRUE, _T("LeverRelease")));
	m_lever.Start();
	TRACE("CLever Konstruktor\n");
}


CLever::~CLever(void)
{
//	VERIFY(CloseHandle(m_hPressEvent));
	VERIFY(CloseHandle(m_hReleaseEvent));
}
