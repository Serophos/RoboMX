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

#if !defined(AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)

#include "stdafx.h"
#include "Metis3.h"
#include "RichEditExCtrl.h"
#include "resource.h"
#include "MainFrm.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;

//#define _NO_EMOTICONS
/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl
UINT UWM_RCLICK = ::RegisterWindowMessage("UWM_RCLICK_F20B5623-6391-4396-8A7E-3C48F6194F10");

CRichEditExCtrl::CRichEditExCtrl()
{

	m_cfDefault.cbSize  = sizeof(CHARFORMAT2);
	m_cfDefault.dwMask = CFM_BACKCOLOR|CFM_COLOR|CFM_SIZE|CFM_FACE|CFM_BOLD|CFM_ITALIC|CFM_LINK;
	m_cfDefault.dwEffects = CFE_BOLD;
	m_cfDefault.crBackColor = RGB(255,255,255);
	m_cfDefault.crTextColor = RGB(0,0,0);
	m_cfDefault.yHeight = 10*20;
	strcpy(m_cfDefault.szFaceName, "Arial");

	m_cfUse = m_cfDefault;
}

CRichEditExCtrl::~CRichEditExCtrl()
{
}


BEGIN_MESSAGE_MAP(CRichEditExCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditExCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl message handlers

void CRichEditExCtrl::Init()
{

	m_cfDefault = g_sSettings.GetDefaultFormat();
	SetBackgroundColor(FALSE, m_cfDefault.crBackColor);
}

void CRichEditExCtrl::SetText(LPCSTR lpszText, COLORREF text, COLORREF bg)
{

	if(!g_sSettings.GetEmoticons()){

		AppendText(lpszText, text, bg);
	}
	else{
		PARAFORMAT pf;

		pf.cbSize = sizeof(PARAFORMAT);
		pf.dwMask = PFM_OFFSETINDENT | PFM_RIGHTINDENT;
		pf.dxRightIndent = 0;
		pf.dxStartIndent = 0;

		SetParaFormat(pf);

		char before[1024], after[1024];
		
		for (;;)
		{
			char next[16] = { NULL }, *rpl = NULL;
			HBITMAP hNext;
			POSITION pos;
			CList<Emoticon *, Emoticon*> &lEmoticons = ((CMainFrame*)GetApp()->m_pMainWnd)->m_lEmoticons;
			pos = lEmoticons.GetHeadPosition();

			while (pos)
			{
				Emoticon *eEmoticon = lEmoticons.GetNext(pos);
				char *txt = stristr(lpszText, eEmoticon->szActivationText);
				if ((txt < rpl && txt) || !rpl && txt)
				{
					rpl = txt;
					hNext = eEmoticon->hBitmap;
					strcpy(next, eEmoticon->szActivationText);
				}
			}

			if (rpl)
			{
				strstp(lpszText, before, next, after);
				AppendText(before, text, bg);
				InsertBitmap(hNext);
				lpszText = after;
			}
			else
			{
				if (strlen(lpszText) > 0) AppendText(lpszText, text, bg);
				break;
			}
		}
	}
}

void CRichEditExCtrl::InsertBitmap(HBITMAP hBitmap)
{

	LineScroll(1);
	int nLen = GetWindowTextLength();
	SetSel(nLen, nLen);

	CImageDataObject::InsertBitmap(GetIRichEditOle(), this, hBitmap);
}

void CRichEditExCtrl::AppendText(LPCSTR lpszText, COLORREF text, COLORREF bg)
{

	int nLines = 0;
	int nLen = 0;

	while(g_sSettings.GetLimitChat() && ((nLines = GetLineCount()) >= g_sSettings.GetMaxLines())){

		nLen = SendMessage(EM_LINEINDEX, 1, NULL);
		SetSel(0, nLen);
		ReplaceSel("");
	}

	nLen = GetWindowTextLength();
	m_cfUse.crTextColor = text;
	m_cfUse.crBackColor = bg;

	SetSel(nLen, nLen);
	SetSelectionCharFormat(m_cfUse);
	ReplaceSel(lpszText);
	int nEnd = 	GetWindowTextLength();
	SetSel(nEnd, nEnd);

	SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

char* CRichEditExCtrl::stristr(const char * str1, const char * str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	if(!*str2)
		return (char *) str1;

	while(*cp){

		s1 = cp;
		s2 = (char *) str2;
		while( *s1 && *s2 && ( !(*s1-*s2) ||
			!(*s1-tolower(*s2)) || !(*s1-toupper(*s2)) ) )
				s1++, s2++;
		if (!*s2)
			return cp;
		cp++;
	}
	return NULL;
}

void CRichEditExCtrl::strstp(const char *in, char *before, const char *txt, char *after)
{

	char *cmp = stristr(in, txt);
	if (!cmp) return;
	strncpy(before, in, cmp - in);
	before[cmp - in] = 0;
	char *out = cmp + strlen(txt); 
	strcpy(after, out);
}


BOOL CRichEditExCtrl::SetSelectionCharFormat(CHARFORMAT2 &cf)
{

	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT2);
	return (BOOL)::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void CRichEditExCtrl::OnLink(NMHDR *pNotifyHeader, LRESULT *pResult)
{
	ENLINK	  *pENLink = (ENLINK *) pNotifyHeader;
	CString	  strURL ;
	CHARRANGE cr ;

	*pResult = 0;

	switch (pNotifyHeader->code)
	{
	case EN_LINK:
		pENLink = (ENLINK *) pNotifyHeader;
	
		switch (pENLink->msg)
		{
		case WM_LBUTTONDOWN:
			GetSel(cr);
			SetSel(pENLink->chrg);
			strURL = GetSelText();
			SetSel(cr);

			{
				CWaitCursor WaitCursor;

				ShellExecute(GetSafeHwnd(), _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL);
				*pResult = 1;
			}
			
			break;

		case WM_LBUTTONUP:
			*pResult = 1;
			break ;
		}
		
		break;
	}
}

BOOL CRichEditExCtrl::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_RBUTTONDOWN){

		::SendMessage(GetParent()->m_hWnd, UWM_RCLICK, m_nID, 0);
	}
	return CRichEditCtrl::PreTranslateMessage(pMsg);
}


