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

#include "ChannelDoc.h"
#include "ChannelView.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "Metis3Doc.h"
#include "Metis3View.h"
#include "RichEditExCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SIDEBYSIDE_COMMONCONTROLS 1

// winapi section

HBITMAP hBmp;

// old wnd proc
WNDPROC pfnOldWndProc;

// new one

LRESULT CALLBACK pfnNewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HDC hcompdc;
	RECT rect;

	switch (uMsg)	{
	case WM_SIZE :
		SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)GetDC(hwnd), 0);
		return 1;
	case WM_ERASEBKGND :
		//CallWindowProc(pfnOldWndProc, hwnd, uMsg, wParam, lParam);
		hcompdc = CreateCompatibleDC(hdc);
		SelectObject(hcompdc, hBmp);
		GetClientRect(hwnd, &rect);

		// bitmap dimensions are static for better performance (I assume
		// background image is statically loaded)
		SetStretchBltMode(hdc, HALFTONE);
		if (0 == StretchBlt(hdc, rect.left, rect.top, rect.right, rect.bottom,
						hcompdc, 0, 0, 800, 600, SRCCOPY))
					MessageBox(hwnd, "error", "while StretchBlt", MB_OK);

		DeleteDC(hcompdc);
		return 1;
	default :
		return CallWindowProc(pfnOldWndProc, hwnd, uMsg, wParam, lParam);
	}
}

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

CMetis3App::CMetis3App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMetis3App object

CMetis3App theApp;

/////////////////////////////////////////////////////////////////////////////
// CMetis3App initialization

BOOL CMetis3App::InitInstance()
{

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	WSADATA wsaData;

	m_nWSA = WSAStartup(0x101, &wsaData);
	
	if(m_nWSA){

		AfxMessageBox("Winsock initialisation failed.", MB_OK+MB_ICONSTOP);
		return FALSE;
	}

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.


	CMultiDocTemplate* pListTemplate;
	pListTemplate = new CMultiDocTemplate(
		IDR_LIST,
		RUNTIME_CLASS(CChannelDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CChannelView));
	AddDocTemplate(pListTemplate);

	CMultiDocTemplate* pChannelTemplate;
	pChannelTemplate = new CMultiDocTemplate(
		IDR_CHANNEL,
		RUNTIME_CLASS(CMetis3Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMetis3View));
	AddDocTemplate(pChannelTemplate);

	CMultiDocTemplate* pServerTemplate;
	pServerTemplate = new CMultiDocTemplate(
		IDR_SERVER,
		RUNTIME_CLASS(CMetis3Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMetis3View));
	AddDocTemplate(pServerTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// Background Image :-)

	hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BG),IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR);
	if (hBmp == NULL)
		AfxMessageBox("Bitmap could not be loaded !!!", MB_OK);

	HWND hMain = pMainFrame->GetWindow(GW_CHILD)->GetSafeHwnd();
	pfnOldWndProc = (WNDPROC)GetWindowLong(hMain, GWL_WNDPROC);
	SetWindowLong(hMain, GWL_WNDPROC, (long)pfnNewWndProc);

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
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
