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
//#include "ImageDataObject.h"

#define NEWLINE_FORMAT	_T("2")


///////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl window
class CRichDocument;
class CRichElement;
class CRichFragment;

typedef struct
{
	int		nFragment;
	int		nOffset;
} RICHPOSITION;

class CRichEditExCtrl : public CWnd
{
// Construction
public:
	CRichEditExCtrl();
	virtual ~CRichEditExCtrl();

	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void Init(UINT m_nID);
	void SetText(LPCSTR lpszText, COLORREF text, COLORREF bg);

// Operations
public:
	void			SetSyncObject(CSyncObject* pSyncRoot);
	void			SetSelectable(BOOL bSelectable);
	void			SetFollowBottom(BOOL bFollowBottom);
	void			SetDefaultLink(BOOL bDefaultLink);
	void			SetDocument(CRichDocument* pDocument);
	BOOL			IsModified() const;
	void			InvalidateIfModified();
	int				FullHeightMove(int nX, int nY, int nWidth, BOOL bShow = FALSE);
	BOOL			GetElementRect(CRichElement* pElement, RECT* prc);
	CRichFragment*	PointToFrag(CPoint& pt);

protected:
	void			ClearFragments();
	void			Layout(CDC* pDC, CRect* pRect);
	void			WrapLineHelper(CPtrList& pLine, CPoint& pt, int& nLineHeight, int nWidth, int nAlign);
	RICHPOSITION	PointToPosition(CPoint& pt);
	CPoint			PositionToPoint(RICHPOSITION& pt);
	void			UpdateSelection();
	void			CopySelection();

protected:
	virtual void	OnLayoutComplete() {};
	virtual void	OnPaintBegin(CDC* pDC) {};
	virtual void	OnPaintComplete(CDC* pDC) {};
	virtual void	OnVScrolled() {};
	virtual BOOL	PreTranslateMessage(MSG *pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()

	friend class CRichFragment;

protected:
	UINT m_nID;
	CSyncObject*	m_pSyncRoot;
	BOOL			m_bSelectable;
	BOOL			m_bFollowBottom;
	BOOL			m_bDefaultLink;

	CRichDocument*	m_pDocument;
	DWORD			m_nCookie;
	CPtrArray		m_pFragments;
	int				m_nLength;

	CRichElement*	m_pHover;
	BOOL			m_bSelecting;
	RICHPOSITION	m_pSelStart;
	RICHPOSITION	m_pSelEnd;
	RICHPOSITION	m_pSelAbsStart;
	RICHPOSITION	m_pSelAbsEnd;

	HCURSOR			m_hcHand;
	HCURSOR			m_hcText;
	CBrush			m_pBrush;

public:
	void UpdateEmoticons(void);
};

typedef struct
{
	NMHDR			hdr;
	CRichElement*	pElement;
} RVN_ELEMENTEVENT;

#define RVN_CLICK		100
#define RVN_RCLICK		101

#define ON_RICH_NOTIFY(wNotifyCode, id, memberFxn) \
	{ WM_NOTIFY, (WORD)(int)wNotifyCode, (WORD)id, (WORD)id, AfxSigNotify_v, \
		(AFX_PMSG) \
		(reinterpret_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) > \
		(memberFxn)) },

///////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_)
