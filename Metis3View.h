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
	CRichEditExCtrl m_rSys;
	CRichEditExCtrl m_rChat;
	CString	m_strInput;
	//}}AFX_DATA
	CSplitterControl m_sSplitter1;
	CSplitterControl m_sSplitter2;
// Attributes
public:
	CMetis3Doc* GetDocument();

// Operations
public:
	afx_msg LRESULT OnSystem(WPARAM wParam, LPARAM lParam);
	void DeleteEmoticons();
	void AddEmoticon(char *szFileName, char *szActivationText);
	void LoadEmoticons();
	void DoResize2(int delta);
	void DoResize1(int delta);
	void RemoveUser(CString strUser);
	void AddUser(CString strUsername, WORD wLine, DWORD dwFiles, CString strNodeIP, WORD wNodePort);
	void WriteSystemMsg(CString strMsg, COLORREF crText = RGB(0, 150, 0));

	CChatClient m_mxClient;
	CArray<MX_USERINFO, MX_USERINFO> m_aUsers;
	CList<struct EMOTICON *, struct EMOTICON *> m_lEmoticons;
	CFont m_fFont;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetis3View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CMetis3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg LRESULT OnInput(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAction(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTopic(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMotd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUnknown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJoin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT CMetis3View::OnRenameMsg(WPARAM wParam, LPARAM lParam);

	//{{AFX_MSG(CMetis3View)
	afx_msg void OnRename();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLeave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Metis3View.cpp
inline CMetis3Doc* CMetis3View::GetDocument()
   { return (CMetis3Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METIS3VIEW_H__7D9E7FA6_DE83_4E32_B378_C478F57D43A7__INCLUDED_)
