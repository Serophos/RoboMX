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

#if !defined(AFX_SETTINGSVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)
#define AFX_SETTINGSVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CSettingsViewh : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsView view

#include "ColorStatusBar.h"
#include "CfgDlg.h"
#include "GeneralCfg.h"
#include "ColorCfg.h"
#include "MessageCfg.h"
#include "SfxCfg.h"
#include "FunCfg.h"
#include "PeerCfg.h"
#include "ExtCfg.h"

class CSettingsView : public CFormView
{
protected:
	CSettingsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSettingsView)

// Attributes
public:
	enum { IDD = IDD_SETTINGS };
	void AddCategory(CCfgDlg* pCat, int nICon, LPCTSTR lpszName);

// Operations
public:
	CColorStatusBar*	 m_pStatusBar;

public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void DoDataExchange(CDataExchange* pDX);

// Implementation
protected:
	virtual ~CSettingsView();

	CListCtrl	m_lcCat;
	CImageList m_cImageList;
	CGeneralCfg cfgGen;
	CColorCfg cfgColor;
	CMessageCfg cfgMsg;
	CExtCfg  cfgExt;
	CSfxCfg  cfgSfx;
	CFunCfg  cfgFun;
	CPeerCfg cfgPeer;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	CPtrArray m_aDlgs;
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnItemchangedCat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_SETTINGSVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)
