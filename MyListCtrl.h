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

#if !defined(AFX_MYLISTCTRL_H__76FD4D17_6C21_4BD7_B104_6A4AE50E9CDD__INCLUDED_)
#define AFX_MYLISTCTRL_H__76FD4D17_6C21_4BD7_B104_6A4AE50E9CDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//

#include "MyHeaderCtrl.h"

#ifdef _DEBUG
	#define ASSERT_VALID_STRING(str) ASSERT(!IsBadStringPtr(str, 0xfffff))
#else
	#define ASSERT_VALID_STRING(str)((void)0)
#endif	//	_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:

// Operations
public:
	BOOL SetHeadings(UINT uiStringID);
	BOOL SetHeadings(const CString& strHeadings);

	int AddItem(LPCTSTR pszText, ...);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
	void LoadColumnInfo();
	void SaveColumnInfo();
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	void Sort(int nColumn, BOOL bAscending );
	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBkColor(COLORREF cr);
	void SetColors(COLORREF cr1, COLORREF cr2, COLORREF cr3, COLORREF cr4, COLORREF cr5);
	virtual ~CMyListCtrl();

	// Generated message map functions
protected:
	virtual void PreSubclassWindow();
	static int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);
	void FreeItemMemory(const int nItem);
	BOOL CMyListCtrl::SetTextArray(int nItem, LPTSTR* arrpsz);
	LPTSTR* CMyListCtrl::GetTextArray(int nItem) const;

	CMyHeaderCtrl m_ctlHeader;

	int  m_nNumColumns;
	int  m_nSortColumn;
	BOOL m_bSortAscending;
	bool m_bInited;

	COLORREF m_cr1;
	COLORREF m_cr2;
	COLORREF m_cr3;
	COLORREF m_cr4;
	COLORREF m_cr5;
	COLORREF m_crBg;
	COLORREF m_crHiLite;
	BOOL     m_bHiLite;
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnCustomdrawMyList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SetHiLite(BOOL bHiLite = TRUE);
	void Sort(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__76FD4D17_6C21_4BD7_B104_6A4AE50E9CDD__INCLUDED_)
