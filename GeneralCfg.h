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

#if !defined(AFX_GENERALCFG_H__7BDDF385_DDD5_4828_8B0F_B09A53064E9C__INCLUDED_)
#define AFX_GENERALCFG_H__7BDDF385_DDD5_4828_8B0F_B09A53064E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralCfg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneralCfg dialog
#include "CfgDlg.h"

class CGeneralCfg : public CCfgDlg
{
// Construction
public:
	void SaveRooms();
	void Apply();
	void LoadRooms();
	CGeneralCfg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneralCfg)
	enum { IDD = IDD_GENERAL };
	CListCtrl	m_lcPlugins;
	CListBox	m_lbChannels;
	CComboBox	m_cbLang;
	CString	m_strLanguage;
	int		m_nHistory;
	BOOL	m_bHistory;
	BOOL	m_bAllChannels;
	CString	m_strChannel;
	CString	m_strGreeting;
	BOOL	m_bLimit;
	int		m_nLimit;
	BOOL	m_bTimeStamp;
	int		m_nTime;
	BOOL	m_bPing;
	BOOL	m_bMiniTray;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralCfg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAllchannels();
	afx_msg void OnAddchannel();
	afx_msg void OnRemovechannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALCFG_H__7BDDF385_DDD5_4828_8B0F_B09A53064E9C__INCLUDED_)
