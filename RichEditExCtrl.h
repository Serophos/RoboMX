/*
** Copyright (C) 2004 Thees Winkler
**  
4** This program is free software; you can redistribute it and/or modify
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

#if !defined(AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_)
#define AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichEditExCtrl.h : header file
//
#include "ImageDataObject.h"

class _AFX_RICHEDITEX_STATE
{
public:
    _AFX_RICHEDITEX_STATE();
    virtual ~_AFX_RICHEDITEX_STATE();

    HINSTANCE m_hInstRichEdit20 ;
};

BOOL PASCAL AfxInitRichEditEx();

struct EMOTICON
{
	HBITMAP hBitmap;
	char szFileName[1024];
	char szActivationText[64];
};

///////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl window

class CMetis3View;

class CRichEditExCtrl : public CRichEditCtrl
{
// Construction
public:
	CRichEditExCtrl();

	CHARFORMAT2 m_cfDefault;
	CHARFORMAT2 m_cfUse;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditExCtrl)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, void *lpExtra);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetSelectionCharFormat(CHARFORMAT2& cf);
	void Init();
	void InsertBitmap(HBITMAP hBitmap);
	void SetText(LPCSTR lpszText, COLORREF text, COLORREF bg);
	virtual ~CRichEditExCtrl();

	static char* stristr(const char * str1, const char * str2);
	static void strstp(const char *in, char *before, const char *txt, char *after);
	// Generated message map functions
protected:
	afx_msg void	OnLink(NMHDR *in_pNotifyHeader, LRESULT *out_pResult);
	//{{AFX_MSG(CRichEditExCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	UINT m_nID;
	void AppendText(LPCSTR lpszText, COLORREF text, COLORREF bg);
	CMetis3View* m_pView;
	// datamembers for bitmap storage
};

///////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_)
