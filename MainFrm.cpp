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
#include "ChildFrm.h"
#include "ChannelView.h"
#include "Metis3Doc.h"
#include "Metis3View.h"
#include "SettingsDlg.h"
#include "Settings.h"
#include <mmsystem.h>
#include "Tokenizer.h"
#include "SystemInfo.h"
#include "ini.h"
#include "RoboEx.h"
#include ".\mainfrm.h"
#include "EmoticonManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPtrArray g_aPlugins;

#define WM_ECHOTEXT			WM_APP+1
#define WM_ECHOSYSTEXT		WM_APP+2
#define WM_INPUT			WM_APP+3
#define WM_DPLTOOLTIP		WM_APP+4
#define WM_INCOMMING		WM_APP+10

extern UINT UWM_INPUT;
extern CSystemInfo  g_sSystem;
extern CSettings    g_sSettings;

CEmoticonManager    g_eEmoticons;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)
#define TIMER_ANIMATE 101010

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_CHANNEL_CHANNELLIST, OnChannelChannellist)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL_CHANNELLIST, OnUpdateChannelChannellist)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_VIEW_FULL_SCREEN, OnViewFullScreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_SCREEN, OnUpdateViewFullScreen)
	ON_COMMAND(AFX_IDS_SCNEXTWINDOW, OnIdsScnextwindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHANNELBAR, OnUpdateViewChannelBar)
	ON_COMMAND(ID_VIEW_CHANNELBAR, OnViewChannelbar)
	ON_COMMAND(ID_MYFILE_NEW, OnFileNew)
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)
	ON_MESSAGE(WM_INPUT, OnPluginInput)
	ON_MESSAGE(WM_ECHOTEXT, OnPluginEcho)
	ON_MESSAGE(WM_DPLTOOLTIP, OnTooltip)
	ON_MESSAGE(WM_ECHOSYSTEXT, OnPluginSysEcho)
	ON_COMMAND(IDR_START_NODESERVER, OnStartNodeserver)
	ON_COMMAND(ID_SYSTRAY_RESTORE, OnSystrayRestore)
	ON_COMMAND(ID_RECONNECT_ALL, OnReconnectAll)
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

	m_bChannelList    = FALSE;
	m_bFullScreenMode = FALSE;
	m_bSettings       = FALSE;
	m_bAway			  = FALSE;
	m_bQuickJoin	  = FALSE;
	m_hIcon			  = NULL;
	m_hIcon2		  = NULL;
	m_nIcon			  = 0;
	m_pMenuNew		  = NULL;
	m_hInstDefault	  = NULL;
	m_hInstUse		  = NULL;
	m_nMaxAni		  = 0;
}

CMainFrame::~CMainFrame()
{

	UnloadPlugins();
	//DeleteEmoticons();
	g_eEmoticons.Free();
	if(m_hIcon)
		DeleteObject(m_hIcon);
	if(m_hIcon2)
		DeleteObject(m_hIcon2);
	if(m_pMenuNew){

		m_pMenuNew->DestroyMenu();
		delete m_pMenuNew;
		m_pMenuNew = NULL;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_hIcon = (HICON)LoadImage(GetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );
	m_hIcon2 = (HICON)LoadImage(GetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ANI), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR );

	m_cmSystray.LoadMenu(IDR_SYSTRAY);
	LoadToTray(
		m_hWnd,
		WM_TRAY_ICON_NOTIFY_MESSAGE, 
		GetApp()->m_strAppTitle, 
		"",
		GetApp()->m_strAppTitle, 
		1, 
		m_hIcon,
		NIIF_INFO
	); 

	//StartAni();

	if (!m_wndToolBarStd.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarStd.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if((::GetDeviceCaps(GetDC()->m_hDC,BITSPIXEL) > 8)) 
	{
	
		CImageList	imageList;
		CBitmap		bitmap;

		// Set up toolbar image lists.
		// Create and set the normal toolbar image list.
		bitmap.LoadMappedBitmap(IDB_TOOLBAR256);
		imageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 7, 1);
		imageList.Add(&bitmap, RGB(255,0,255));
		m_wndToolBarStd.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
		imageList.Detach();
		bitmap.Detach();

		// Create and set the disabled toolbar image list.
		bitmap.LoadMappedBitmap(IDB_TOOLBAR256_DEACT);
		imageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 7, 1);
		imageList.Add(&bitmap, RGB(255,0,255));
		m_wndToolBarStd.SendMessage( TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
		imageList.Detach();
		bitmap.Detach();
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	

	m_wndToolBarStd.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBarStd);

	m_wndDocSelector.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | CBRS_BOTTOM | CBRS_TOP,
		         CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR);

	m_wndDocSelector.SetBarStyle(CBRS_ALIGN_BOTTOM|CBRS_ALIGN_TOP);
	if(!g_sSettings.GetBarTop()){

		m_wndDocSelector.EnableDocking(CBRS_ALIGN_BOTTOM|CBRS_ALIGN_TOP);
		DockControlBar(&m_wndDocSelector, AFX_IDW_DOCKBAR_BOTTOM);
	}

	m_wndStatusBar.SetPaneInfo(0,ID_SEPARATOR,SBPS_STRETCH,120);
    m_wndStatusBar.SetPaneInfo(1,ID_SEPARATOR,SBPS_NORMAL,240);
	m_wndStatusBar.SetPaneInfo(2,ID_SEPARATOR,SBPS_NORMAL,200);
	m_wndStatusBar.SetPaneInfo(3,ID_SEPARATOR,SBPS_NORMAL,100);
	m_wndStatusBar.GetStatusBarCtrl().SetText("", 2, SBT_OWNERDRAW); 

	RecalcLayout();

	SetWindowText(GetApp()->m_pszAppName);

	if(g_sSettings.GetSoundFX()){

		PlaySound(g_sSettings.GetSfxStart(), 0, SND_FILENAME|SND_ASYNC);
	}

	//LoadEmoticons();
	g_eEmoticons.Load();

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	CRect rect;
	GetDesktopWindow()->GetWindowRect(rect);
	if(rect.Width() >= 1024){

		cs.cx = 800;
		cs.cy = 700;
	}


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


void CMainFrame::StopAni(void)
{

	if(m_nIcon < 2){

		KillTimer(TIMER_ANIMATE);
		m_nIcon = 2;
		ModifyTrayIcon(m_hIcon);
	}
}

void CMainFrame::StartAni(void)
{
	if(m_nIcon == 2){

		m_nIcon = 0;
		SetTimer(TIMER_ANIMATE, 500, 0);
	}
}

void CMainFrame::OnTimer(UINT nIDEvent)
{

	if(nIDEvent == TIMER_ANIMATE){

		m_nMaxAni++;
		if(m_nIcon == 0){

			m_nIcon = 1;
			ModifyTrayIcon(m_hIcon);
		}
		else{

            m_nIcon = 0;
			ModifyTrayIcon(m_hIcon2);
		}
		if(m_nMaxAni >= 50) StopAni();
	}
	else{

		CMDIFrameWnd::OnTimer(nIDEvent);
	}

}

void CMainFrame::DisplayToolTip(CString strMessage, UINT uTimeout, DWORD dwIcon)
{

	m_nIconData.uFlags		     = NIF_TIP | NIF_INFO;
	m_nIconData.dwInfoFlags      = dwIcon; // add an icon to a balloon ToolTip
	m_nIconData.uTimeout         = uTimeout * 1000;
	strcpy( m_nIconData.szInfo, strMessage);

	Shell_NotifyIcon(NIM_MODIFY, &m_nIconData); // add to the taskbar's status area
}

void CMainFrame::ModifyTrayIcon(HICON hIcon)
{

	m_nIconData.uFlags	= NIF_ICON;
	m_nIconData.hIcon	= hIcon;

	Shell_NotifyIcon(NIM_MODIFY, &m_nIconData);
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
		m_cmSystray.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
		m_cmSystray.GetSubMenu(0)->SetDefaultItem(0,TRUE);
		break;
	case 0x00000405:
		::ShellExecute(	0,
				"open",
				"http://mxcontrol.sourceforge.net",
				0,
				0,
				SW_SHOW
			);
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

	g_sSettings.Save();
	
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{

	CMDIFrameWnd::OnSysCommand(nID, lParam);

 	if(((nID & 0xFFF0) == SC_MINIMIZE) && g_sSettings.GetMiniTray())
	{

		if(m_bFullScreenMode){

			FullScreenModeOff();
		}
		this->ShowWindow(SW_HIDE);
	}
}


void CMainFrame::OnChannelChannellist() 
{
	
	if(!m_bChannelList){

		// Create a new ChannelListClient (only one allowed)
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

	// Create a new ChatClientDocument
	POSITION pos = GetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate->OpenDocumentFile(NULL);
}


void CMainFrame::OnStartNodeserver()
{

	// Create a new ChatServerDocument
	POSITION pos = GetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate = GetApp()->GetNextDocTemplate(pos);
	pTemplate->OpenDocumentFile(NULL);	  
	//AfxMessageBox("Sorry, the roomserver is disabled in this release.", MB_ICONINFORMATION);
}


void CMainFrame::OnViewOptions() 
{

	// Create a new SettingsDocument
	if(!m_bSettings){

		POSITION pos = GetApp()->GetFirstDocTemplatePosition();
		CDocTemplate* pTemplate = GetApp()->GetNextDocTemplate(pos);
		pTemplate = GetApp()->GetNextDocTemplate(pos);
		pTemplate = GetApp()->GetNextDocTemplate(pos);
		pTemplate->OpenDocumentFile(NULL); 
		m_bSettings = TRUE;
	}
	else{

		CString strText;
		CString strSettings;
		strSettings.LoadString(IDS_SETTINGS);
		MDIGetActive()->GetWindowText(strText);
		while(strText.Compare(strSettings) != 0){
			
			this->MDINext();
			MDIGetActive()->GetWindowText(strText);
		}
	}
}


void CMainFrame::JoinChannel()
{

	OnFileNew();
}


void CMainFrame::FullScreenModeOn()
{

	// available only if there is an active doc
	CMDIChildWnd* pChild=MDIGetActive();
	if(!pChild) return;

	m_bToolBarWasVisible=(m_wndToolBarStd.IsWindowVisible()!=0);
	m_wndToolBarStd.ShowWindow(SW_HIDE);
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
		m_wndToolBarStd.ShowWindow(SW_SHOW);
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

	m_wndDocSelector.ShowWindow(m_wndDocSelector.IsWindowVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewChannelBar(CCmdUI* pCmdUI)
{

	pCmdUI->SetCheck(m_wndDocSelector.IsWindowVisible());
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{

	//CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return; // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	
	if(bAddToTitle){
		
		TCHAR szText[256+_MAX_PATH];
		CString strTitle;

		strTitle = AfxGetApp()->m_pszAppName;

		if (pDocument == NULL){

			lstrcpy(szText, m_strTitle);
		}
		else{

			lstrcpy(szText, pDocument->GetTitle());
		}
	
		if (m_nWindow > 0){
			
			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);
		}

		// set title if changed, but don't remove completely
		SetWindowText(szText);
		lstrcat(szText, (char *)((LPCTSTR) strTitle));
		AfxGetMainWnd()->SetWindowText(szText);
	}
}


void CMainFrame::LoadPlugins(void)
{

	int i = 0;
	CString strFilename;
	CFileFind finder;

	BOOL bResult = finder.FindFile(g_sSettings.GetWorkingDir() + _T("\\Add-Ons\\*.rEx"));
	while(bResult){

		bResult = finder.FindNextFile();
		strFilename = finder.GetFilePath();

		HINSTANCE hDLL = LoadLibrary(strFilename);
		if(hDLL == NULL){
		
			CString strError;
			strError.Format(IDS_ERROR_PLUGIN_LOAD, strFilename);
			AfxMessageBox(strError, MB_ICONSTOP);
			continue;
		}
		
		REGISTERPLUGIN pRegister = (REGISTERPLUGIN)GetProcAddress(hDLL, "RegisterExtension");

		if(pRegister == NULL){
		
			CString strError;
			strError.Format(IDS_ERROR_INVALID_PLUGIN, finder.GetFileName());
			AfxMessageBox(strError, MB_ICONSTOP);
			continue;
		}
	
		CRoboEx* pEx = pRegister();

		if(pEx == NULL){

			CString strError;
			strError.Format(IDS_ERROR_PLUGIN_INIT, finder.GetFileName());
			AfxMessageBox(strError, MB_ICONSTOP);
			continue;
		}
		
		pEx->Init();
		g_aPlugins.Add(pEx);
		m_aMods.Add(hDLL);
	}
}

void CMainFrame::UnloadPlugins(void)
{

	for(int i = 0; i < g_aPlugins.GetSize(); i++){

		((CRoboEx*)g_aPlugins[i])->Quit();
	}
	g_aPlugins.RemoveAll();

	for(i = 0; i < m_aMods.GetSize(); i++){

		FreeLibrary(m_aMods[i]);
	}
	m_aMods.RemoveAll();
}

BOOL CMainFrame::DeletePlugin(CString strName)
{
	
	for(int i = 0; i < g_aPlugins.GetSize(); i++){

		if(((CRoboEx*)g_aPlugins[i])->m_strName == strName){

			((CRoboEx*)g_aPlugins[i])->Quit();
			g_aPlugins.RemoveAt(i, 1);
			FreeLibrary(m_aMods[i]);
			m_aMods.RemoveAt(i);
			TRY{

				CFile::Remove(g_sSettings.GetWorkingDir() + "\\Add-Ons\\" + strName);
			}
			CATCH(CFileException, e){

				return 0;
			}END_CATCH;

			return 1;
		}
	}
	return 0;
}


void CMainFrame::ReloadPlugins(void)
{

	UnloadPlugins();
	LoadPlugins();
	if(g_aPlugins.GetSize() == 0) return;

	for(int i = 0; i < m_wndDocSelector.m_Buttons.GetSize(); i++){

		if(((CSwitcherButton*)m_wndDocSelector.m_Buttons[i])->m_bIsChatClient){

			for(int j = 0; j < g_aPlugins.GetSize(); j++){

				((CRoboEx*)g_aPlugins[j])->OnJoinChannel((DWORD)((CSwitcherButton*)m_wndDocSelector.m_Buttons[i])->m_AttachedView->m_hWnd, 
						((CMetis3View*)((CSwitcherButton*)m_wndDocSelector.m_Buttons[i])->m_AttachedView)->GetDocument()->m_strRoom,
						&((CMetis3View*)((CSwitcherButton*)m_wndDocSelector.m_Buttons[i])->m_AttachedView)->m_aUsers
						);
					
			}
		}
	}
}

LRESULT CMainFrame::OnTooltip(WPARAM wParam, LPARAM lParam)
{

	EXT_MSG_STRUCT* msg = (EXT_MSG_STRUCT*)lParam;
	DisplayToolTip(msg->lpszMsg, (UINT)msg->wParam, (DWORD)msg->lParam);

	return 0;
}

LRESULT CMainFrame::OnPluginInput(WPARAM wParam, LPARAM lParam)
{

	m_wndDocSelector.DeliverMessage((DWORD)wParam, UWM_INPUT, 1, (LPARAM)((EXT_MSG_STRUCT*)lParam)->lpszMsg);
	return 0;
}

LRESULT CMainFrame::OnPluginEcho(WPARAM wParam, LPARAM lParam)
{

	m_wndDocSelector.DeliverMessage((DWORD)wParam, WM_ECHOTEXT, 0, lParam);
	return 0;
}

LRESULT CMainFrame::OnPluginSysEcho(WPARAM wParam, LPARAM lParam)
{

	m_wndDocSelector.DeliverMessage((DWORD)wParam, WM_ECHOSYSTEXT, 0, lParam);
	return 0;
}

void CMainFrame::ActivateFrame(int nCmdShow)
{

	TRACE("Activate Frame %d\n", nCmdShow);

	CMDIFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::OnSystrayRestore()
{

	ShowWindow(SW_RESTORE);
}

void CMainFrame::CheckUpdate(void)
{

	CInternetSession	is;
	CString				strTmp, strNewVersion;
	
	try{

		CHttpFile* pFile = (CHttpFile*) is.OpenURL("http://mxcontrol.sourceforge.net/update/robomx.info");
		//some ISPs interfear with a proxy to display adds when the first page is loaded
		//so we close and opem the file again
		pFile->Close();
		delete pFile;
		pFile = 0;
		pFile = (CHttpFile*) is.OpenURL("http://mxcontrol.sourceforge.net/update/robomx.info");
		
		if(pFile != NULL){

			pFile->ReadString(strNewVersion);
			pFile->Close();
			delete pFile;
		}

		is.Close();
	}
	catch(CInternetException* pEx){
		
		TCHAR   szCause[255];
		CString strFormatted;

		pEx->GetErrorMessage(szCause, 255);
		strFormatted.Format("Error during Update Query: %s\n", szCause);
		TRACE(strFormatted);
		return;
	}

	CString strOldVersion = STR_VERSION_DLG;
	strOldVersion.Replace("\n", " ");
	strNewVersion.Replace("\\n", " ");

	if(strNewVersion != strOldVersion){

		strTmp.Format(IDS_UPDATE_AVAILABLE, strOldVersion, strNewVersion);
		DisplayToolTip(strTmp, 30);
	}
}

void CMainFrame::ExecuteAutoJoins(void)
{

	CIni ini;
	int n = 0;
	CString strTmp, strRoom;
	
	ini.SetIniFileName(g_sSettings.GetWorkingDir(FALSE) + "\\RoboMX.ini");
	n = ini.GetValue("AutoJoins", "Num", 0);

	for(int i = 1; i < n+1; i++){

		strTmp.Format("AutoJoin_%03d", i);
		strRoom = ini.GetValue("AutoJoins", strTmp, "");
		if(!strRoom.IsEmpty()){


			m_strRoom = strRoom;
			m_bQuickJoin = TRUE;
			OnFileNew();
			Sleep(100);
		}	
	}
}

void CMainFrame::OnReconnectAll()
{

	m_wndDocSelector.BroadcastMessage(WM_COMMAND, MAKEWPARAM(ID_RECONNECT, 0), 0);
}

void CMainFrame::SetLanguage(void)
{

	CString strBuffer, strLanguage = g_sSettings.GetLanguage();

	LCID		lcid			= NULL;

	if(!m_hInstDefault){

		m_hInstDefault = AfxGetInstanceHandle();
	}

	if(strLanguage.CompareNoCase("English") != 0){

		CString strDLL;
		strDLL.Format("%s\\lang_%s.dll", g_sSettings.GetWorkingDir(), strLanguage);

		m_hInstUse = ::LoadLibrary(strDLL);
	}

	if(!m_hInstUse){

		// there has been an error or english is selected
		m_hInstUse = m_hInstDefault;
	}
	
	AfxSetResourceHandle(m_hInstUse);

	CMenu *pMenuCurrent = GetMenu();
	m_pMenuNew = new CMenu;

		// has the menu changed?
		// m_hMenuDefault is the default menu resource for this frame see AFXWIN.H
	if(pMenuCurrent->m_hMenu != m_hMenuDefault)
		{
			// Destroy the "New" menu and delete the resource
			// We, after all created it!
		pMenuCurrent->DestroyMenu();
		delete pMenuCurrent;
		}

		// Load our new resource menu
	m_pMenuNew->LoadMenu(IDR_MAINFRAME);
		// Display the new menu
	SetMenu(m_pMenuNew);

		// Update any other text strings that are displayed
		// e.g. The Status bar
	strBuffer.LoadString(AFX_IDS_IDLEMESSAGE);
	m_wndStatusBar.SetPaneText(0, strBuffer);

		// We need to change the accelerators
		// m_hAccelTable is used in Winfrm.cpp as the accelerator handle,
		// only one accelerator can be loaded at a time so we MUST clear it
	m_hAccelTable = NULL;		

	if(!LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME))){

		CString strError;
		strError.Format("LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME) %d", GetLastError());
		AfxMessageBox(strError, MB_ICONEXCLAMATION);
	}

	// Our view, force a repaint to update the text
	//m_wndView.Invalidate();
}

BOOL CMainFrame::CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu)
{

	if(m_hInstUse == NULL){

		return CMDIFrameWnd::CreateClient(lpCreateStruct, pWindowMenu);
	}

	ASSERT(m_hWnd != NULL);
	ASSERT(m_hWndMDIClient == NULL);
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		MDIS_ALLCHILDSTYLES;    // allow children to be created invisible
	DWORD dwExStyle = 0;
	// will be inset by the frame

	// special styles for 3d effect on Win4
	dwStyle &= ~WS_BORDER;
	dwExStyle = WS_EX_CLIENTEDGE;

	AfxSetResourceHandle(m_hInstUse);

	pWindowMenu->DestroyMenu();
	pWindowMenu->LoadMenu(IDR_CHANNEL);
	lpCreateStruct->hInstance = m_hInstUse;

	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu = pWindowMenu->GetSafeHmenu();
		// set hWindowMenu for MFC V1 backward compatibility
		// for MFC V2, window menu will be set in OnMDIActivate
	ccs.idFirstChild = AFX_IDM_FIRST_MDICHILD;

	if (lpCreateStruct->style & (WS_HSCROLL|WS_VSCROLL))
	{
		// parent MDIFrame's scroll styles move to the MDICLIENT
		dwStyle |= (lpCreateStruct->style & (WS_HSCROLL|WS_VSCROLL));

		// fast way to turn off the scrollbar bits (without a resize)
		ModifyStyle(WS_HSCROLL|WS_VSCROLL, 0, SWP_NOREDRAW|SWP_FRAMECHANGED);
	}

	// Create MDICLIENT control with special IDC
	if ((m_hWndMDIClient = ::CreateWindowEx(dwExStyle, _T("mdiclient"), NULL,
		dwStyle, 0, 0, 0, 0, m_hWnd, (HMENU)AFX_IDW_PANE_FIRST,
		m_hInstUse, (LPVOID)&ccs)) == NULL)
	{
		TRACE(traceAppMsg, 0, _T("Warning: CMDIFrameWnd::OnCreateClient: failed to create MDICLIENT.")
			_T(" GetLastError returns 0x%8.8X\n"), ::GetLastError());
		return FALSE;
	}
	// Move it to the top of z-order
	::BringWindowToTop(m_hWndMDIClient);

	return TRUE;
}

