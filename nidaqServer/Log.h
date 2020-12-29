// Log.h: Schnittstelle für die Klasse CLog.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__BA69A0D4_598B_4712_99BD_3DCD7F14A274__INCLUDED_)
#define AFX_LOG_H__BA69A0D4_598B_4712_99BD_3DCD7F14A274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLog : protected CObject  
{
public:
	static void AddToLog(CString LogEntry);
	CLog();
	static CEdit* m_Edit;
	virtual ~CLog();

private:
};

#endif // !defined(AFX_LOG_H__BA69A0D4_598B_4712_99BD_3DCD7F14A274__INCLUDED_)
