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


#include "stdafx.h"
#include "Metis3.h"
#include "MyEdit.h"
#include "Metis3Doc.h"
#include "Metis3View.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSettings g_sSettings;
/////////////////////////////////////////////////////////////////////////////
// CMyEdit

UINT UWM_INPUT = ::RegisterWindowMessage("UWM_INPUT-7A14F66B-ABAB-4525-AC01-841C82EC48B6");

extern CStringArray g_aRCMSCommands;
CStringArray g_aQuick;

#define g_aRCMSCommandsNUM g_aRCMSCommands.GetSize() 
#define g_aQuickNUM g_aQuick.GetSize() 

CMyEdit::CMyEdit()
{

	m_crBg = ::GetSysColor(COLOR_WINDOW); // Initializing the Background Color to the system face color.
	m_crBgFocus = g_sSettings.GetRGBFocus();
	m_brBkgnd.CreateSolidBrush(m_crBg); // Create the Brush Color for the Background.
	m_crDraw = m_crBg;
	m_bEx = FALSE;
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

/////////////////////////////////
// Search string in autocommand
/////////////////////////////////
int CMyEdit::SearchItem(CString strString)
{

	CString strTmp;

	for(int nIndex = 0; nIndex < g_aQuick.GetSize(); nIndex++){

		strTmp = g_aQuick[nIndex];
		if(strTmp == strString) break;
	}

	if((nIndex >= g_aQuick.GetSize())  || g_aQuick.GetSize() == 0) return 0;

	return nIndex + 1;
}

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg) 
{

	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)
			&& (GetKeyState(VK_SHIFT) >= 0)){

		::SendMessage(GetParent()->m_hWnd, UWM_INPUT, 0, 0);
	}
	else if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)
				&& (GetKeyState(VK_SHIFT) < 0)){

		CString strText;
		GetWindowText(strText);
		strText+="\r\n";
		SetWindowText(strText);
		SetSel(0, -1, FALSE);
		SetSel(-1, 0, FALSE);
		return TRUE;
	}
	else if((pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_KEYDOWN) && !m_bEx){


		///////////////////////////////////////////////////////
		// Scroll through RCMS commands UP (PAGE UP)
		///////////////////////////////////////////////////////
		if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_PRIOR) || 
			(pMsg->message == WM_MOUSEWHEEL && ((short)HIWORD(pMsg->wParam)) > 0)){
		
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){
		
				DWORD dwIndex = atol(szTempStr);
				if(dwIndex == 0){

					lstrcpy(szTempStr, g_aRCMSCommands[g_aRCMSCommandsNUM - 1]);
					SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
					return TRUE;
				}
				else if((dwIndex == 1) || (dwIndex >= g_aRCMSCommandsNUM)){

					SetWindowText("");
				}
				else{

					lstrcpy(szTempStr, g_aRCMSCommands[dwIndex - 2]);
					SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
				}
				return TRUE;
			}
			else{

				lstrcpy(szTempStr, g_aRCMSCommands[g_aRCMSCommandsNUM - 1]);
				SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				return TRUE;
			}
		}
		///////////////////////////////////////////////////////
		// RCMS Down (PAGE DOWN)
		///////////////////////////////////////////////////////
		else if(((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_NEXT) || 
			(pMsg->message == WM_MOUSEWHEEL && ((short)HIWORD(pMsg->wParam)) <= 0)) && !m_bEx){
		
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){

				DWORD dwIndex = atol(szTempStr);
				if(dwIndex >= g_aRCMSCommandsNUM){

					SendMessage(WM_SETTEXT, 0, (LPARAM)"");
					//Beep(1000, 50);
				}
				else{

					lstrcpy(szTempStr, g_aRCMSCommands[dwIndex]);
					SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
				}
				return TRUE;
			}
			else{

				lstrcpy(szTempStr, g_aRCMSCommands[0]);
				SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				return TRUE;
			}
		}
		///////////////////////////////////////////////////////
		// Commands UP UPARROW or MOUSEHWEL UP
		///////////////////////////////////////////////////////
		else if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP) && (g_aQuickNUM != 0) && !m_bEx){
		
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){
		
				DWORD dwIndex = SearchItem(szTempStr);
				if(dwIndex == 0){ // Item was not found

					CString strOut = g_aQuick[g_aQuick.GetSize() - 1];
					CMetis3View::ReplaceVars(strOut);
					lstrcpy(szTempStr, strOut);
					SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
					return TRUE;
				}
				else if(dwIndex == 1){ // Item found but we are at no1

					SetWindowText("");
					//Beep(1000, 50);
				}
				else{ // item found 

					CString strOut = g_aQuick[dwIndex - 2];
					CMetis3View::ReplaceVars(strOut);
					lstrcpy(szTempStr, strOut);
					SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
				}
				return TRUE;
			}
			else{

				CString strOut = g_aQuick[g_aQuick.GetSize() - 1];
				CMetis3View::ReplaceVars(strOut);
				lstrcpy(szTempStr, strOut);
				SendMessage(WM_SETTEXT, 0, (LPARAM)szTempStr);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				return TRUE;
			}
		}
		///////////////////////////////////////////////////////
		// Commands down DOWNARROW or MOUSEWHEEL DOWN
		///////////////////////////////////////////////////////
		else if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN) && (g_aQuickNUM != 0) && !m_bEx){
		
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){
				DWORD dwIndex = SearchItem(szTempStr);
				if(dwIndex >= g_aQuick.GetSize()){

					SetWindowText("");
					//Beep(1000, 50);
				}
				else{

					CString strOut = g_aQuick[dwIndex];
					CMetis3View::ReplaceVars(strOut);

					lstrcpy(szTempStr, strOut);

					SetWindowText(szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
				}
				return TRUE;
			}
			else{

				CString strOut = g_aQuick[0];
				CMetis3View::ReplaceVars(strOut);

				lstrcpy(szTempStr, strOut);
				SetWindowText(szTempStr);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				return TRUE;
			}
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}


HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{

	
	HBRUSH hbr;
	hbr = (HBRUSH)m_brBkgnd;
	pDC->SetBkColor(m_crDraw);

	return hbr;
	
}

void CMyEdit::OnKillFocus(CWnd* pNewWnd) 
{

	CEdit::OnKillFocus(pNewWnd);

	if(g_sSettings.GetFocus() && !m_bEx){

		m_crDraw = m_crBg;
		m_brBkgnd.DeleteObject(); 
		m_brBkgnd.CreateSolidBrush(m_crDraw);
		RedrawWindow();
	}
}

void CMyEdit::OnSetFocus(CWnd* pOldWnd) 
{

	CEdit::OnSetFocus(pOldWnd);
	if(g_sSettings.GetFocus() && !m_bEx){

		m_crDraw = m_crBgFocus;
		m_brBkgnd.DeleteObject();
		m_brBkgnd.CreateSolidBrush(m_crDraw);
		RedrawWindow();
	}
}

void CMyEdit::SetBkColor(COLORREF cr)
{

	m_crBg = cr;
	m_crBgFocus = g_sSettings.GetRGBFocus();
	Invalidate();
}

void CMyEdit::SetExtended()
{

	m_bEx = TRUE;
}
