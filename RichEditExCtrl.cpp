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
** MERCHANTABILITY or FITNESS forA PARTICULAR PURPOSE.  See the
** GNU General Public License formore details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; ifnot, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#if!defined(AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)

#include "stdafx.h"
#include "Metis3.h"
#include "RichEditExCtrl.h"
#include "RichDocument.h"
#include "RichElement.h"
#include "RichFragment.h"
#include "resource.h"
#include "MainFrm.h"
#include "Settings.h"
#include "SystemInfo.h"
#include "EmoticonManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;
extern CSystemInfo g_sSystem;
extern CEmoticonManager g_eEmoticons;

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl
CRichEditExCtrl::CRichEditExCtrl()
{

	m_bSelectable	= FALSE;
	m_bFollowBottom	= FALSE;
	m_bDefaultLink	= TRUE;

	m_pDocument		= NULL;
	m_nLength		= 0;
	m_pHover		= NULL;
	m_bSelecting	= FALSE;
}

CRichEditExCtrl::~CRichEditExCtrl()
{
	ClearFragments();
}


BEGIN_MESSAGE_MAP(CRichEditExCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl message handlers


void CRichEditExCtrl::Init(UINT nID)
{

	m_nID = nID;
}

void CRichEditExCtrl::SetText(LPCSTR lpszText, COLORREF text, COLORREF bg)
{

}

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl operations

BOOL CRichEditExCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{

	dwStyle |= WS_CHILD|WS_VSCROLL;
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CRichEditExCtrl::SetSelectable(BOOL bSelectable)
{

	m_bSelectable = bSelectable;
}

void CRichEditExCtrl::SetFollowBottom(BOOL bFollowBottom)
{

	m_bFollowBottom = bFollowBottom;
}

void CRichEditExCtrl::SetDefaultLink(BOOL bDefaultLink)
{

	m_bDefaultLink = bDefaultLink;
}

void CRichEditExCtrl::SetDocument(CRichDocument* pDocument)
{

	m_pDocument	= pDocument;
	m_nCookie	= 0xFFFFFFFF;
	m_pHover	= NULL;	
	
	KillTimer(1);
	ClearFragments();
	Invalidate();
}

BOOL CRichEditExCtrl::IsModified() const
{

	return (m_pDocument != NULL && m_pDocument->m_nCookie != m_nCookie);
}

void CRichEditExCtrl::InvalidateIfModified()
{

	if(m_pDocument != NULL && m_pDocument->m_nCookie != m_nCookie) Invalidate();
}

int CRichEditExCtrl::FullHeightMove(int nX, int nY, int nWidth, BOOL bShow)
{

	if(m_pDocument == NULL) return 0;
	
	CRect rc(0, 0, nWidth, -1);
	
	CClientDC dc(this);
	CFont* pOldFont = (CFont*)dc.SelectObject(&GetApp()->m_gdiFont);
	Layout(&dc, &rc);
	dc.SelectObject(pOldFont);
	
	SetWindowPos(NULL, nX, nY, nWidth, m_nLength, SWP_NOZORDER | (bShow ? SWP_SHOWWINDOW : 0));
	
	return m_nLength;
}

BOOL CRichEditExCtrl::GetElementRect(CRichElement* pElement, RECT* prc)
{

	for(int nFragment = 0 ; nFragment < m_pFragments.GetCount(); nFragment++){

		CRichFragment* pFragment = (CRichFragment*)m_pFragments.GetAt(nFragment);
		
		if(pFragment->m_pElement == pElement){

			prc->left	= pFragment->m_pt.x;
			prc->top	= pFragment->m_pt.y;
			prc->right	= prc->left + pFragment->m_sz.cx;
			prc->bottom	= prc->top + pFragment->m_sz.cy;
			
			return TRUE;
		}
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl message handlers

int CRichEditExCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CWnd::OnCreate(lpCreateStruct) == -1) return -1;
	
	m_hcHand = GetApp()->LoadCursor(IDC_HAND);
	m_hcText = GetApp()->LoadStandardCursor(IDC_IBEAM);
	
	SetScrollRange(SB_VERT, 0, 0);
	
	return 0;
}

void CRichEditExCtrl::OnDestroy() 
{

	CWnd::OnDestroy();
}

void CRichEditExCtrl::OnSize(UINT nType, int cx, int cy) 
{

	CWnd::OnSize(nType, cx, cy);
	m_nCookie = 0xFFFFFFFF;
	Invalidate();
}

BOOL CRichEditExCtrl::OnEraseBkgnd(CDC* pDC) 
{

	return TRUE;
}

void CRichEditExCtrl::OnPaint() 
{

	CPaintDC dc(this);
	CRect rc;
	
	GetClientRect(&rc);
	
	if(m_pDocument == NULL){

		dc.FillSolidRect(&rc, g_sSettings.GetRGBBg());
		return;
	}
	
	CSingleLock pLock(&m_pDocument->m_pSection, TRUE);
	
	CFont* pOldFont = (CFont*)dc.SelectObject(&GetApp()->m_gdiFont);
	
	if(m_pDocument->m_nCookie != m_nCookie || m_pBrush.m_hObject == NULL){

		Layout(&dc, &rc);
		
		if(m_pBrush.m_hObject != NULL) m_pBrush.DeleteObject();
		m_pBrush.CreateSolidBrush(m_pDocument->m_crBackground);
	}
	
	dc.SetBkMode(OPAQUE);
	dc.SetBkColor(m_pDocument->m_crBackground);
	dc.SetViewportOrg(0, -GetScrollPos(SB_VERT));
	
	OnPaintBegin(&dc);
	
	CRichElement* pElement = NULL;
	
	for(int nFragment = 0 ; nFragment < m_pFragments.GetSize() ; nFragment++)
	{
		CRichFragment* pFragment = (CRichFragment*)m_pFragments.GetAt(nFragment);
		
		if(pFragment->m_pElement != pElement)
		{
			pElement = pFragment->m_pElement;
			pElement->PrePaint(&dc, m_pHover == pElement);
		}
		
		pFragment->Paint(&dc, this, nFragment);
	}
	
	OnPaintComplete(&dc);
	
	dc.SelectObject(pOldFont);
	
	dc.SetViewportOrg(0, 0);
	
	dc.FillSolidRect(&rc, m_pDocument->m_crBackground);
}

BOOL CRichEditExCtrl::PreTranslateMessage(MSG *pMsg)
{

	if(pMsg->message == WM_MOUSEWHEEL){

		int nMax, nMin, nPos;
		GetScrollRange(SB_VERT, &nMin, &nMax);
		if(((short)HIWORD(pMsg->wParam)) <= 0){	 // down

			nPos = GetScrollPos(SB_VERT);
			if(nPos+20 < nMax){

				nPos+=20;
				SetScrollPos(SB_VERT, nPos, TRUE);
				Invalidate();
			}
		}
		else{ // up

			nPos = GetScrollPos(SB_VERT);
			if(nPos-20 > nMin){

				nPos-=20;
				SetScrollPos(SB_VERT, nPos, TRUE);
				Invalidate();
			}
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}
BOOL CRichEditExCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{

	if(nHitTest == HTCLIENT && m_pDocument != NULL)
	{
		CSingleLock pLock(&m_pDocument->m_pSection, TRUE);
		CPoint pt;
		
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		
		CRichFragment* pFrag = m_bSelecting ? NULL : PointToFrag(pt);

		if(pFrag != NULL && pFrag->m_pElement->m_sLink.GetLength())
		{
			SetCursor(m_hcHand);
			return TRUE;
		}
		else if(m_bSelectable)
		{
			if(m_bSelecting || (pFrag != NULL && pFrag->m_nLength > 0))
			{
				SetCursor(m_hcText);
				return TRUE;
			}
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CRichEditExCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{

	SetFocus();
	
	if(m_bSelectable && m_pDocument != NULL)
	{
		CSingleLock pLock(&m_pDocument->m_pSection, TRUE);
		
		RICHPOSITION pos = PointToPosition(point);
		
		if(pos.nFragment >= 0)
		{
			m_bSelecting	= TRUE;
			m_pSelStart		= pos;
			m_pSelEnd		= pos;
			UpdateSelection();
			SetCapture();
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CRichEditExCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{

	if(m_pDocument != NULL)
	{
		CSingleLock pLock(&m_pDocument->m_pSection, TRUE);
		
		if(m_bSelecting)
		{
			RICHPOSITION pos = PointToPosition(point);
			
			if(pos.nFragment >= 0)
			{
				m_pSelEnd = (pos.nFragment >= 0) ? pos : m_pSelStart;
				UpdateSelection();
			}
		}
		else
		{
			CRichFragment* pFrag = PointToFrag(point);
			CRichElement* pHover = pFrag != NULL ? pFrag->m_pElement : NULL;
			
			if(pHover != m_pHover)
			{
				BOOL bPaint =	(pHover != NULL && pHover->m_nType == retLink) ||
								(m_pHover != NULL && m_pHover->m_nType == retLink);
				
				if(pHover != NULL && m_pHover == NULL)
					SetTimer(1, 200, NULL);
				else if(pHover == NULL && m_pHover != NULL)
					KillTimer(1);
				
				m_pHover = pHover;
				
				if(bPaint) Invalidate();
			}
		}
	}
	
	CRect rc;
	GetClientRect(&rc);

	if(point.y < 0)
	{
		PostMessage(WM_VSCROLL, SB_LINEUP);
	}
	else if(point.y >= rc.bottom)
	{
		PostMessage(WM_VSCROLL, SB_LINEDOWN);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CRichEditExCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
	CWnd::OnLButtonUp(nFlags, point);
	
	if(m_pDocument != NULL)
	{
		CSingleLock pLock(&m_pDocument->m_pSection, TRUE);

		if(m_bSelecting &&
				(m_pSelStart.nFragment != m_pSelEnd.nFragment ||
				  m_pSelStart.nOffset != m_pSelEnd.nOffset))
		{
			CopySelection();
		}
		else if(CRichFragment* pFrag = PointToFrag(point))
		{
			ReleaseCapture();

			if(m_bDefaultLink && pFrag->m_pElement->m_sLink.Find(_T("http://")) == 0)
			{
				ShellExecute(GetSafeHwnd(), _T("open"), pFrag->m_pElement->m_sLink,
					NULL, NULL, SW_SHOWNORMAL);
			}
			
			if(m_pHover == pFrag->m_pElement && m_pHover->m_nType == retLink)
			{
				KillTimer(1);
				m_pHover = NULL;
				Invalidate();
			}
			
			RVN_ELEMENTEVENT pNotify;
			pNotify.hdr.hwndFrom	= GetSafeHwnd();
			pNotify.hdr.idFrom		= GetDlgCtrlID();
			pNotify.hdr.code		= RVN_CLICK;
			pNotify.pElement		= pFrag->m_pElement;
			
			GetOwner()->SendMessage(WM_NOTIFY, pNotify.hdr.idFrom, (LPARAM)&pNotify);
		}
	}
	
	if(m_bSelecting)
	{
		m_bSelecting = FALSE;
		m_pSelStart.nFragment = m_pSelStart.nOffset = 0;
		m_pSelEnd = m_pSelStart;
		
		UpdateSelection();
		ReleaseCapture();
	}
}

void CRichEditExCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{

	CWnd::OnRButtonUp(nFlags, point);
	
	if(m_pDocument != NULL)
	{
		CSingleLock pLock(&m_pDocument->m_pSection, TRUE);

		if(CRichFragment* pFrag = PointToFrag(point))
		{
			
			if(m_pHover == pFrag->m_pElement && m_pHover->m_nType == retLink)
			{
				KillTimer(1);
				m_pHover = NULL;
				Invalidate();
			}
			
			RVN_ELEMENTEVENT pNotify;
			pNotify.hdr.hwndFrom	= GetSafeHwnd();
			pNotify.hdr.idFrom		= GetDlgCtrlID();
			pNotify.hdr.code		= RVN_RCLICK;
			pNotify.pElement		= pFrag->m_pElement;
			
			GetOwner()->SendMessage(WM_NOTIFY, pNotify.hdr.idFrom, (LPARAM)&pNotify);
		}
		else{

			RVN_ELEMENTEVENT pNotify;
			pNotify.hdr.hwndFrom	= GetSafeHwnd();
			pNotify.hdr.idFrom		= GetDlgCtrlID();
			pNotify.hdr.code		= RVN_RCLICK;
			pNotify.pElement		= 0;
			
			GetOwner()->SendMessage(WM_NOTIFY, pNotify.hdr.idFrom, (LPARAM)&pNotify);
		}
	}
}


void CRichEditExCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO pInfo;
	
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_POS|SIF_RANGE|SIF_PAGE;
	
	GetScrollInfo(SB_VERT, &pInfo);
	
	switch (nSBCode)
	{
	case SB_BOTTOM:
		pInfo.nPos = pInfo.nMax;
		break;
	case SB_LINEDOWN:
		pInfo.nPos += 16;
		break;
	case SB_LINEUP:
		pInfo.nPos -= 16;
		break;
	case SB_PAGEDOWN:
		pInfo.nPos += pInfo.nPage;
		break;
	case SB_PAGEUP:
		pInfo.nPos -= pInfo.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pInfo.nPos = nPos;
		break;
	case SB_TOP:
		pInfo.nPos = 0;
		break;
	}
	
	pInfo.fMask	= SIF_POS;
	pInfo.nPos	= max(0, min(pInfo.nPos, pInfo.nMax - (int)pInfo.nPage));
	SetScrollInfo(SB_VERT, &pInfo);
	
	OnVScrolled();
	Invalidate();
}

void CRichEditExCtrl::OnTimer(UINT nIDEvent)
{
	CPoint point;
	CRect rect;
	
	GetCursorPos(&point);
	GetWindowRect(&rect);
	
	if(rect.PtInRect(point)) return;
	
	KillTimer(1);
	
	if(m_pHover != NULL)
	{
		m_pHover = NULL;
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl layout engine

void CRichEditExCtrl::ClearFragments()
{
	for(int nFragment = 0 ; nFragment < m_pFragments.GetSize() ; nFragment++)
	{
		delete (CRichFragment*)m_pFragments.GetAt(nFragment);
	}
	
	m_pFragments.RemoveAll();
	m_nLength = 0;
	
	if(m_bSelecting)
	{
		m_bSelecting = FALSE;
		ReleaseCapture();
	}
	
	m_pSelStart.nFragment		= m_pSelStart.nOffset		= 0;
	m_pSelEnd.nFragment			= m_pSelEnd.nOffset			= 0;
	m_pSelAbsStart.nFragment	= m_pSelAbsStart.nOffset	= 0;
	m_pSelAbsEnd.nFragment		= m_pSelAbsEnd.nOffset		= 0;
}

void CRichEditExCtrl::Layout(CDC* pDC, CRect* pRect)
{
	ASSERT(m_pDocument != NULL);
	
	CSingleLock pLock(&m_pDocument->m_pSection, TRUE);
	
	ClearFragments();
	
	pRect->DeflateRect(m_pDocument->m_szMargin.cx, m_pDocument->m_szMargin.cy);
	
	CPoint pt(pRect->left, pRect->top);
	
	int nLeftPoint		= pRect->left;
	int nWidth			= pRect->Width();
	int nLineHeight		= 0;
	int nAlign			= reaLeft;
	
	CRichFragment* pFrag = NULL;
	CPtrList pLine;
	
	for(POSITION pos = m_pDocument->GetIterator() ; pos ;)
	{
		CRichElement* pElement = m_pDocument->GetNext(pos);
		
		if(pElement->m_nFlags & retfHidden) continue;
		
		pElement->PrePaint(pDC, FALSE);
		
		if(pElement->m_nType == retNewline)
		{
			WrapLineHelper(pLine, pt, nLineHeight, nWidth, nAlign);
			
			int nGap = 0, nIndent = 0;
			
			if(_stscanf(pElement->m_sText, _T("%lu.%lu"), &nGap, &nIndent) == 2)
			{
				nLeftPoint	= pRect->left + nIndent;
				nWidth		= pRect->right - nIndent;
			}
			
			pt.x = nLeftPoint;
			pt.y += nGap;
			
			continue;
		}
		else if(pElement->m_nType == retAlign)
		{
			WrapLineHelper(pLine, pt, nLineHeight, nWidth, nAlign);
			
			if(pElement->m_sText.CompareNoCase(_T("center")) == 0)
			{
				nAlign = reaCenter;
			}
			else if(pElement->m_sText.CompareNoCase(_T("right")) == 0)
			{
				nAlign = reaRight;
			}
			else
			{
				nAlign = reaLeft;
			}
			
			continue;
		}
		else if(pElement->m_nType == retGap || pElement->m_nType < retText)
		{
			pFrag = new CRichFragment(pElement, &pt);
			m_pFragments.Add(pFrag);
			
			if(pt.x + pFrag->m_sz.cx > nWidth)
			{
				WrapLineHelper(pLine, pt, nLineHeight, nWidth, nAlign);
				pFrag->m_pt = pt;
			}
			
			pt.x += pFrag->m_sz.cx;
			
			if(pElement->m_nType != retGap)
			{
				nLineHeight = max(nLineHeight, pFrag->m_sz.cy);
				pLine.AddTail(pFrag);
			}
			
			continue;
		}
		
		LPTSTR  pszText	= (LPTSTR)(LPCTSTR)pElement->m_sText;
		LPCTSTR pszWord	= pszText;
		LPCTSTR pszLast	= NULL;
		
		int nWordStart	= 0;
		int nSpace		= pDC->GetTextExtent(_T(" ")).cx;
		
		pFrag = NULL;
		
		for(int nChar = 0 ; nChar <= pElement->m_sText.GetLength() ; nChar++, pszText++)
		{
			if(*pszText != ' ' && *pszText != '\t' && *pszText != 0) continue;
			
			if(nChar > nWordStart || *pszText == 0)
			{
				TCHAR cSave = *pszText;
				*pszText = 0;
				
				CSize szWord = pDC->GetTextExtent(pszWord);
				
				if(pFrag != NULL)
				{
					CSize szLast = pDC->GetTextExtent(pszLast);
					
					if(pFrag->m_pt.x + szLast.cx <= nWidth)
					{
						pt.x -= pFrag->m_sz.cx;
						pFrag->Add(nChar - pFrag->m_nOffset, &szLast);
						pt.x += pFrag->m_sz.cx;
						nWordStart = -1;
					}
				}
				
				if(nChar == nWordStart && *pszText == 0) break;
				
				if(nWordStart >= 0)
				{
					if(pt.x + szWord.cx + (pFrag ? nSpace : 0) > nWidth)
					{
						WrapLineHelper(pLine, pt, nLineHeight, nWidth, nAlign);
					}
					else if(pFrag != NULL) pt.x += nSpace;
					
					pFrag = new CRichFragment(pElement, nWordStart, nChar - nWordStart, &pt, &szWord);
					pszLast = pszWord;
					
					pt.x += pFrag->m_sz.cx;
					nLineHeight = max(nLineHeight, pFrag->m_sz.cy);
					
					m_pFragments.Add(pFrag);
					pLine.AddTail(pFrag);
				}
				
				*pszText = cSave;
			}
			
			if(pFrag != NULL)
			{
				nWordStart	= nChar + 1;
				pszWord		= pszText + 1;
			}
		}
	}
	
	WrapLineHelper(pLine, pt, nLineHeight, nWidth, nAlign);
	
	pRect->InflateRect(m_pDocument->m_szMargin.cx, m_pDocument->m_szMargin.cy);
	
	m_nLength = pt.y - pRect->top + 2*m_pDocument->m_szMargin.cy;
	
	SCROLLINFO pInfo;
	
	pInfo.cbSize	= sizeof(pInfo);
	pInfo.fMask		= SIF_POS | SIF_RANGE | SIF_PAGE;
	
	GetScrollInfo(SB_VERT, &pInfo, pInfo.fMask);
	
	if(m_bFollowBottom && pInfo.nPos >= pInfo.nMax - (int)pInfo.nPage - 1)
		pInfo.nPos = m_nLength;
	
	pInfo.fMask		= SIF_ALL & ~SIF_TRACKPOS;
	pInfo.nMin		= 0;
	pInfo.nMax		= m_nLength - 1;
	pInfo.nPage		= (pRect->bottom == -1) ? m_nLength : pRect->Height();
	pInfo.nPos		= min(pInfo.nPos, max(0, pInfo.nMax - (int)pInfo.nPage));
	
	SetScrollInfo(SB_VERT, &pInfo, TRUE);
	
	m_nCookie = m_pDocument->m_nCookie;
	OnLayoutComplete();
}

void CRichEditExCtrl::WrapLineHelper(CPtrList& pLine, CPoint& pt, int& nLineHeight, int nWidth, int nAlign)
{
	if(pLine.GetCount() == 0) return;
	
	int nLeft = ((CRichFragment*)pLine.GetHead())->m_pt.x;
	int nHorz = 0;

	if(nAlign == reaCenter)
	{
		nHorz = nWidth / 2 - (pt.x - nLeft) / 2;
	}
	else if(nAlign == reaRight)
	{
		nHorz = nWidth - pt.x;
	}

	for(POSITION posAlign = pLine.GetHeadPosition() ; posAlign ;)
	{
		CRichFragment* pAlign = (CRichFragment*)pLine.GetNext(posAlign);

		if(pAlign->m_pElement->m_nFlags & retfMiddle)
		{
			pAlign->m_pt.y += (nLineHeight / 2 - pAlign->m_sz.cy / 2);
		}
		else
		{
			pAlign->m_pt.y += (nLineHeight - pAlign->m_sz.cy);
		}

		pAlign->m_pt.x += nHorz;
	}

	pt.x = nLeft;
	pt.y += nLineHeight;

	pLine.RemoveAll();
	nLineHeight = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl coordinate mapping

CRichFragment* CRichEditExCtrl::PointToFrag(CPoint& pt)
{
	if(m_pDocument == NULL || m_pDocument->m_nCookie != m_nCookie) return NULL;
	
	pt.y += GetScrollPos(SB_VERT);
	
	for(int nFragment = m_pFragments.GetSize() - 1 ; nFragment >= 0 ; nFragment--)
	{
		CRichFragment* pFragment = (CRichFragment*)m_pFragments.GetAt(nFragment);
		
		if(pt.x >= pFragment->m_pt.x && pt.y >= pFragment->m_pt.y &&
			 pt.x < pFragment->m_pt.x + pFragment->m_sz.cx &&
			 pt.y < pFragment->m_pt.y + pFragment->m_sz.cy)
		{
			pt.y -= GetScrollPos(SB_VERT);
			return pFragment;
		}
	}
	
	pt.y -= GetScrollPos(SB_VERT);
	
	return NULL;
}

RICHPOSITION CRichEditExCtrl::PointToPosition(CPoint& pt)
{
	RICHPOSITION pos = { -1, 0 };
	
	if(m_pDocument == NULL || m_pDocument->m_nCookie != m_nCookie) return pos;

	pt.y += GetScrollPos(SB_VERT);
	
	for(int nFragment = 0 ; nFragment < m_pFragments.GetSize() ; nFragment++)
	{
		CRichFragment* pFragment = (CRichFragment*)m_pFragments.GetAt(nFragment);
		
		if(pt.x >= pFragment->m_pt.x && pt.y >= pFragment->m_pt.y &&
			 pt.x < pFragment->m_pt.x + pFragment->m_sz.cx &&
			 pt.y < pFragment->m_pt.y + pFragment->m_sz.cy)
		{
			pos.nFragment = nFragment;
			
			if(pFragment->m_nLength > 0)
			{
				CClientDC dc(this);
				CFont* pOld = dc.GetCurrentFont();
				pFragment->m_pElement->PrePaint(&dc, FALSE);
				
				LPCTSTR pszText = pFragment->m_pElement->m_sText;
				pszText += pFragment->m_nOffset;
				int nX = pt.x - pFragment->m_pt.x;
				
				for(pos.nOffset = 0 ; pos.nOffset < pFragment->m_nLength ; pszText ++)
				{
					int nWidth = dc.GetTextExtent(pszText, 1).cx;
					if(nX < (nWidth >> 1)) break;
					pos.nOffset++;
					if(nX < nWidth) break;
					nX -= nWidth;
				}
				
				dc.SelectObject(pOld);
			}
			
			break;
		}		
	}	
	
	pt.y -= GetScrollPos(SB_VERT);
	
	return pos;
}

CPoint CRichEditExCtrl::PositionToPoint(RICHPOSITION& pos)
{
	CPoint pt(0, - GetScrollPos(SB_VERT));
	
	if(m_pDocument == NULL || m_pDocument->m_nCookie != m_nCookie) return pt;
	if(pos.nFragment < 0 || m_pFragments.GetSize() == 0) return pt;
	
	BOOL bOverload = pos.nFragment >= m_pFragments.GetSize();
	
	CRichFragment* pFragment = (CRichFragment*)m_pFragments.GetAt(
								bOverload ? m_pFragments.GetSize() - 1 : pos.nFragment);
	
	pt.x = pFragment->m_pt.x;
	pt.y += pFragment->m_pt.y;
	
	if(bOverload)
	{
		pt.x += pFragment->m_sz.cx;
	}
	else if(pos.nOffset > 0)
	{
		if(pos.nOffset < pFragment->m_nLength)
		{
			CClientDC dc(this);
			CFont* pOld = dc.GetCurrentFont();
			pFragment->m_pElement->PrePaint(&dc, FALSE);
			LPCTSTR pszText = pFragment->m_pElement->m_sText;
			pszText += pFragment->m_nOffset;
			pt.x += dc.GetTextExtent(pszText, pos.nOffset).cx;
			dc.SelectObject(pOld);
		}
		else
		{
			pt.x += pFragment->m_sz.cx;
		}
	}
	
	return pt;
}

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl selection helpers

void CRichEditExCtrl::UpdateSelection()
{
	if(m_pSelStart.nFragment < m_pSelEnd.nFragment || (m_pSelStart.nFragment == m_pSelEnd.nFragment && m_pSelStart.nOffset <= m_pSelEnd.nOffset))
	{
		if(	m_pSelAbsStart.nFragment	!= m_pSelStart.nFragment ||
				m_pSelAbsStart.nOffset		!= m_pSelStart.nOffset ||
				m_pSelAbsEnd.nFragment		!= m_pSelEnd.nFragment ||
				m_pSelAbsEnd.nOffset		!= m_pSelEnd.nOffset)
		{
			m_pSelAbsStart	= m_pSelStart;
			m_pSelAbsEnd	= m_pSelEnd;
			Invalidate();
		}
	}
	else
	{
		if(	m_pSelAbsStart.nFragment	!= m_pSelEnd.nFragment ||
				m_pSelAbsStart.nOffset		!= m_pSelEnd.nOffset ||
				m_pSelAbsEnd.nFragment		!= m_pSelStart.nFragment ||
				m_pSelAbsEnd.nOffset		!= m_pSelStart.nOffset)
		{
			m_pSelAbsStart	= m_pSelEnd;
			m_pSelAbsEnd	= m_pSelStart;
			Invalidate();
		}
	}
}

void CRichEditExCtrl::CopySelection()
{
	CString str;
	
	for(int nFragment = m_pSelAbsStart.nFragment ; nFragment <= m_pSelAbsEnd.nFragment ; nFragment++)
	{
		CRichFragment* pFragment = (CRichFragment*)m_pFragments.GetAt(nFragment);
		
		if(pFragment->m_nLength == 0)
		{
			if(nFragment == m_pSelAbsEnd.nFragment) break;
			
			if(pFragment->m_pElement->m_nType < retText)
			{
				str += ' ';
			}
		}
		else
		{
			int nCharStart = 0, nCharEnd = pFragment->m_nLength;
			
			if(m_pSelAbsStart.nFragment == nFragment)
			{
				nCharStart = m_pSelAbsStart.nOffset;
			}
			
			if(m_pSelAbsEnd.nFragment == nFragment)
			{
				nCharEnd = m_pSelAbsEnd.nOffset;
			}
			
			if(nCharEnd > nCharStart)
			{
				str +=  pFragment->m_pElement->m_sText.Mid(
						pFragment->m_nOffset + nCharStart,
						nCharEnd - nCharStart);
			}
		}
		
		if(nFragment < m_pSelAbsEnd.nFragment)
		{
			CRichFragment* pNextFrag = (CRichFragment*)m_pFragments.GetAt(nFragment + 1);
			
			if(pFragment->m_pElement != pNextFrag->m_pElement)
			{
				POSITION pos = m_pDocument->Find(pFragment->m_pElement);
				
				while (pos)
				{
					CRichElement* pCopy = m_pDocument->GetNext(pos);
					if(pCopy == pNextFrag->m_pElement) break;
					if(pCopy->m_nType == retNewline) str += _T("\r\n");
				}
			}
		}
	}
	
	if(str.GetLength() && AfxGetMainWnd()->OpenClipboard())
	{
		USES_CONVERSION;
		EmptyClipboard();
		
		if(g_sSystem.IsWindowsNT())
		{
			LPCWSTR pszWide = T2CW((LPCTSTR)str);
			HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, (wcslen(pszWide) + 1) * sizeof(WCHAR));
			LPVOID pMem = GlobalLock(hMem);
			CopyMemory(pMem, pszWide, (wcslen(pszWide) + 1) * sizeof(WCHAR));
			GlobalUnlock(hMem);
			SetClipboardData(CF_UNICODETEXT, hMem);
		}
		else
		{
			LPCSTR pszASCII = T2CA((LPCTSTR)str);
			HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, strlen(pszASCII) + 1);
			LPVOID pMem = GlobalLock(hMem);
			CopyMemory(pMem, pszASCII, strlen(pszASCII) + 1);
			GlobalUnlock(hMem);
			SetClipboardData(CF_TEXT, hMem);
		}
		
		CloseClipboard();
	}
}

void CRichEditExCtrl::UpdateEmoticons(void)
{

	if(m_pDocument == NULL) return;

	for(POSITION pos = m_pDocument->GetIterator(); pos; ){

		CRichElement* pElement = m_pDocument->GetNext(pos);
		if(pElement->m_nType == retBitmap){

			int nPos = g_eEmoticons.Lookup(pElement->m_sText);
			if(nPos >= 0){

				pElement->m_hImage = (DWORD)g_eEmoticons.m_aEmoticons[nPos]->hBitmap;
			}
			else{

				pElement->m_hImage = NULL;
				pElement->m_nType = retText;
			}
		}
	}
}
