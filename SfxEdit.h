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

#if !defined(AFX_SFXEDIT_H__3DFD8812_A588_48EC_8980_03EEA7694D9A__INCLUDED_)
#define AFX_SFXEDIT_H__3DFD8812_A588_48EC_8980_03EEA7694D9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SfxEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSfxEdit dialog

class CSfxEdit : public CDialog
{
// Construction
public:
	CSfxEdit(CWnd* pParent = NULL);   // standard constructor
	void GetValues(int& nEvent, CString& strTrigger, CString& strReaction);
	void Init(int nEventType, CString strTrigger, CString strReaction);

// Dialog Data
	//{{AFX_DATA(CSfxEdit)
	enum { IDD = IDD_SFX_EDIT };
	CEdit	m_wndTrigger;
	CEdit	m_wndRaction;
	CComboBoxEx	m_cbEvent;
	CButton	m_wndBrowseBtn;
	int		m_nEvent;
	CString	m_strReaction;
	CString	m_strTrigger;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSfxEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSfxEdit)
	afx_msg void OnPlay();
	afx_msg void OnBrowseSound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SFXEDIT_H__3DFD8812_A588_48EC_8980_03EEA7694D9A__INCLUDED_)
