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
#include "ChannelDoc.h"
#include "ChannelView.h"
#include "ServerDoc.h"
#include "ServerView.h"
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
CStringArray g_aRCMSCommands;

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_CHANNEL_CHANNELLIST, OnChannelChannellist)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_CHANNELLIST, OnUpdateChannelChannellist)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(IDR_START_NODESERVER, OnStartNodeserver)
	ON_UPDATE_COMMAND_UI(IDR_START_NODESERVER, OnUpdateStartNodeserver)
	ON_COMMAND(ID_VIEW_FULL_SCREEN, OnViewFullScreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_SCREEN, OnUpdateViewFullScreen)
	ON_COMMAND(AFX_IDS_SCNEXTWINDOW, OnIdsScnextwindow)
	ON_COMMAND(ID_VIEW_CHANNELBAR, OnViewChannelbar)
	ON_COMMAND(ID_MYFILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,			// status line indicator
	ID_SEPARATOR,			// status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bChannelList    = FALSE;
	m_bFullScreenMode = FALSE;
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

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if((::GetDeviceCaps(GetDC()->m_hDC,BITSPIXEL) > 8)) 
	{
	
		CImageList	imageList;
		CBitmap		bitmap;

		// Set up toolbar image lists.
		// Create and set the normal toolbar image list.
		bitmap.LoadMappedBitmap(IDB_TOOLBAR256);
		imageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 7, 1);
		imageList.Add(&bitmap, RGB(255,0,255));
		m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
		imageList.Detach();
		bitmap.Detach();

		// Create and set the disabled toolbar image list.
		bitmap.LoadMappedBitmap(IDB_TOOLBAR256_DEACT);
		imageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 7, 1);
		imageList.Add(&bitmap, RGB(255,0,255));
		m_wndToolBar.SendMessage( TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
		imageList.Detach();
		bitmap.Detach();
	}

	m_wndDocSelector.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | CBRS_TOP,
		         CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR);

	m_wndDocSelector.SetBarStyle(CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndDocSelector.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndDocSelector);

	m_wndStatusBar.SetPaneInfo(0,ID_SEPARATOR,SBPS_STRETCH,120);
    m_wndStatusBar.SetPaneInfo(1,ID_SEPARATOR,SBPS_NORMAL,240);
	m_wndStatusBar.SetPaneInfo(2,ID_SEPARATOR,SBPS_NORMAL,200);
	m_wndStatusBar.SetPaneInfo(3,ID_SEPARATOR,SBPS_NORMAL,100);

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
	
	LoadRCMS();
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
		ShowWindow(SW_RESTORE);
		SetFocus();
		SetForegroundWindow();
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

		if(m_bFullScreenMode){

			FullScreenModeOff();
		}
		this->ShowWindow(SW_HIDE);
	}
 	if ((nID & 0xFFF0) == SC_MAXIMIZE)
	{
		
		if(GetActiveFrame()->GetStyle() & WS_MAXIMIZE){

			MDIMaximize(GetActiveFrame());
		}
	}
}

void CMainFrame::OnStartNodeserver() 
{
	
	AfxMessageBox("Not implemented yet");
}

void CMainFrame::OnUpdateStartNodeserver(CCmdUI* pCmdUI) 
{
	
	pCmdUI->Enable(FALSE);
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

void CMainFrame::LoadRCMS()
{

	CString strIniFile = "RCMS.ini";
	CStdioFile ini;
	CString strBuffer;
	g_aRCMSCommands.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				g_aRCMSCommands.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

	}END_CATCH;

	if(g_aRCMSCommands.GetSize() == 0){

		g_aRCMSCommands.Add("Error Reading RCMS.ini. Please check your installation");
	}
}

void CMainFrame::FullScreenModeOn()
{

	// available only if there is an active doc
	CMDIChildWnd* pChild=MDIGetActive();
	if(!pChild) return;

	m_bToolBarWasVisible=(m_wndToolBar.IsWindowVisible()!=0);
	m_wndToolBar.ShowWindow(SW_HIDE);
	// first create the new toolbar
	// this will contain the full-screen off button
	m_pwndFullScreenBar=new CToolBar;
	m_pwndFullScreenBar->Create(this);
	m_pwndFullScreenBar->LoadToolBar(IDR_FULLSCREEN);
	m_pwndFullScreenBar->SetBarStyle(m_pwndFullScreenBar->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	// to look better:
	m_pwndFullScreenBar->ModifyStyle(0, TBSTYLE_FLAT);
	m_pwndFullScreenBar->EnableDocking(0);
	// place the full-screen off button somewhere:
	CPoint pt(300,200);
	FloatControlBar(m_pwndFullScreenBar,pt);
	
	// now save the old positions of the main and child windows
	GetWindowRect(&m_mainRect);
	// remove the caption of the mainWnd:
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
	style&=~WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);
	int screenx=GetSystemMetrics(SM_CXSCREEN);
	int screeny=GetSystemMetrics(SM_CYSCREEN);
	// resize:
	SetWindowPos(NULL,-4,-4,screenx+8,screeny+8,SWP_NOZORDER);
	style=::GetWindowLong(pChild->m_hWnd,GWL_STYLE);
	m_bChildMax=(style & WS_MAXIMIZE)?true:false;
	// note here: m_bMainMax is not needed since m_hWnd only
	// changed its caption...

	// and maximize the child window
	// it will remove its caption, too.
	pChild->ShowWindow(SW_SHOWMAXIMIZED);
	RecalcLayout();
	m_bFullScreenMode=true;

}

void CMainFrame::FullScreenModeOff()
{

	// You can use SaveBarState() in OnClose(),
	// so remove the newly added toolbar entirely
	// in order SaveBarState() not
	// to save its state. That is why I used dynamic
	// allocation
	delete m_pwndFullScreenBar;
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
	style|=WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);
	if(m_bToolBarWasVisible)
		m_wndToolBar.ShowWindow(SW_SHOW);
	if(m_bStatusBarWasVisible)
		m_wndStatusBar.ShowWindow(SW_SHOW);
	MoveWindow(&m_mainRect);
	RecalcLayout();
	CMDIChildWnd* pChild=MDIGetActive();
	// pchild can be NULL if the USER closed all the
	// childs during Full Screen Mode:
	if(pChild){
		if(m_bChildMax)
			MDIMaximize(pChild);
		else MDIRestore(pChild);
	}
	m_bFullScreenMode=false;
}

void CMainFrame::OnClose() 
{

	if(m_bFullScreenMode){

		FullScreenModeOff();
	}
	CMDIFrameWnd::OnClose();
}


void CMainFrame::OnViewFullScreen() 
{

	if(m_bFullScreenMode){

		FullScreenModeOff();
	}
	else{
		
		FullScreenModeOn();
	}
}

void CMainFrame::OnUpdateViewFullScreen(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(m_bFullScreenMode);	
}

void CMainFrame::OnIdsScnextwindow() 
{

	MDINext();
}

void CMainFrame::OnViewChannelbar() 
{

	m_wndDocSelector.ShowWindow(SW_HIDE);
}
