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
#include "MyListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{

	m_cr1 = RGB(0,0,0);
	m_cr2 = RGB(200,0,0);
	m_cr3 = RGB(0,150,0);
	m_cr4 = RGB(0,0,150);
	m_cr5 = RGB(0,0,180);
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
		ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdrawMyList )
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers

void CMyListCtrl::OnCustomdrawMyList(NMHDR* pNMHDR, LRESULT* pResult)
{

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.

    if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT){

        *pResult = CDRF_NOTIFYITEMDRAW;
    }
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
    else if(pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)){

        // This is the prepaint stage for an item. Here's where we set the
        // item's text color. Our return value will tell Windows to draw the
        // item itself, but it will use the new color we set here.
        // We'll cycle the colors through red, green, and light blue.

        COLORREF crText;

        if(pLVCD->iSubItem == 1){ // files

            crText = m_cr2;
		}
        else if(pLVCD->iSubItem == 2){ // line

            crText = m_cr3;
		}
        else if(pLVCD->iSubItem == 3){ // ip

            crText = m_cr4;
		}
        else if(pLVCD->iSubItem == 4){ // port

            crText = m_cr5;
		}
		else{

			crText = m_cr1;
		}

        // Store the color back in the NMLVCUSTOMDRAW struct.
        pLVCD->clrText = crText;
		pLVCD->clrTextBk = m_crBg;

        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
	}
}

int CMyListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_FULLROWSELECT);

	return 0;
}

void CMyListCtrl::SetColors(COLORREF cr1, COLORREF cr2, COLORREF cr3, COLORREF cr4, COLORREF cr5)
{

	m_cr1 = cr1;
	m_cr2 = cr2;
	m_cr3 = cr3;
	m_cr4 = cr4;
	m_cr5 = cr5;
}

void CMyListCtrl::SetBkColor(COLORREF cr)
{

	CListCtrl::SetBkColor(cr);
	m_crBg = cr;
}
