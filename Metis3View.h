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


#if !defined(AFX_METIS3VIEW_H__7D9E7FA6_DE83_4E32_B378_C478F57D43A7__INCLUDED_)
#define AFX_METIS3VIEW_H__7D9E7FA6_DE83_4E32_B378_C478F57D43A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Winsock2.h>
#pragma comment(lib, "WS2_32")

#include "ChatClient.h"
#include "RichEditExCtrl.h"
#include "MyEdit.h"
#include "SplitterControl.h"
#include "MyListCtrl.h"
#include "ColorStatusBar.h"

#define LCSB_CLIENTDATA 1
#define LCSB_NCOVERRIDE 2

class CMetis3View : public CFormView
{
protected: // create from serialization only
	CMetis3View();
	DECLARE_DYNCREATE(CMetis3View)

public:
	//{{AFX_DATA(CMetis3View)
	enum { IDD = IDD_METIS3_FORM };
	CButton	m_btExit;
	CMyEdit	m_eInput;
	CMyListCtrl	m_lcUsers;
	CString	m_strInput;
	//}}AFX_DATA
	CSplitterControl m_sSplitter1;
	CSplitterControl m_sSplitter2;
// Attributes
public:
	CMetis3Doc* GetDocument();

// Operations
public:
	LRESULT OnReloadCfg(WPARAM w, LPARAM l);
	static CString GetMyLocalTime();
	static void ReplaceVars(CString& strMsg);
	void InputWelcome();
	static CString GetMySystemInfo();
	void Input(CString strText);
	void UpdateAverageLag(BOOL bStart = TRUE);
	void LoadRCMSMenu();
	void ReCalcLayout();
	afx_msg LRESULT OnSystem(WPARAM wParam, LPARAM lParam);
	void DoResize2(int delta);
	void DoResize1(int delta);
	void RemoveUser(const CString strUser, const CString strIP, WORD wPort);
	void AddUser(CString strUsername, WORD wLine, DWORD dwFiles, CString strNodeIP, WORD wNodePort, CString strIP, WORD wUserLevel);
	void WriteSystemMsg(CString strMsg, COLORREF crText = RGB(0, 150, 0));

	static CString GetWinampSong();
	static BOOL    IsVideoPlaying();
	static CString GetUpTime();

	CChatClient m_mxClient;
	CArray<MX_USERINFO, MX_USERINFO> m_aUsers;
	CFont m_fFont;
	int m_nServerType;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetis3View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CMetis3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CColorStatusBar*	 m_pStatusBar;

	CStringArray m_aRCMSMenu;
	CString		 m_strWd;
	DWORD		 m_dwAvLag;
	DWORD		 m_dwLastTic;

	CImageList	m_iImageList;
	CString		 m_strTarget;

// Generated message map functions
protected:
	BOOL m_bHasJoined;

	afx_msg void OnAwayControl(UINT nID);
	afx_msg void OnAwayControlBack(UINT nID);

	afx_msg LRESULT OnEchoText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEchoSysText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInput(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAction(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTopic(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMotd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUnknown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJoin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRedirect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRenameMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRclickChat(WPARAM w, LPARAM l);

	afx_msg void OnUpdateUserlistMenu(CCmdUI* pCmdUI);
	afx_msg void OnRename();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLeave();
	afx_msg void OnRclickUserlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUserlistSendmessage();
	afx_msg void OnUserlistRemoveadmin();
	afx_msg void OnUserlistRedirect();
	afx_msg void OnUserlistPrintuserstat();
	afx_msg void OnUserlistPrintip();
	afx_msg void OnUserlistUnban();
	afx_msg void OnUserlistKickban();
	afx_msg void OnUserlistKick();
	afx_msg void OnUserlistCopyusername();
	afx_msg void OnUserlistBan();
	afx_msg void OnUserlistAddadmin();
	afx_msg void OnUserlistReadusermessage();
	afx_msg void OnUserlistCustomizethismenu();
	afx_msg void OnUserlistIgnore();
	afx_msg void OnUserlistUnignore();
	afx_msg void OnReconnect();
	afx_msg void OnUpdateReconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWinampsongMenu(CCmdUI* pCmdUI);
	afx_msg void OnChatroomSelectall();
	afx_msg void OnChatroomSearchongoogle();
	afx_msg void OnChatroomQuote();
	afx_msg void OnChatroomCopy();
	afx_msg void OnChatroomClearscreen();
	afx_msg void OnChatroomCopyroomname();
	afx_msg void OnDisplayWinampsong();
	afx_msg void OnDisplaySystemuptime();
	afx_msg void OnDisplaySysteminfo();
	afx_msg void OnDisplayAveragelag();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChatroomTexttricksBubbles();
	afx_msg void OnChatroomTexttricksBox();
	afx_msg void OnChatroomTexttricksHacker();
	afx_msg void OnChatroomAsciiartdesign();
	afx_msg void OnChatTexttricks3dbuttonsnormal();
	afx_msg void OnChatTexttricks3dbuttonsaction();
	afx_msg LRESULT OnRoomRename(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetServerType(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRenNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CRichEditExCtrl m_rSys;
	CRichEditExCtrl m_rChat;
	afx_msg void OnEnLinkChat(NMHDR *pNMHDR, LRESULT *pResult);


	void HandleHiLite(void);
	afx_msg LRESULT OnRenameCl(WPARAM wParam, LPARAM lParam);
protected:
	CString GetUserInput(CString strReason = "");
};

#ifndef _DEBUG  // debug version in Metis3View.cpp
inline CMetis3Doc* CMetis3View::GetDocument()
   { return (CMetis3Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METIS3VIEW_H__7D9E7FA6_DE83_4E32_B378_C478F57D43A7__INCLUDED_)
