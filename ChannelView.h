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

#if !defined(AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)
#define AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChannelView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChannelView view

#include "ListClient.h"

class CChannelView : public CFormView
{
protected:
	CChannelView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CChannelView)

// Attributes
public:
	//{{AFX_DATA(CMetis3View)
	enum { IDD = IDD_CHANNELLIST };
	CListCtrl m_lcList;
	CString   m_strSearch;
	CString   m_strMsg;
	//}}AFX_DATA

// Operations
public:
	void UpdateRoomItem(LPCTSTR lpszRoomName, WORD wUsers, WORD wLimit, LPCTSTR lpszTopic);
	static BOOL ClientCallback(DWORD dwParam, WPARAM wParam, LPARAM lParam);
	static CChannelView* m_pThis;

	CListClient m_mxClient;
	int m_nLastItem;
	CStatusBar*	 m_pStatusBar;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual ~CChannelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CChannelView)
	afx_msg void OnRefresh();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnClearRefresh();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeSearch();
	afx_msg void OnItemchangedChannels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickChannels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkChannels(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)
