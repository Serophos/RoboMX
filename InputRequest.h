#if !defined(AFX_INPUTREQUEST_H__E652540C_D046_4052_B91C_0061B6520A51__INCLUDED_)
#define AFX_INPUTREQUEST_H__E652540C_D046_4052_B91C_0061B6520A51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputRequest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputRequest dialog
#include "MyEdit.h"

class CInputRequest : public CDialog
{
// Construction
public:
	afx_msg LRESULT OnEnter(WPARAM w, LPARAM l);
	void SetMode(DWORD dwMode, CFont* pFont);
	CInputRequest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputRequest)
	enum { IDD = IDD_INPUT_REQUEST };
	CMyEdit	m_eInput;
	CString	m_strInput;
	BOOL	m_bConvert;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputRequest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD m_dwMode;
	CFont *m_pFont;

	// Generated message map functions
	//{{AFX_MSG(CInputRequest)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTREQUEST_H__E652540C_D046_4052_B91C_0061B6520A51__INCLUDED_)
