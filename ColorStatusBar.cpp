// ColorStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "ColorStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar

CColorStatusBar::CColorStatusBar()
{
}

CColorStatusBar::~CColorStatusBar()
{
}


BEGIN_MESSAGE_MAP(CColorStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CColorStatusBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar message handlers

void CColorStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	if(lpDrawItemStruct->itemID == 2){
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);

		dc.SetBkMode(TRANSPARENT);

		// Get the pane rectangle and calculate text coordinates
		CRect rect(&lpDrawItemStruct->rcItem);


			dc.SetTextColor(m_crLag);
			dc.TextOut(rect.left+2, rect.top, m_strLag);

		dc.Detach();
	}
}

void CColorStatusBar::SetLagColor(COLORREF cr, CString strLag)
{

	m_strLag = strLag;
	m_crLag  = cr;
}

void CColorStatusBar::SetPaneText(DWORD dwIndex, CString strText, COLORREF cr)
{

	if(!::IsWindow(m_hWnd)) return;
	CStatusBar::SetPaneText(dwIndex, strText, TRUE);

	if(dwIndex == 2){

		GetStatusBarCtrl().SetText(strText, 2, SBT_OWNERDRAW); 
		m_crLag = cr;
		m_strLag = strText;
	}
	Invalidate();
}
