#if !defined(AFX_SERVERDOC_H__41648631_FB1B_463B_8BC1_00307BACCC47__INCLUDED_)
#define AFX_SERVERDOC_H__41648631_FB1B_463B_8BC1_00307BACCC47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerDoc document

class CServerDoc : public CDocument
{
protected:
	CServerDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CServerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CServerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERDOC_H__41648631_FB1B_463B_8BC1_00307BACCC47__INCLUDED_)
