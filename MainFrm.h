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


#if !defined(AFX_MAINFRM_H__4EE0D859_D847_4259_AA7D_531CB4E1928B__INCLUDED_)
#define AFX_MAINFRM_H__4EE0D859_D847_4259_AA7D_531CB4E1928B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "docselect.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

	void LoadToTray(HWND hWnd, UINT uCallbackMessage, CString strInfoTitle, CString strInfo, CString strTip, int uTimeout, HICON icon, DWORD dwIcon);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void LoadRCMS();
	void JoinChannel();
	CString m_strRoom;
	BOOL  m_bChannelList;
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CDocSelector	m_wndDocSelector;

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	
	// used for full-screen mode
	BOOL	  m_bFullScreenMode;
	BOOL	  m_bStatusBarWasVisible;
	BOOL	  m_bToolBarWasVisible;
	CRect	  m_mainRect;
	CToolBar* m_pwndFullScreenBar;
	bool	  m_bChildMax;

	void FullScreenModeOn();
	void FullScreenModeOff();


// Generated message map functions
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nIconData;

	//{{AFX_MSG(CMainFrame)
	afx_msg void OnClose();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAppExit();
	afx_msg void OnViewOptions();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnChannelChannellist();
	afx_msg void OnUpdateChannelChannellist(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnStartNodeserver();
	afx_msg void OnUpdateStartNodeserver(CCmdUI* pCmdUI);
	afx_msg void OnViewFullScreen();
	afx_msg void OnUpdateViewFullScreen(CCmdUI* pCmdUI);
	afx_msg void OnIdsScnextwindow();
	afx_msg void OnViewChannelbar();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__4EE0D859_D847_4259_AA7D_531CB4E1928B__INCLUDED_)
