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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

UINT UWM_INPUT = ::RegisterWindowMessage("UWM_INPUT-7A14F66B-ABAB-4525-AC01-841C82EC48B6");

extern CStringArray g_aRCMSCommands;

#define g_aRCMSCommandsNUM g_aRCMSCommands.GetSize() 

CMyEdit::CMyEdit()
{

	m_crBg = ::GetSysColor(COLOR_WINDOW); // Initializing the Background Color to the system face color.
	m_brBkgnd.CreateSolidBrush(m_crBg); // Create the Brush Color for the Background.
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


BOOL CMyEdit::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){

		::SendMessage(GetParent()->m_hWnd, UWM_INPUT, 0, 0);
	}
	else if(pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_KEYDOWN){


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
		else if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_NEXT) || 
			(pMsg->message == WM_MOUSEWHEEL && ((short)HIWORD(pMsg->wParam)) <= 0)){
		
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
	}

	return CEdit::PreTranslateMessage(pMsg);
}


HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{


	HBRUSH hbr;
	hbr = (HBRUSH)m_brBkgnd;
	pDC->SetBkColor(m_crBg);

//	if (nCtlColor)       // To get rid of compiler warning
  //    nCtlColor += 0;

	return hbr;
	
}

void CMyEdit::OnKillFocus(CWnd* pNewWnd) 
{

	CEdit::OnKillFocus(pNewWnd);

	m_crBg = ::GetSysColor(COLOR_WINDOW);
	m_brBkgnd.DeleteObject(); 
	m_brBkgnd.CreateSolidBrush(m_crBg);
	RedrawWindow();
}

void CMyEdit::OnSetFocus(CWnd* pOldWnd) 
{

	CEdit::OnSetFocus(pOldWnd);
	
	m_crBg = RGB(255, 245, 210);
	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(m_crBg);
	RedrawWindow();
}
