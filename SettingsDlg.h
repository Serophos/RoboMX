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

#if !defined(AFX_SETTINGSDLG_H__6AA0ABC2_6436_4465_9C81_3100EBD1FFBA__INCLUDED_)
#define AFX_SETTINGSDLG_H__6AA0ABC2_6436_4465_9C81_3100EBD1FFBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDlg.h : header file
//

#include "CfgDlg.h"
#include "GeneralCfg.h"
#include "ColorCfg.h"
#include "MessageCfg.h"
#include "SfxCfg.h"
#include "FunCfg.h"
#include "PeerCfg.h"

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
// Construction
public:
	void AddCategory(CCfgDlg* pCat, int nICon, LPCTSTR lpszName);
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS };
	CListCtrl	m_lcCat;
	//}}AFX_DATA

	CImageList m_cImageList;
	CGeneralCfg cfgGen;
	CColorCfg cfgColor;
	CMessageCfg cfgMsg;
	CSfxCfg  cfgSfx;
	CFunCfg  cfgFun;
	CPeerCfg cfgPeer;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPtrArray m_aDlgs;
	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnItemchangedCat(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__6AA0ABC2_6436_4465_9C81_3100EBD1FFBA__INCLUDED_)
