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
#include "ChannelDoc.h"
#include "ChannelView.h"
#include "MainFrm.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;
extern UINT UWM_LOAD_SETTINGS;

/////////////////////////////////////////////////////////////////////////////
// CChannelView

IMPLEMENT_DYNCREATE(CChannelView, CFormView)

CChannelView* CChannelView::m_pThis = NULL;

CChannelView::CChannelView()
	: CFormView(CChannelView::IDD)
{
	m_pThis = this;
	m_bNoScroll = FALSE;
}

CChannelView::~CChannelView()
{
}

void CChannelView::DoDataExchange(CDataExchange* pDX) 
{
	
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelView)
	DDX_Control(pDX, IDC_CHANNELS, m_lcList);
	DDX_Text(pDX, IDC_SEARCH, m_strSearch);
	DDX_Text(pDX, IDC_STATIC_MESSAGES, m_strMsg);
	DDX_Check(pDX, IDC_SELECTION_NOSCROLL, m_bNoScroll);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChannelView, CFormView)
	//{{AFX_MSG_MAP(CChannelView)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CLEAR_REFRESH, OnClearRefresh)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_SEARCH, OnChangeSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHANNELS, OnItemchangedChannels)
	ON_NOTIFY(NM_RCLICK, IDC_CHANNELS, OnRclickChannels)
	ON_NOTIFY(NM_DBLCLK, IDC_CHANNELS, OnDblclkChannels)
	ON_BN_CLICKED(IDC_SELECTION_NOSCROLL, OnSelectionNoscroll)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UWM_LOAD_SETTINGS, OnReloadCfg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelView drawing

void CChannelView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CChannelView diagnostics

#ifdef _DEBUG
void CChannelView::AssertValid() const
{
	CView::AssertValid();
}

void CChannelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChannelView message handlers
#define TIMER_LOAD 10000

void CChannelView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit(FALSE);
	m_pStatusBar = (CColorStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.AddButton( this, IDR_LIST );

	m_lcList.InsertColumn(0, "Room", LVCFMT_LEFT, 300);
	m_lcList.InsertColumn(1, "Users", LVCFMT_RIGHT, 50);
	m_lcList.InsertColumn(2, "Limit", LVCFMT_RIGHT, 40);
	m_lcList.InsertColumn(3, "Topic", LVCFMT_LEFT, 300);
	ListView_SetExtendedListViewStyle(m_lcList.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lcList.SetColors(g_sSettings.GetRGBNormalMsg(), RGB(0, 128, 0), RGB(128, 0, 0), RGB(0, 64, 128), RGB(0, 64, 128));
	m_lcList.SetBkColor(g_sSettings.GetRGBBg());

	m_mxClient.SetCallBackProck(CChannelView::ClientCallback, (DWORD)this);
	m_mxClient.Connect(NULL, "ROBOMX000", 0, 6699);
}

LRESULT CChannelView::OnReloadCfg(WPARAM w, LPARAM l)
{

	m_lcList.SetColors(g_sSettings.GetRGBNormalMsg(), RGB(0, 128, 0), RGB(128, 0, 0), RGB(0, 64, 128), RGB(0, 64, 128));
	m_lcList.SetBkColor(g_sSettings.GetRGBBg());

	Invalidate();

	return 1;
}

void CChannelView::OnDestroy() 
{

	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.RemoveButton(this);

	m_mxClient.Disconnect();
	CFormView::OnDestroy();
	((CMainFrame*)GetApp()->m_pMainWnd)->m_bChannelList = FALSE;
	((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom.Empty();
}

void CChannelView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if(!m_lcList.m_hWnd){

		return;
	}

	CRect	rcItem;
	CRect	rcParent;

	GetClientRect(rcParent);

	// resize userlist
	m_lcList.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	
	rcItem.right = rcParent.right - 1;
	rcItem.bottom = rcParent.bottom -1;

	m_lcList.MoveWindow(rcItem);
	
}

void CChannelView::OnClearRefresh() 
{

	m_lcList.DeleteAllItems();
	m_mxClient.Connect(NULL, "ROBOMX000", 0, 6699);
	GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
	GetDlgItem(IDC_CLEAR_REFRESH)->EnableWindow(FALSE);
}

void CChannelView::OnRefresh() 
{

	m_mxClient.Connect(NULL, "ROBOMX000", 0, 6699);
	GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
	GetDlgItem(IDC_CLEAR_REFRESH)->EnableWindow(FALSE);
}

BOOL CChannelView::ClientCallback(DWORD dwParam, WPARAM wParam, LPARAM lParam)
{

	switch(wParam){

		case CLN_ERROR :
			switch(lParam){
				case CLNC_WPNCONNECTFAILED :
					m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "Could not connect to WPN");
					return TRUE;

				case CLNC_WPNGETPARENTFAILED :
					m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "Connection to Parent Node failed.");
					return TRUE;
				}
			break;		

		case CLN_WPNCONNECTSTART :
			m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "Connecting to WPN");
			return TRUE; 

		case CLN_WPNCONNECTED :
			m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "Loading channel list...");
			m_pThis->m_nLastItem = GetTickCount();
			m_pThis->m_mxClient.SendQueStatus(1, 1, 0);
			m_pThis->m_mxClient.SendFormatMessage(WPN_ENUMCHATROOMS, "D", 0);
			m_pThis->SetTimer(TIMER_LOAD, 500, 0);
			return TRUE;

		case CLN_CHANGEPARENTSTART :
			m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "");
			return TRUE;
		
		case CLN_CHANGEPARENTCOMP :
			m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "");
			return TRUE;

		case CLN_PARENTDISCONNECTED :
			m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "Parent node closed connection");
			return TRUE;	

		case CLN_WPNDISCONNECTED :
			m_pThis->SetDlgItemText(IDC_STATIC_MESSAGES, "WPN Disconnected");
			return TRUE;
		
		case CLN_CHATROOMINFO : {
			CHATROOMINFO *pRoomInfo = (CHATROOMINFO *)lParam;
			m_pThis->UpdateRoomItem(pRoomInfo->lpszRoomName, (WORD)pRoomInfo->dwUsers, (WORD)pRoomInfo->dwLimit, pRoomInfo->lpszTopic);
			return TRUE;
			}
		}

	return FALSE;
}

void CChannelView::UpdateRoomItem(LPCTSTR lpszRoomName, WORD wUsers, WORD wLimit, LPCTSTR lpszTopic)
{

	m_nLastItem = GetTickCount();
	LVFINDINFO lvi;

	lvi.flags = LVFI_STRING;
	lvi.psz   = lpszRoomName;
	if(m_lcList.FindItem(&lvi, -1) == -1){

		CString strTmp;
		int nPos = m_lcList.InsertItem(m_lcList.GetItemCount(), lpszRoomName);
		strTmp.Format("%d", wUsers);
		m_lcList.SetItemText(nPos, 1, strTmp);
		strTmp.Format("%d", wLimit);
		m_lcList.SetItemText(nPos, 2, strTmp);
		m_lcList.SetItemText(nPos, 3, lpszTopic);

		strTmp.Format("%04d Rooms", m_lcList.GetItemCount());
		GetDlgItem(IDC_STATIC_ROOMS)->SetWindowText(strTmp);
		if(m_bNoScroll){

			int n = m_lcList.GetNextItem(-1, LVNI_SELECTED);
			if(n >= 0){

				m_lcList.EnsureVisible(n, FALSE);
			}
		}
	}

}

void CChannelView::OnTimer(UINT nIDEvent) 
{

	if(nIDEvent == TIMER_LOAD){

		if((GetTickCount() - m_nLastItem) >= 10000){

			m_mxClient.Disconnect();
			GetDlgItem(IDC_REFRESH)->EnableWindow(TRUE);
			GetDlgItem(IDC_CLEAR_REFRESH)->EnableWindow(TRUE);
			SetDlgItemText(IDC_STATIC_MESSAGES, "Finished.");
			KillTimer(TIMER_LOAD);
		}
		if(m_pStatusBar){

			if(((CMainFrame*)GetApp()->m_pMainWnd)->GetActiveFrame() == GetParentFrame()){
				m_pStatusBar->SetPaneText(1, "Channel List");
				CString strText;
				strText.Format("%d channels", m_lcList.GetItemCount());
				m_pStatusBar->SetLagColor(0, strText);
				m_pStatusBar->SetPaneText(2, strText);
				m_pStatusBar->SetPaneText(3, "");
			}
		}
	}
	CFormView::OnTimer(nIDEvent);
}

void CChannelView::OnChangeSearch() 
{

	UpdateData(TRUE);

	m_nLastItem = GetTickCount();
	LVFINDINFO lvi;

	lvi.flags = LVFI_STRING|LVFI_PARTIAL;
	lvi.psz   = m_strSearch;
	int n = m_lcList.FindItem(&lvi, -1);
	if(n != -1){

		//m_lcList.SetSelectionMark(n);
		m_lcList.SetItemState(n, LVNI_SELECTED, LVNI_SELECTED);
		m_lcList.EnsureVisible(n, FALSE);
	}
}

void CChannelView::OnItemchangedChannels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->uNewState & LVNI_SELECTED){

		m_lcList.GetItemText(pNMListView->iItem, 0);
		((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom = m_lcList.GetItemText(pNMListView->iItem, 0);
	}
	*pResult = 0;
}

void CChannelView::OnRclickChannels(NMHDR* pNMHDR, LRESULT* pResult) 
{

	CMenu mContextMenu;
	mContextMenu.LoadMenu(IDR_LISTRMENU);

	POINT point;
	GetCursorPos(&point);

	if(m_lcList.GetNextItem(-1, LVNI_FOCUSED) >= 0){

		mContextMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
											point.x,
											point.y,
											AfxGetMainWnd());
	}

	*pResult = 0;
}

void CChannelView::OnDblclkChannels(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->iItem >= 0){

		m_lcList.GetItemText(pNMListView->iItem, 0);
		((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom = m_lcList.GetItemText(pNMListView->iItem, 0);
		((CMainFrame*)GetApp()->m_pMainWnd)->JoinChannel();
	}
	*pResult = 0;
}

void CChannelView::OnSelectionNoscroll() 
{

	UpdateData(TRUE);
}
