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
#include "Settings.h"
#include "SettingsDlg.h"
#include "MainFrm.h"
#include ".\settingsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog
extern CSettings g_sSettings;

UINT UWM_LOAD_SETTINGS = ::RegisterWindowMessage("UWM_LOAD_SETTINGS_A1C54C04-EF90-43D7-9050-6CC3BCA18D22");

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDC_CAT, m_lcCat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CAT, OnItemchangedCat)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers
BOOL CSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::OnApply() 
{

	for(int i = 0; i < m_aDlgs.GetSize(); i++){

		((CCfgDlg*)m_aDlgs[i])->Apply();
	}
	g_sSettings.Save();

	((CMainFrame*)GetParent())->m_wndDocSelector.BroadcastMessage(UWM_LOAD_SETTINGS, 0, 0);
}


void CSettingsDlg::OnOK() 
{

	OnApply();
	CDialog::OnOK();
}

void CSettingsDlg::OnItemchangedCat(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CSettingsDlg::AddCategory(CCfgDlg *pCat, int nIcon, LPCTSTR lpszName)
{

	m_aDlgs.Add(pCat);
	m_lcCat.InsertItem(m_lcCat.GetItemCount(), lpszName, nIcon);
}



void CSettingsDlg::OnBnClickedCancel()
{

	OnCancel();
}
