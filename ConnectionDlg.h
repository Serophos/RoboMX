/*
** Copyright (C) 2004 Thees Winkler
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#if !defined(AFX_CONNECTIONDLG_H__E858A9B9_6AAD_4E5D_BAC3_8C64FF7804FE__INCLUDED_)
#define AFX_CONNECTIONDLG_H__E858A9B9_6AAD_4E5D_BAC3_8C64FF7804FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog

class CConnectionDlg : public CDialog
{
// Construction
public:
	CString m_strWd;
	CConnectionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectionDlg)
	enum { IDD = IDD_CONNECT };
	CComboBox	m_cbRoom;
	int		m_nLine;
	DWORD	m_dwFiles;
	CString	m_strName;
	CString	m_strRoom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectionDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnUsenode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONDLG_H__E858A9B9_6AAD_4E5D_BAC3_8C64FF7804FE__INCLUDED_)
