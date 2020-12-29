
// nidaqServerDoc.h : interface of the CnidaqServerDoc class
//


#pragma once
#include "resource.h"       // main symbols
#include "Reward.h"

class CnidaqServerDoc : public CDocument
{
protected: // create from serialization only
	CnidaqServerDoc();
	DECLARE_DYNCREATE(CnidaqServerDoc)

// Attributes
public:
//	short m_rewardTime;
//	short m_rewardCode;
	static CauxLine* m_pauxLines[8];

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CnidaqServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLogRewardTriggers();
	afx_msg void OnUpdateLogRewardTriggers(CCmdUI* pCmdUI);

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
