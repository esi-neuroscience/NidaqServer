
// nidaqServerView.h : interface of the CnidaqServerView class
//

#pragma once

#include "resource.h"


class CnidaqServerView : public CFormView
{
protected: // create from serialization only
	CnidaqServerView();
	DECLARE_DYNCREATE(CnidaqServerView)

public:
	enum{ IDD = IDD_NIDAQSERVER_FORM };

// Attributes
public:
	CnidaqServerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CnidaqServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
//	CString m_logText;
	CEdit m_EditLog;
};

#ifndef _DEBUG  // debug version in nidaqServerView.cpp
inline CnidaqServerDoc* CnidaqServerView::GetDocument() const
   { return reinterpret_cast<CnidaqServerDoc*>(m_pDocument); }
#endif

