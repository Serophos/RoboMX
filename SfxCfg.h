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

#if !defined(AFX_SFXCFG_H__734BF1BA_75D9_4D7B_BAA1_CBCB6B2AF310__INCLUDED_)
#define AFX_SFXCFG_H__734BF1BA_75D9_4D7B_BAA1_CBCB6B2AF310__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SfxCfg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSfxCfg dialog
#include "CfgDlg.h"
#include "Settings.h"

class CSfxCfg : public CCfgDlg
{
// Construction
public:
	void WriteSounds();
	void LoadSounds();
	void Apply();
	CSfxCfg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSfxCfg)
	enum { IDD = IDD_SOUND };
	CListCtrl	m_lcSounds;
	CString	m_strConnect;
	CString	m_strDisconnect;
	CString	m_strMotd;
	CString	m_strRedirect;
	CString	m_strStart;
	CString	m_strError;
	CString	m_strTopic;
	BOOL	m_bChatSfx;
	BOOL	m_bSfx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSfxCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSfxCfg)
	afx_msg void OnConnected();
	afx_msg void OnDisconnected();
	afx_msg void OnMotd();
	afx_msg void OnRedirected();
	afx_msg void OnStartup();
	afx_msg void OnSyserror();
	afx_msg void OnTopic();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnPlay();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlayConnected();
	afx_msg void OnBnClickedPlayDisconnected();
	afx_msg void OnBnClickedPlayTopic();
	afx_msg void OnBnClickedPlayMotd();
	afx_msg void OnBnClickedPlayRedirected();
	afx_msg void OnBnClickedPlayStart();
	afx_msg void OnBnClickedPlayError();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SFXCFG_H__734BF1BA_75D9_4D7B_BAA1_CBCB6B2AF310__INCLUDED_)
