#if !defined(AFX_COLORSTATUSBAR_H__7C404582_D184_4405_ADAD_8A1F1AACA3A5__INCLUDED_)
#define AFX_COLORSTATUSBAR_H__7C404582_D184_4405_ADAD_8A1F1AACA3A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatusBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar window

class CColorStatusBar : public CStatusBar
{
// Construction
public:
	CColorStatusBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatusBar)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetLagColor(COLORREF cr, CString strLag);
	virtual ~CColorStatusBar();

protected:
	COLORREF m_crLag;
	CString  m_strLag;

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatusBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSTATUSBAR_H__7C404582_D184_4405_ADAD_8A1F1AACA3A5__INCLUDED_)
