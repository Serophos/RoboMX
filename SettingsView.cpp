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
#include "SettingsView.h"
#include "MainFrm.h"
#include "Settings.h"
#include ".\settingsview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;

UINT UWM_LOAD_SETTINGS = ::RegisterWindowMessage("UWM_LOAD_SETTINGS_A1C54C04-EF90-43D7-9050-6CC3BCA18D22");

/////////////////////////////////////////////////////////////////////////////
// CSettingsView

IMPLEMENT_DYNCREATE(CSettingsView, CFormView)


CSettingsView::CSettingsView()
	: CFormView(CSettingsView::IDD)
{
}

CSettingsView::~CSettingsView()
{
}

void CSettingsView::DoDataExchange(CDataExchange* pDX) 
{

	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAT, m_lcCat);

}

BEGIN_MESSAGE_MAP(CSettingsView, CFormView)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_OK, OnOK)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CAT, OnItemchangedCat)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsView drawing

void CSettingsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsView diagnostics

#ifdef _DEBUG
void CSettingsView::AssertValid() const
{
	CView::AssertValid();
}

void CSettingsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSettingsView message handlers
#define TIMER_LOAD 10000

void CSettingsView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit(FALSE);
	m_pStatusBar = (CColorStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.AddButton( this, IDR_SETTINGS );

	HIMAGELIST hList = ImageList_Create(32,32, ILC_COLOR8 |ILC_MASK , 5, 1);
	m_cImageList.Attach(hList);

	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_PREF);
	m_cImageList.Add(&cBmp, RGB(255,0, 255));
	cBmp.DeleteObject();
	
	m_lcCat.SetImageList(&m_cImageList, LVSIL_NORMAL);
	m_lcCat.SetIconSpacing(48,32);

	m_lcCat.InsertColumn(0, "Categories", LVCFMT_CENTER, 52, 0);
	m_lcCat.SetIconSpacing(48,32);
	
	CRect rcClient;
	GetDlgItem(IDC_STATIC_CLIENT)->GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	cfgGen.Create(IDD_GENERAL, this);
	cfgGen.MoveWindow(rcClient);

	cfgColor.Create(IDD_COLORS, this);
	cfgColor.MoveWindow(rcClient);

	cfgPeer.Create(IDD_PAGERCFG, this);
	cfgPeer.MoveWindow(rcClient);

	cfgMsg.Create(IDD_MESSAGES, this);
	cfgMsg.MoveWindow(rcClient);

	cfgSfx.Create(IDD_SOUND, this);
	cfgSfx.MoveWindow(rcClient);

	cfgFun.Create(IDD_FUN, this);
	cfgFun.MoveWindow(rcClient);

	AddCategory(&cfgGen, 0, "General");
	AddCategory(&cfgColor, 1, "Colors");
	AddCategory(&cfgPeer, 5, "Server");
	AddCategory(&cfgMsg, 2, "Messages");
	AddCategory(&cfgSfx, 3, "Sound FX");
	AddCategory(&cfgFun, 4, "Fun :-)");

	m_lcCat.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);
	cfgGen.ShowWindow(SW_SHOW);
}

void CSettingsView::OnDestroy() 
{

	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.RemoveButton(this);
	((CMainFrame*)AfxGetMainWnd())->m_bSettings = FALSE;
	CFormView::OnDestroy();
}

void CSettingsView::OnApply() 
{

	for(int i = 0; i < m_aDlgs.GetSize(); i++){

		((CCfgDlg*)m_aDlgs[i])->Apply();
	}
	g_sSettings.Save();

	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.BroadcastMessage(UWM_LOAD_SETTINGS, 0, 0);
}


void CSettingsView::OnOK() 
{

	OnApply();
	GetDocument()->OnCloseDocument();
}

void CSettingsView::OnItemchangedCat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int n = m_lcCat.GetNextItem(-1, LVNI_SELECTED);

	if((n >= 0) && (n < m_aDlgs.GetSize())){

		for(int i = 0; i < m_aDlgs.GetSize(); i++){

			((CCfgDlg*)m_aDlgs[i])->ShowWindow(SW_HIDE);
		}
		((CCfgDlg*)m_aDlgs[n])->ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}

void CSettingsView::AddCategory(CCfgDlg *pCat, int nIcon, LPCTSTR lpszName)
{

	m_aDlgs.Add(pCat);
	m_lcCat.InsertItem(m_lcCat.GetItemCount(), lpszName, nIcon);
}



void CSettingsView::OnBnClickedCancel()
{

	GetDocument()->OnCloseDocument();
}

