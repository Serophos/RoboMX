/*
** Copyright (C) 2004 Thees Winkler
**  
4** This program is free software; you can redistribute it and/or modify
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

#if !defined(AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)

#include "stdafx.h"
#include "Metis3.h"
#include "commctrl.h"

#include "ChildFrm.h"
#include "MainFrm.h"
#include "Settings.h"
#include "SettingsDoc.h"
#include "SettingsView.h"
#include "ServerDoc.h"
#include "ServerView.h"
#include "ChannelDoc.h"
#include "ChannelView.h"
#include "Metis3Doc.h"
#include "Metis3View.h"
#include "Ini.h"
#include "aboutctrl.h"
#include ".\metis3.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SIDEBYSIDE_COMMONCONTROLS 1

// winapi section

// HBITMAP hBmp;



/////////////////////////////////////////////////////////////////////////////
// CMetis3App

BEGIN_MESSAGE_MAP(CMetis3App, CWinApp)
	//{{AFX_MSG_MAP(CMetis3App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetis3App construction

CSettings g_sSettings;

CMetis3App::CMetis3App() :
   m_bPicLoaded(FALSE)
{

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMetis3App object

CMetis3App theApp;

/////////////////////////////////////////////////////////////////////////////
// CMetis3App initialization


void CMetis3App::ApplyPic(void)
{

	if(m_bPicLoaded){

		m_iBgImage.Destroy();
		HWND hMain = m_pMainWnd->GetWindow(GW_CHILD)->GetSafeHwnd();
		SetWindowLong(hMain, GWL_WNDPROC, (LONG)m_pOldWndProc);
		m_bPicLoaded = FALSE;
		SendMessage(hMain, WM_ERASEBKGND, (WPARAM)::GetDC(hMain), 0);
	}

	if(g_sSettings.GetUseImage()){

		//hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), g_sSettings.GetImage(),IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
		//if (hBmp == NULL){
		if(m_iBgImage.Load(g_sSettings.GetImage()) == E_FAIL){

			AfxMessageBox(IDS_ERROR_BG_LOAD, MB_OK);
		}
		else{

			m_bPicLoaded = TRUE;
			HWND hMain = m_pMainWnd->GetWindow(GW_CHILD)->GetSafeHwnd();
			m_pOldWndProc = (WNDPROC)GetWindowLong(hMain, GWL_WNDPROC);
			SetWindowLong(hMain, GWL_WNDPROC, (LONG)NewWndProc);
			SendMessage(hMain, WM_ERASEBKGND, (WPARAM)::GetDC(hMain), 0);
		}
	}
}


BOOL CMetis3App::InitInstance()
{

	CWinApp::InitInstance();

	// check ok we may continue :-)
	
	AfxEnableControlContainer();

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_BAR_CLASSES|ICC_COOL_CLASSES|ICC_DATE_CLASSES|ICC_HOTKEY_CLASS|ICC_INTERNET_CLASSES|ICC_LISTVIEW_CLASSES|ICC_PAGESCROLLER_CLASS|ICC_PROGRESS_CLASS|ICC_TAB_CLASSES|ICC_TREEVIEW_CLASSES|ICC_UPDOWN_CLASS|ICC_USEREX_CLASSES|ICC_WIN95_CLASSES;  

	if(!InitCommonControlsEx(&icc)){

		AfxMessageBox(IDS_ERROR_COMCTRL, MB_ICONSTOP);
		return FALSE;
	}

	SetRegistryKey(_T("RoboMX"));
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	WSADATA wsaData;

	m_nWSA = WSAStartup(MAKEWORD(1, 1), &wsaData);
	
	if(m_nWSA){

		AfxMessageBox(IDS_ERROR_WINSOCK, MB_OK+MB_ICONSTOP);
		return FALSE;
	}

	if(	LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1){
		
		AfxMessageBox(IDS_ERROR_WINSOCK, MB_OK+MB_ICONSTOP);
		return FALSE;
	}

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	g_sSettings.Load();
	SetRandomAppTitle();

	m_gdiFont.CreateFont( -11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, _T("Tahoma") );

	CMultiDocTemplate* pListTemplate;
	pListTemplate = new CMultiDocTemplate(
		IDR_LIST,
		RUNTIME_CLASS(CChannelDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CChannelView));
	if (!pListTemplate)
		return FALSE;
	AddDocTemplate(pListTemplate);

	CMultiDocTemplate* pChannelTemplate;
	pChannelTemplate = new CMultiDocTemplate(
		IDR_CHANNEL,
		RUNTIME_CLASS(CMetis3Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMetis3View));
	if (!pChannelTemplate)
		return FALSE;
	AddDocTemplate(pChannelTemplate);

	CMultiDocTemplate* pSettingsTemplate;
	pSettingsTemplate = new CMultiDocTemplate(
		IDR_SETTINGS,
		RUNTIME_CLASS(CSettingsDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSettingsView));
	if (!pSettingsTemplate)
		return FALSE;
	AddDocTemplate(pSettingsTemplate);

	CMultiDocTemplate* pServerTemplate;
	pServerTemplate = new CMultiDocTemplate(
		IDR_SERVER,
		RUNTIME_CLASS(CServerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CServerView));
	if (!pServerTemplate)
		return FALSE;
	AddDocTemplate(pServerTemplate);	 


	// create main MDI Frame window
	CMDIFrameWnd* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	((CMainFrame*)pMainFrame)->SetLanguage();
	((CMainFrame*)pMainFrame)->LoadPlugins();

#ifndef _DEBUG
	if(g_sSettings.GetUpdate()){

		((CMainFrame*)pMainFrame)->CheckUpdate();
	}
#endif

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// Background Image :-)
	ApplyPic();
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	if(g_sSettings.GetAutoList()){

		((CMainFrame*)pMainFrame)->OnChannelChannellist();
	}
	
	if(GetKeyState(VK_SHIFT) >= 0){

		((CMainFrame*)pMainFrame)->ExecuteAutoJoins();
	}
	return TRUE;
}

LRESULT CALLBACK CMetis3App::NewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	RECT rect;

	switch (uMsg)	{
	case WM_SIZE :
		SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)GetDC(hwnd), 0);
        return CallWindowProc(theApp.m_pOldWndProc, hwnd, uMsg, wParam, lParam);
	case WM_ERASEBKGND :

		SetStretchBltMode(hdc, HALFTONE);
		GetClientRect(hwnd, &rect);
		theApp.m_iBgImage.Draw(hdc, rect);

		return 1;
	default :
		return CallWindowProc(theApp.m_pOldWndProc, hwnd, uMsg, wParam, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CAboutCtrl m_ctrlAbout;
protected:
	virtual BOOL OnInitDialog(void);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ABOUTCTRL, m_ctrlAbout);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMetis3App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMetis3App message handlers

void CMetis3App::SetRandomAppTitle()
{

	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir() + "\\RoboMX.ini");

	CString strNum;

	int nNum = 0, nRand = 0;
	srand(GetTickCount());

	nNum = ini.GetValue("Title", "NumVendors", 1);
	if(nNum <= 0) nNum = 1;

	nRand = rand() % nNum;
	strNum.Format("Vendor_%02d", nRand);

	m_strAppTitle = ini.GetValue("Title", strNum, "BendSoft");

	nNum = ini.GetValue("Title", "NumNames", 1);
	if(nNum <= 0) nNum = 1;

	nRand = rand() % nNum;
	strNum.Format("Name_%02d", nRand);

	m_strAppTitle += " " + ini.GetValue("Title", strNum, "Robo");

	nNum = ini.GetValue("Title", "NumSuffix", 1);
	if(nNum <= 0) nNum = 1;

	nRand = rand() % nNum;
	strNum.Format("Suffix_%02d", nRand);

	m_strAppTitle += ini.GetValue("Title", strNum, "MX");

	free((void*)m_pszAppName);
	//strcpy((char*)m_pszAppName, strTitle);
	m_pszAppName = _tcsdup(m_strAppTitle);

	// RoboMX [:]\n\nRoboMX [:]\n\n\nRoboMX.Document\nRoboMX Document
}

BOOL CAboutDlg::OnInitDialog(void)
{

	CDialog::OnInitDialog();

	return TRUE;
}
