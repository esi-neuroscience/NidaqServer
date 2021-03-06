
// nidaqServerDoc.cpp : implementation of the CnidaqServerDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
//#include "nidaqServer.h"
#endif

#include "nidaqServerDoc.h"
//#include "Reward.h"

//#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CnidaqServerDoc

CauxLine* CnidaqServerDoc::m_pauxLines[8];

IMPLEMENT_DYNCREATE(CnidaqServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CnidaqServerDoc, CDocument)
//	ON_COMMAND(ID_LOG_REWARDTRIGGERS, &CnidaqServerApp::OnLogRewardTriggers)
//	ON_UPDATE_COMMAND_UI(ID_LOG_REWARDTRIGGERS, &CnidaqServerApp::OnUpdateLogRewardTriggers)
	ON_COMMAND(ID_LOG_REWARDTRIGGERS, &CnidaqServerDoc::OnLogRewardTriggers)
	ON_UPDATE_COMMAND_UI(ID_LOG_REWARDTRIGGERS, &CnidaqServerDoc::OnUpdateLogRewardTriggers)
END_MESSAGE_MAP()


// CnidaqServerDoc construction/destruction

CnidaqServerDoc::CnidaqServerDoc()
//	: m_rewardTime(100)
//	, m_rewardCode(0)
{
	// TODO: add one-time construction code here
	m_pauxLines[3] = new CReward();
}

CnidaqServerDoc::~CnidaqServerDoc()
{
	delete m_pauxLines[3];
}

BOOL CnidaqServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CnidaqServerDoc serialization

void CnidaqServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CnidaqServerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CnidaqServerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CnidaqServerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CnidaqServerDoc diagnostics

#ifdef _DEBUG
void CnidaqServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CnidaqServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CnidaqServerDoc commands

void CnidaqServerDoc::OnLogRewardTriggers()
{
	// TODO: F�gen Sie hier Ihren Befehlsbehandlungscode ein.
	((CReward*)m_pauxLines[3])->m_logPulseTriggers = !((CReward*)m_pauxLines[3])->m_logPulseTriggers;
//	m_logRewardTriggers = !m_logRewardTriggers;
}


void CnidaqServerDoc::OnUpdateLogRewardTriggers(CCmdUI* pCmdUI)
{
	// TODO: F�gen Sie hier Ihren Befehlsaktualisierungs-UI-Behandlungscode ein.
	pCmdUI->SetCheck(((CReward*)m_pauxLines[3])->m_logPulseTriggers);
//	pCmdUI->SetCheck(m_logRewardTriggers);
}
