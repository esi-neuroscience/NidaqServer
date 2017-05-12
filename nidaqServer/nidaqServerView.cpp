
// nidaqServerView.cpp : implementation of the CnidaqServerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "nidaqServer.h"
#endif

#include "nidaqServerDoc.h"
#include "nidaqServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CnidaqServerView

IMPLEMENT_DYNCREATE(CnidaqServerView, CFormView)

BEGIN_MESSAGE_MAP(CnidaqServerView, CFormView)
END_MESSAGE_MAP()

// CnidaqServerView construction/destruction

CnidaqServerView::CnidaqServerView()
	: CFormView(CnidaqServerView::IDD)
{
	// TODO: add construction code here

}

CnidaqServerView::~CnidaqServerView()
{
}

void CnidaqServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CnidaqServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CnidaqServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CnidaqServerView diagnostics

#ifdef _DEBUG
void CnidaqServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CnidaqServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CnidaqServerDoc* CnidaqServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CnidaqServerDoc)));
	return (CnidaqServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CnidaqServerView message handlers
