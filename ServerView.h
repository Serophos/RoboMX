#if !defined(AFX_SERVERVIEW_H__3DABFC5F_5DF8_41A4_959D_51A519E3F06C__INCLUDED_)
#define AFX_SERVERVIEW_H__3DABFC5F_5DF8_41A4_959D_51A519E3F06C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerView.h : header file
//
#include "MySocket.h"

/////////////////////////////////////////////////////////////////////////////
// CServerView view

class CServerView : public CRichEditView
{
protected:
	CServerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CServerView)

// Attributes
public:

// Operations
public:
	static UINT ClientThread(PVOID pParam);
	static UINT ServerThread(PVOID pParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerView)
	protected:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bListen;
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CServerView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERVIEW_H__3DABFC5F_5DF8_41A4_959D_51A519E3F06C__INCLUDED_)
