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
#include "MainFrm.h"
#include "ChannelView.h"
#include "Metis3Doc.h"
#include "Metis3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_CHANNEL_CHANNELLIST, OnChannelChannellist)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_CHANNELLIST, OnUpdateChannelChannellist)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_MYFILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bChannelList = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_hIcon = (HICON)LoadImage(GetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );


	LoadToTray(
		m_hWnd,
		WM_TRAY_ICON_NOTIFY_MESSAGE, 
		"RoboMX [:]", 
		"",
		"RoboMX [:]", 
		1, 
		m_hIcon,
		NIIF_INFO
	); 

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnAppExit()
{

	//GetApp()->m_pView->m_mxClient.Disconnect();
	OnClose();
}

void CMainFrame::OnViewOptions() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("Not implemented yet");
}

// dwIcon may be
// - NIIF_ERROR     An error icon. 
// - NIIF_INFO      An information icon. 
// - NIIF_NONE      No icon. 
// - NIIF_WARNING   A warning icon. 
// - NIIF_ICON_MASK Version 6.0. Reserved. 
// - NIIF_NOSOUND   Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips 
// UTimeOut
//  min 10 secs, max 30 secs (enforced by windows :-( )
void CMainFrame::LoadToTray(HWND hWnd, UINT uCallbackMessage, CString strInfoTitle, CString strInfo, CString strTip, int uTimeout, HICON icon, DWORD dwIcon)
{

	//NOTIFYICONDATA contains information that the system needs to process taskbar status area messages

	ZeroMemory( &m_nIconData, sizeof( NOTIFYICONDATA ) );

	m_nIconData.cbSize		      = sizeof( NOTIFYICONDATA );
	m_nIconData.hWnd			  = hWnd;
	m_nIconData.uID			      = 120132;
	
	if(strInfo.IsEmpty()){

		m_nIconData.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	}
	else{

		m_nIconData.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	}
			// Flag Description:
			// - NIF_ICON	 The hIcon member is valid.  
	        // - NIF_MESSAGE The uCallbackMessage member is valid. 
	        // - NIF_TIP	 The szTip member is valid. 
	        // - NIF_STATE	 The dwState and dwStateMask members are valid. 
	        // - NIF_INFO	 Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid. 
            // - NIF_GUID	 Reserved. 

	m_nIconData.dwInfoFlags      = dwIcon; // add an icon to a balloon ToolTip

	m_nIconData.uCallbackMessage = uCallbackMessage;  
	m_nIconData.uTimeout         = uTimeout * 1000;
	m_nIconData.hIcon		   	 = icon;

	strcpy( m_nIconData.szInfoTitle, strInfoTitle );
	strcpy( m_nIconData.szInfo,      strInfo      );
	strcpy( m_nIconData.szTip,       strTip       );

	Shell_NotifyIcon( NIM_ADD, &m_nIconData ); // add to the taskbar's status area
}


LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{

    UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam; 
    uMsg = (UINT) lParam; 
 
	if (uID != 120132)
		return 0;
	
	CPoint pt;	

    switch (uMsg ) 
	{ 
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_SHOWNORMAL);
		SetFocus();
		SetForegroundWindow( );
		break;
	
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		break;
    } 
	return 1;
}

void CMainFrame::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
	
	if(m_nIconData.hWnd && m_nIconData.uID>0)
	{
		Shell_NotifyIcon(NIM_DELETE,&m_nIconData);
	}
	
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{

	CMDIFrameWnd::OnSysCommand(nID, lParam);

 	if ((nID & 0xFFF0) == SC_MINIMIZE)
	{
		this->ShowWindow(SW_HIDE);
	}
}

void CMainFrame::OnChannelChannellist() 
{
	
	if(!m_bChannelList){

		POSITION pos = GetApp()->GetFirstDocTemplatePosition();
		CDocTemplate* pTemplate = GetApp()->GetNextDocTemplate(pos);
		pTemplate->OpenDocumentFile(NULL);
		m_bChannelList = TRUE;
	}

}

void CMainFrame::OnUpdateChannelChannellist(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(!m_bChannelList);
}


void CMainFrame::OnFileNew() 
{

	POSITION pos = GetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = GetApp()->GetNextDocTemplate(pos);

	pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate->OpenDocumentFile(NULL);  // creates the second document
	
}

void CMainFrame::JoinChannel()
{

	OnFileNew();
}
