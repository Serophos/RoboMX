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

#include "winamp.h"
#include "MainFrm.h"
#include "Metis3Doc.h"
#include "Metis3View.h"
#include "RenameDlg.h"
#include "Clipboard.h"
#include "SystemInfo.h"
#include "Settings.h"
#include "Tokenizer.h"
#include "SfxCfg.h"
#include "ini.h"
#include "InputRequest.h"
#include "RoboEx.h"
#include <mmsystem.h>
#include "RoboEx.h"
#include "Util.h"
#include ".\metis3view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TIMER_CONNECT 10334
#define TIMER_PING    10035
//#define TIMER_UPDATE  10036

extern UINT UWM_INPUT; // = ::RegisterWindowMessage("UWM_INPUT-7A14F66B-ABAB-4525-AC01-841C82EC48B6");
extern UINT UWM_MESSAGE;
extern UINT UWM_ACTION;
extern UINT UWM_TOPIC;
extern UINT UWM_MOTD;
extern UINT UWM_JOIN;
extern UINT UWM_PART;
extern UINT UWM_RENAME;
extern UINT UWM_ADDUSER;
extern UINT UWM_UNKNOWN;
extern UINT UWM_SYSTEM;
extern UINT UWM_REDIRECT;
extern UINT UWM_LOAD_SETTINGS;
extern UINT UWM_RCLICK;
extern UINT UWM_ROOMRENAME;
extern UINT UWM_SERVERTYPE;
extern UINT UWM_RENOTIFY;

#define WM_ECHOTEXT WM_APP+1
#define WM_ECHOSYSTEXT WM_APP+2

extern CSettings g_sSettings;

extern CPtrArray g_aPlugins;
#define PLUGIN ((CRoboEx*)g_aPlugins[i])

extern CArray<SOUND, SOUND> g_aSounds;
extern CStringArray g_aRCMSCommands;
extern CStringArray g_aWinMXCommands;

CStringArray g_aIgnored;
CSystemInfo  g_sSystem;
CStringArray g_aRooms;
CStringArray g_aGreetings;

UINT UWM_RENAMECL = ::RegisterWindowMessage("UWM_RENAMECL-{494D99C1-03BE-49e3-8A47-D0D17C6D4ACE}");

/////////////////////////////////////////////////////////////////////////////
// CMetis3View

IMPLEMENT_DYNCREATE(CMetis3View, CFormView)

BEGIN_MESSAGE_MAP(CMetis3View, CFormView)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LEAVE, OnLeave)
	ON_NOTIFY(NM_RCLICK, IDC_USERLIST, OnRclickUserlist)
	ON_COMMAND(ID_USERLIST_SENDMESSAGE, OnUserlistSendmessage)
	ON_COMMAND(ID_USERLIST_REMOVEADMIN, OnUserlistRemoveadmin)
	ON_COMMAND(ID_USERLIST_REDIRECT, OnUserlistRedirect)
	ON_COMMAND(ID_USERLIST_PRINTUSERSTAT, OnUserlistPrintuserstat)
	ON_COMMAND(ID_USERLIST_PRINTIP, OnUserlistPrintip)
	ON_COMMAND(ID_USERLIST_UNBAN, OnUserlistUnban)
	ON_COMMAND(ID_USERLIST_KICKBAN, OnUserlistKickban)
	ON_COMMAND(ID_USERLIST_KICK, OnUserlistKick)
	ON_COMMAND(ID_USERLIST_COPYUSERNAME, OnUserlistCopyusername)
	ON_COMMAND(ID_USERLIST_BAN, OnUserlistBan)
	ON_COMMAND(ID_USERLIST_ADDADMIN, OnUserlistAddadmin)
	ON_COMMAND(ID_USERLIST_READUSERMESSAGE, OnUserlistReadusermessage)
	ON_COMMAND(ID_USERLIST_CUSTOMIZETHISMENU, OnUserlistCustomizethismenu)
	ON_COMMAND(ID_USERLIST_IGNORE, OnUserlistIgnore)
	ON_COMMAND(ID_USERLIST_UNIGNORE, OnUserlistUnignore)
	ON_COMMAND(ID_RECONNECT, OnReconnect)
	ON_UPDATE_COMMAND_UI(ID_RECONNECT, OnUpdateReconnect)
	ON_UPDATE_COMMAND_UI(ID_CHATROOM_DISPLAYINCHANNEL_WINAMPSONG, OnUpdateWinampsongMenu)
	ON_COMMAND(ID_CHATROOM_SELECTALL, OnChatroomSelectall)
	ON_COMMAND(ID_CHATROOM_SEARCHONGOOGLE, OnChatroomSearchongoogle)
	ON_COMMAND(ID_CHATROOM_QUOTE, OnChatroomQuote)
	ON_COMMAND(ID_CHATROOM_COPY, OnChatroomCopy)
	ON_COMMAND(ID_CHATROOM_CLEARSCREEN, OnChatroomClearscreen)
	ON_COMMAND(ID_CHATROOM_COPYROOMNAME, OnChatroomCopyroomname)
	ON_COMMAND(ID_CHATROOM_DISPLAYINCHANNEL_WINAMPSONG, OnDisplayWinampsong)
	ON_COMMAND(ID_CHATROOM_DISPLAYINCHANNEL_SYSTEMUPTIME, OnDisplaySystemuptime)
	ON_COMMAND(ID_CHATROOM_DISPLAYINCHANNEL_SYSTEMINFO, OnDisplaySysteminfo)
	ON_COMMAND(ID_CHATROOM_DISPLAYINCHANNEL_AVERAGELAG, OnDisplayAveragelag)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_CHATROOM_TEXTTRICKS_BUBBLES, OnChatroomTexttricksBubbles)
	ON_COMMAND(ID_CHATROOM_TEXTTRICKS_BOX, OnChatroomTexttricksBox)
	ON_COMMAND(ID_CHATROOM_TEXTTRICKS_HACKER, OnChatroomTexttricksHacker)
	ON_COMMAND(ID_CHATROOM_ASCIIARTDESIGN, OnChatroomAsciiartdesign)
	ON_COMMAND(ID_CHAT_TEXTTRICKS_3DBUTTONSNORMAL, OnChatTexttricks3dbuttonsnormal)
	ON_COMMAND(ID_CHAT_TEXTTRICKS_3DBUTTONSACTION, OnChatTexttricks3dbuttonsaction)
	ON_UPDATE_COMMAND_UI_RANGE(ID_USERLIST_SENDMESSAGE,ID_USERLIST_READUSERMESSAGE, OnUpdateUserlistMenu)
	ON_REGISTERED_MESSAGE(UWM_INPUT, OnInput)
	ON_REGISTERED_MESSAGE(UWM_MESSAGE, OnMessage)
	ON_REGISTERED_MESSAGE(UWM_ACTION, OnAction)
	ON_REGISTERED_MESSAGE(UWM_TOPIC, OnTopic)
	ON_REGISTERED_MESSAGE(UWM_MOTD, OnMotd)
	ON_REGISTERED_MESSAGE(UWM_JOIN, OnJoin)
	ON_REGISTERED_MESSAGE(UWM_PART, OnPart)
	ON_REGISTERED_MESSAGE(UWM_RENAME, OnRenameMsg)
	ON_REGISTERED_MESSAGE(UWM_ADDUSER, OnAddUser)
	ON_REGISTERED_MESSAGE(UWM_UNKNOWN, OnUnknown)
	ON_REGISTERED_MESSAGE(UWM_SYSTEM, OnSystem)
	ON_REGISTERED_MESSAGE(UWM_REDIRECT, OnRedirect)
	ON_REGISTERED_MESSAGE(UWM_LOAD_SETTINGS, OnReloadCfg)
	ON_REGISTERED_MESSAGE(UWM_RCLICK, OnRclickChat)
	ON_NOTIFY(EN_LINK, IDC_CHAT, OnEnLinkChat)
	ON_MESSAGE(WM_ECHOTEXT, OnEchoText)
	ON_MESSAGE(WM_ECHOSYSTEXT, OnEchoSysText)
	ON_REGISTERED_MESSAGE(UWM_ROOMRENAME, OnRoomRename)
	ON_REGISTERED_MESSAGE(UWM_SERVERTYPE, OnSetServerType)
	ON_REGISTERED_MESSAGE(UWM_RENOTIFY, OnRenNotify)
	ON_REGISTERED_MESSAGE(UWM_RENAMECL, OnRenameCl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetis3View construction/destruction

CMetis3View::CMetis3View()
	: CFormView(CMetis3View::IDD)
{
	//{{AFX_DATA_INIT(CMetis3View)
	m_strInput = _T("");
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_pStatusBar = 0;
	m_dwAvLag = 0;
	m_dwLastTic = 0;
	m_bHasJoined = FALSE;
	m_nServerType = SERVER_RCMS;
}

CMetis3View::~CMetis3View()
{

	m_iImageList.DeleteImageList();
}

void CMetis3View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetis3View)
	DDX_Control(pDX, IDC_LEAVE, m_btExit);
	DDX_Control(pDX, IDC_INPUT, m_eInput);
	DDX_Control(pDX, IDC_USERLIST, m_lcUsers);
	DDX_Text(pDX, IDC_INPUT, m_strInput);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SYS, m_rSys);
	DDX_Control(pDX, IDC_CHAT, m_rChat);
}

BOOL CMetis3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CFormView::PreCreateWindow(cs);
}

void CMetis3View::OnInitialUpdate()
{
	
	CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();

	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.AddButton( this, IDR_CHANNEL );
	m_pStatusBar = (CColorStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	
	LoadRCMSMenu();
	CRect rc;
	CWnd* pWnd = GetDlgItem(IDC_SPLITTER_1);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_sSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER_1);
	m_sSplitter1.SetStyle(SPS_VERTICAL);
	m_sSplitter1.SetRange(200, 200, 1);
	
	pWnd = GetDlgItem(IDC_SPLITTER_2);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_sSplitter2.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER_2);
	m_sSplitter2.SetStyle(SPS_HORIZONTAL);
	m_sSplitter2.SetRange(150, 150, 1);

	m_rSys.Init();
	m_rSys.SetBackgroundColor(FALSE, g_sSettings.GetRGBBg());
	m_rChat.Init();
	m_rChat.SetBackgroundColor(FALSE, g_sSettings.GetRGBBg());
	m_rChat.SendMessage(EM_AUTOURLDETECT, TRUE, 0);


	m_fFont.CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_STRING_PRECIS, 
		CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_DONTCARE, "Arial");
	ListView_SetExtendedListViewStyle(m_lcUsers.m_hWnd, LVS_EX_FULLROWSELECT);

	CBitmap		bitmap;
	bitmap.LoadMappedBitmap(IDB_USERMODE);
	m_iImageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 4, 1);
	m_iImageList.Add(&bitmap, RGB(255,0,255));
	
	m_lcUsers.SetHeadings("User,136;Share,60;Speed,80;Node-IP,120;Node-Port,80;IP (Admin/Op),120");
	m_lcUsers.LoadColumnInfo();
	
	m_lcUsers.SetColors(
		g_sSettings.GetRGBNormalMsg(),
		g_sSettings.GetRGBFiles(),
		g_sSettings.GetRGBLine(),
		g_sSettings.GetRGBIP(),
		g_sSettings.GetRGBPort()
		);
	m_lcUsers.SetBkColor(g_sSettings.GetRGBBg());

	m_lcUsers.SetColors(
		g_sSettings.GetRGBNormalMsg(),
		g_sSettings.GetRGBFiles(),
		g_sSettings.GetRGBLine(),
		g_sSettings.GetRGBIP(),
		g_sSettings.GetRGBPort()
		);

	if(g_sSettings.GetHiliteUsers()){

		m_lcUsers.SetHiLite();
	}
	m_lcUsers.SetImageList(&m_iImageList, LVSIL_SMALL);
	m_lcUsers.SetFont(&m_fFont);
	m_eInput.SetFont(&m_fFont);
	m_eInput.SetBkColor(g_sSettings.GetRGBBg());
	m_eInput.SetCommands(&g_aRCMSCommands);

	// Set up client
	m_sSplitter1.SetRange(150, 50, -1);

	if(g_sSettings.GetMaxi()){

		ReCalcLayout();
	}
	else{

		ResizeParentToFit(FALSE);
	}


	m_mxClient.m_pView		= this;
	m_mxClient.m_dwFiles	= GetDocument()->m_dwFiles;
	m_mxClient.m_wLineType	= GetDocument()->m_wLine;
	m_mxClient.m_strUser	= GetDocument()->m_strName;

	m_mxClient.SetRoom(GetDocument()->m_strRoom);
	WriteSystemMsg("Connecting. Stand by...", g_sSettings.GetRGBPend());

	OnUpdate(this, 0, NULL);
	
	SetTimer(TIMER_CONNECT, 500, 0);
}


LRESULT CMetis3View::OnReloadCfg(WPARAM w, LPARAM l)
{

	m_lcUsers.SetColors(
		g_sSettings.GetRGBNormalMsg(),
		g_sSettings.GetRGBFiles(),
		g_sSettings.GetRGBLine(),
		g_sSettings.GetRGBIP(),
		g_sSettings.GetRGBPort()
		);
	m_lcUsers.SetBkColor(g_sSettings.GetRGBBg());
	if(g_sSettings.GetHiliteUsers()){

		m_lcUsers.SetHiLite();
	}
	else{

		m_lcUsers.SetHiLite(FALSE);
	}
	m_eInput.SetBkColor(g_sSettings.GetRGBBg());
	m_rSys.Init();
	m_rSys.SetBackgroundColor(FALSE, g_sSettings.GetRGBBg());
	m_rChat.Init();
	m_rChat.SetBackgroundColor(FALSE, g_sSettings.GetRGBBg());
	Invalidate();

	return 1;
}

BOOL CMetis3View::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{

	if (wParam == IDC_SPLITTER_1){	

		SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
		DoResize1(pHdr->delta);
	}	
	else if (wParam == IDC_SPLITTER_2){	

		SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
		DoResize2(pHdr->delta);
	}	
	return CFormView::OnNotify(wParam, lParam, pResult);
}

void CMetis3View::DoResize1(int delta)
{

	CSplitterControl::ChangeWidth(&m_lcUsers, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_rSys, delta, CW_LEFTALIGN);
	CSplitterControl::ChangeWidth(&m_rChat, delta, CW_LEFTALIGN);
	CSplitterControl::ChangeWidth(&m_sSplitter2, delta, CW_LEFTALIGN);
	Invalidate();
	UpdateWindow();
}

void CMetis3View::DoResize2(int delta)
{

	CSplitterControl::ChangeHeight(&m_rSys, delta, CW_TOPALIGN);
	CSplitterControl::ChangeHeight(&m_rChat, -delta, CW_BOTTOMALIGN);
//	CSplitterControl::ChangeHeight(&m_eInput, 0);
	Invalidate();
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMetis3View diagnostics

#ifdef _DEBUG
void CMetis3View::AssertValid() const
{
	CFormView::AssertValid();
}

void CMetis3View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMetis3Doc* CMetis3View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMetis3Doc)));
	return (CMetis3Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMetis3View message handlers


void CMetis3View::OnDestroy() 
{
	
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.RemoveButton(this);
	m_lcUsers.SaveColumnInfo();

	for(int i = 0; i < g_aPlugins.GetSize() && m_bHasJoined; i++){

		PLUGIN->OnLeaveChannel((DWORD)m_hWnd);
	}

	KillTimer(TIMER_PING);
	m_mxClient.Disconnect();

	if(g_sSettings.GetSoundFX() && m_bHasJoined){

		PlaySound(g_sSettings.GetSfxPart(), NULL, SND_FILENAME|SND_ASYNC);
	}

	CFormView::OnDestroy();
}

void CMetis3View::OnTimer(UINT nIDEvent) 
{

	if(nIDEvent == TIMER_CONNECT){

		if(!m_mxClient.Connect()){

			KillTimer(TIMER_CONNECT);
			return;	
		}
		KillTimer(TIMER_CONNECT);
	
		SetTimer(TIMER_PING, 5*60*1000, 0);
	}
	else if(nIDEvent == TIMER_PING){

		m_mxClient.Ping();
	}
	CFormView::OnTimer(nIDEvent);
}

void CMetis3View::WriteSystemMsg(CString strMsg, COLORREF crText)
{

	strMsg.Replace("\\rtf", "#rtf");

	CString strTime;
	strTime.Format("[%s]", GetMyLocalTime());
	
	m_rSys.SetText(strTime, g_sSettings.GetRGBTime(), crText);
	m_rSys.SetText(" " + strMsg + "\n", crText, g_sSettings.GetRGBBg());

	if(crText == g_sSettings.GetRGBErr() && g_sSettings.GetSoundFX()){

		PlaySound(g_sSettings.GetSfxError(), NULL, SND_FILENAME|SND_ASYNC);
	}
}

void CMetis3View::RemoveUser(const CString strUser, const CString strIP, WORD wPort)
{

	LVFINDINFO fi;
	fi.flags = LVFI_STRING|LVFI_WRAP;
	fi.psz = strUser;

	int nPos = m_lcUsers.FindItem(&fi, -1);

	while(nPos >= 0){

		if((m_lcUsers.GetItemText(nPos, 0) == strUser) &&
			(m_lcUsers.GetItemText(nPos, 4) == Util::FormatInt(wPort)) &&
			(m_lcUsers.GetItemText(nPos, 3) == strIP)){


			m_lcUsers.DeleteItem(nPos);
			m_lcUsers.Sort();
			break;
		}

	}
}

void CMetis3View::AddUser(CString strUsername, WORD wLine, DWORD dwFiles, CString strNodeIP, WORD wNodePort, CString strIP, WORD wUserLevel)
{

	CString strFiles, strLine, strPort;
	strFiles.Format("%d", dwFiles);
	strLine = Util::FormatLine(wLine);
	strPort.Format("%d", wNodePort);

	m_lcUsers.AddItem(wUserLevel, strUsername, (LPCTSTR)strFiles, (LPCTSTR)strLine, (LPCTSTR)strNodeIP, (LPCTSTR)strPort, (LPCTSTR)strIP);
	m_lcUsers.Sort();
}


LRESULT CMetis3View::OnRoomRename(WPARAM wParam, LPARAM lParam)
{

	CString strRoom = (LPCSTR)lParam;
	CString strOldRoom = GetDocument()->m_strRoom;
	CString strOut;

	GetDocument()->SetTitle(strRoom);
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->m_strRoom);

	strOut.Format("Roomname changed from %s to %s", strOldRoom, GetDocument()->m_strRoom);
	
	WriteSystemMsg(strOut, g_sSettings.GetRGBOK());
	
	return 1;
}

LRESULT CMetis3View::OnTopic(WPARAM wParam, LPARAM lParam)
{

	CString strTopic = (LPCSTR)lParam;

	strTopic.Replace("\\rtf", "#rtf");

	if(g_sSettings.GetPrintTime()){

		CString strTime;
		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBTopic());
	}

	m_rChat.SetText(" Topic: " + strTopic + "\n", g_sSettings.GetRGBTopic(), g_sSettings.GetRGBBg());

	if(g_sSettings.GetSoundFX() && m_bHasJoined){

		PlaySound(g_sSettings.GetSfxTopic(), NULL, SND_FILENAME|SND_ASYNC);
	}
	return 1;
}

LRESULT CMetis3View::OnMotd(WPARAM wParam, LPARAM lParam)
{

	CString strMotd = (LPCSTR)lParam;
	strMotd.Replace("\\rtf", "#rtf");

	if(g_sSettings.GetPrintTime()){

		CString strTime;
		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBMotd());
	}

	m_rChat.SetText(" " + strMotd + "\n", g_sSettings.GetRGBMotd(), g_sSettings.GetRGBBg());

	if(g_sSettings.GetSoundFX() && m_bHasJoined){

		PlaySound(g_sSettings.GetSfxMotd(), NULL, SND_FILENAME|SND_ASYNC);
	}
	return 1;
}

LRESULT CMetis3View::OnAddUser(WPARAM wParam, LPARAM lParam)
{

	char* pUserInfo = (char*)lParam;
	WORD  wType		= LOWORD(wParam);
	WORD  wLen		= HIWORD(wParam);

	//Userlist-Notification (Server)
    // 0x006F][Username:N][00:1][IP-Address:4][UDP-Port:2][Line-Type:2][Shared-Files:4]

	MX_USERINFO user;
	
	DWORD dwIP = 0, dwRealIP = 0;
	LPSTR lpUser;
	CString strUser;

	if(wType == 0x072){ // winmx 3.52 and later

		if(Util::ScanMessage(pUserInfo, wLen, "SDWWDW", &lpUser, &dwIP, &user.wNodePort, &user.wLineType, &user.dwNumFiles, &user.wUserLever, &dwRealIP) < 6){

			TRACE("Error decoding join notification packet\n");
			return 0;
		}
	}
	else{

		if(Util::ScanMessage(pUserInfo, wLen, "SDWWD", &lpUser, &dwIP, &user.wNodePort, &user.wLineType, &user.dwNumFiles) != 5){

			TRACE("Error decoding join notification packet\n");
			return 0;
		}
		user.wUserLever = 0;
	}
	
	strUser = lpUser;
	user.strUser = strUser;
	user.strNodeIP = Util::FormatIP(dwIP);
	user.strRealIP = Util::FormatIP(dwRealIP);

	m_aUsers.Add(user);
	
	AddUser(strUser, user.wLineType, user.dwNumFiles, user.strNodeIP, user.wNodePort, user.strRealIP, user.wUserLever);

	return 1;
}

LRESULT CMetis3View::OnJoin(WPARAM wParam, LPARAM lParam)
{

	char* pUserInfo = (char*)lParam;
	WORD  wType		= LOWORD(wParam);
	WORD  wLen		= HIWORD(wParam);

	MX_USERINFO user;
	//User-Join notification (Server)
	//0x006E][Username:N][00:1][IP-Address:4][UDP-Port:2][Line-Type:2][Shared-Files:4]
	DWORD dwIP = 0, dwRealIP = 0;
	LPSTR lpUser;
	CString strUser;
	
	if(wType == 0x071){ // winmx 3.52 and later (no-ip display)

		BYTE btDummy;
		if(Util::ScanMessage(pUserInfo, wLen, "SDWWDB", &lpUser, &dwIP, &user.wNodePort, &user.wLineType, &user.dwNumFiles, &btDummy) < 6){

			TRACE("Error decoding join notification packet\n");
			return 0;
		}
		user.wUserLever = (WORD)btDummy;
	}
	else if(wType == 0x075){ // winmx 3.52 and later (ip display)

		BYTE btDummy;
		if(Util::ScanMessage(pUserInfo, wLen, "SDWWDBD", &lpUser, &dwIP, &user.wNodePort, &user.wLineType, &user.dwNumFiles, &btDummy, &dwRealIP) < 6){

			TRACE("Error decoding join notification packet\n");
			return 0;
		}
		user.wUserLever = (WORD)btDummy;
	}
	else{  // old winmx

		if(Util::ScanMessage(pUserInfo, wLen, "SDWWD", &lpUser, &dwIP, &user.wNodePort, &user.wLineType, &user.dwNumFiles) != 5){

			TRACE("Error decoding join notification packet\n");
			return 0;
		}

		user.wUserLever = 0;
	}
	
	strUser = lpUser;
	user.strUser = strUser;
	user.strNodeIP = Util::FormatIP(dwIP);
	user.strRealIP = Util::FormatIP(dwRealIP);

	m_aUsers.Add(user);

	strUser.Replace("\\rtf", "#rtf");
	
	CString strJoin;

	if(g_sSettings.GetPrintTime()){

		CString strTime;
		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBJoin());
	}

	strJoin = g_sSettings.GetJoin();
	strJoin.Replace("%NAME%", strUser);
	strJoin.Replace("%LINE%", Util::FormatLine(user.wLineType));
	strJoin.Replace("%FILES%", Util::FormatInt(user.dwNumFiles));
	strJoin.Replace("%IP%", user.strRealIP);

	m_rChat.SetText(" " + strJoin + "\n", g_sSettings.GetRGBJoin(), g_sSettings.GetRGBBg());

	AddUser(strUser, user.wLineType, user.dwNumFiles, user.strNodeIP, user.wNodePort, user.strRealIP, user.wUserLever);

	if(!m_bHasJoined){

		if(strUser == GetDocument()->m_strName){

			for(int i = 0; i < g_aPlugins.GetSize(); i++){

				PLUGIN->OnJoinChannel((DWORD)m_hWnd, GetDocument()->m_strRoom, &m_aUsers);
			}

			m_bHasJoined = TRUE;
			InputWelcome();
		}
	}

	for(int i = 0; i < g_aPlugins.GetSize(); i++){

		PLUGIN->OnJoin((DWORD)m_hWnd, &user);
	}
	OnUpdate(this, 0, 0);
	return 1;
}

LRESULT CMetis3View::OnRenameMsg(WPARAM wParam, LPARAM lParam)
{

	char* pRenameInfo = (char*)lParam;
	WORD  wType		= LOWORD(wParam);
	WORD  wLen		= HIWORD(wParam);

	//Rename notification (Server)
	//0x0070]
	//[Name:N][00:1][IP-Address:4][UDP-Port:2] // old
	//[Name:N][00:1][IP-Address:4][UDP-Port:2][Line-Type:2][Shared-Files:4] // new

	LPSTR   lpOldName = 0, lpNewName = 0;
	DWORD   dwOldIP = 0, dwNewIP = 0, dwFiles = 0, dwRealIP = 0;
	WORD    wOldPort = 0, wNewPort = 0, wLine = 0, wMode = 0;

	if(wType == 0x0074){

		if(Util::ScanMessage(pRenameInfo, wLen, "SDWSDWWDWD", 
									&lpOldName, &dwOldIP, &wOldPort,
									&lpNewName, &dwNewIP, &wNewPort, &wLine, &dwFiles, &wMode, &dwRealIP) < 9){

			TRACE("Error decoding rename notification packet\n");
			return 0;
		}
	}
	else{

		if(Util::ScanMessage(pRenameInfo, wLen, "SDWSDWWD", 
									&lpOldName, &dwOldIP, &wOldPort,
									&lpNewName, &dwNewIP, &wNewPort, &wLine, &dwFiles) != 8){

			TRACE("Error decoding rename notification packet\n");
			return 0;
		}
		wMode = 10;
	}
	//CString strOldName;
	//strOldName.Format("%s", lpOldName);

	LVFINDINFO fi;
	fi.flags = LVFI_STRING|LVFI_WRAP;
	fi.psz = lpOldName;

	m_lcUsers.Sort();
	int nPos = m_lcUsers.FindItem(&fi, -1);

	MX_USERINFO oldUser, newUser;

	if(nPos >= 0){

		// Find user...
		for(int i = 0; i < m_aUsers.GetSize(); i++){

			TRACE("%s %s\n", m_aUsers[i].strUser, lpOldName);
			if(m_aUsers[i].strUser.Compare(lpOldName) == 0) break;
		}
		if(i >= m_aUsers.GetSize()){

			CString strMsg;
			strMsg.Format("Error during User-Rename handling: User %s does not exist\n", lpOldName);
			WriteSystemMsg(strMsg, g_sSettings.GetRGBErr());
			return 0;
		}
		oldUser = m_aUsers[i];
		newUser = oldUser;

		newUser.strUser = lpNewName;
		newUser.wLineType = wLine;
		newUser.wNodePort = wNewPort;
		newUser.dwNumFiles = dwFiles;
		newUser.strNodeIP = Util::FormatIP(dwNewIP);
		newUser.wUserLever = (wMode != 10 ? wMode : oldUser.wUserLever);
		newUser.strRealIP = Util::FormatIP(dwRealIP);
		
		m_aUsers.SetAt(i, newUser);
		m_lcUsers.DeleteItem(nPos);
		m_lcUsers.Sort();
		m_lcUsers.AddItem(newUser.wUserLever, newUser.strUser, (LPCTSTR)Util::FormatInt(newUser.dwNumFiles), (LPCTSTR)Util::FormatLine(newUser.wLineType),
			(LPCTSTR)newUser.strNodeIP, (LPCTSTR)Util::FormatInt(newUser.wNodePort), (LPCSTR)newUser.strRealIP);
		//m_lcUsers.AddItem(wUserLevel, strUsername, (LPCTSTR)strFiles, (LPCTSTR)strLine, (LPCTSTR)strNodeIP, (LPCTSTR)strPort, (LPCTSTR)strIP);
		//m_lcUsers.SetItemState(nPos, &item);
		//m_lcUsers.SetItemText(nPos, 0, newUser.strUser); // name
		//m_lcUsers.SetItemText(nPos, 1, Util::FormatInt(newUser.dwNumFiles)); // files
		//m_lcUsers.SetItemText(nPos, 2, Util::FormatLine(newUser.wLineType)); // line
		//m_lcUsers.SetItemText(nPos, 3, newUser.strNodeIP); // nodeip
		//m_lcUsers.SetItemText(nPos, 4, Util::FormatInt(newUser.wNodePort)); // nodeport
		m_lcUsers.Sort();
	}
	else{

		CString strErr;
		strErr.Format("Debug Assertion Failed (call: OnRenameMsg): Could not locate '%s' in the userlist. Please report this error. CMyListCtrl::FindItem returned %d\n", lpOldName, nPos);
		WriteSystemMsg(strErr, g_sSettings.GetRGBErr());
		return 0;
	}
	for(int i = 0; i < g_aPlugins.GetSize(); i++){

		PLUGIN->OnRename((DWORD)m_hWnd, &oldUser, &newUser);
	}

	OnUpdate(this, 0, 0);
	return 1;
}


LRESULT CMetis3View::OnRenNotify(WPARAM wParam, LPARAM lParam)
{

	CString strMsg = (char*)lParam;
	
	strMsg.Replace("\\rtf", "#rtf");
	
	CString strPart, strTime;
	if(g_sSettings.GetPrintTime()){

		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBPend());
	}
	
	m_rChat.SetText(" " + strMsg + "\n", g_sSettings.GetRGBPend(), g_sSettings.GetRGBBg());
	
	return 1;
}

LRESULT CMetis3View::OnPart(WPARAM wParam, LPARAM lParam)
{

	//User-Part notification (Server)
	//0x0073][Username:N][00:1][IP-Address:4][UDP-Port:2]

	char* pPart = (char*)lParam;
	WORD  wLen  = (WORD)wParam;
	
	DWORD dwIP = 0;
	WORD wPort = 0;
	LPSTR lpUser = 0;

	if(Util::ScanMessage(pPart, wLen, "SDW", &lpUser, &dwIP, &wPort) != 3){

		TRACE("Error decoding Part notification packet\n");
		return 0;
	}

	CString strIP   = Util::FormatIP(dwIP);
	CString strUser = lpUser;
	MX_USERINFO user;

	for(int i = 0; i < m_aUsers.GetSize(); i++){

		if((m_aUsers[i].strUser == strUser) && 
			(m_aUsers[i].strNodeIP == strIP) &&
			 (m_aUsers[i].wNodePort == wPort)) break;
	}

	if(i < m_aUsers.GetSize()){

		user = m_aUsers[i];
		m_aUsers.RemoveAt(i);
		RemoveUser(strUser, strIP, wPort);
	}

	strUser.Replace("\\rtf", "#rtf");
	
	CString strPart, strTime;
	if(g_sSettings.GetPrintTime()){

		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBPart());
	}
	
	strPart = g_sSettings.GetPart();
	strPart.Replace("%NAME%", strUser);

	m_rChat.SetText(" " + strPart + "\n", g_sSettings.GetRGBPart(), g_sSettings.GetRGBBg());

	for(i = 0; i < g_aPlugins.GetSize(); i++){

		PLUGIN->OnPart((DWORD)m_hWnd, &user);
	}
	//OnUpdate(this, 0, 0);
	return 1;
}

LRESULT CMetis3View::OnMessage(WPARAM wParam, LPARAM lParam)
{

	//Normal Message (Server)
    //0x00C9][Name:N][Message:N]
	char* pMsg = (char*)lParam;
	WORD wLen  = (WORD)wParam;

	CString strMsg, strName;
	strName = pMsg;
	int nNameLen = strName.GetLength()+1;
	
	strMsg =  (pMsg+nNameLen);

	if(strMsg.IsEmpty() && strName.IsEmpty()) return 0;
	
	if(GetDocument()->m_strName.Find(strName, 0) == 0){

		UpdateAverageLag(FALSE);
	}
	
	for(int i = 0; i < g_aIgnored.GetSize(); i++){

		if(strName.Find(g_aIgnored[i], 0) == 0)
			return 1;
	}

	strName.Replace("\\rtf", "#rtf");
	strMsg.Replace("\\rtf", "#rtf");

	for(i = 0; i < g_aPlugins.GetSize(); i++){

		PLUGIN->OnMessage((DWORD)m_hWnd, &strName, &strMsg, FALSE);
	}
	if(strMsg.IsEmpty()) return 1;

	if(g_sSettings.GetPrintTime()){


		CString strTime;
		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBNormalName());
	}
	m_rChat.SetText(" " + g_sSettings.GetBrMsgFront(), g_sSettings.GetRGBBrMessage(), g_sSettings.GetRGBBg());
	m_rChat.SetText(strName, g_sSettings.GetRGBNormalName(), g_sSettings.GetRGBBg());
	m_rChat.SetText(g_sSettings.GetBrMsgEnd() + " ", g_sSettings.GetRGBBrMessage(), g_sSettings.GetRGBBg());
	m_rChat.SetText(strMsg + "\n", g_sSettings.GetRGBNormalMsg(), g_sSettings.GetRGBBg());
	HandleHiLite();

	if(g_sSettings.GetSfxChatSfx()){

		for(int i = 0; i < g_aSounds.GetSize(); i++){

			if(strMsg.Find(g_aSounds[i].strTrigger, 0) >= 0){

				PlaySound(g_aSounds[i].strSound, NULL, SND_FILENAME|SND_ASYNC);
				break;
			}
		}
	}
	return 1;
}

LRESULT CMetis3View::OnAction(WPARAM wParam, LPARAM lParam)
{

	//Action Message (Server)
	//0x00CB][Name:N][Message:N][00:1]
	char* pMsg = (char*)lParam;
	WORD wLen  = (WORD)wParam;

	CString strMsg, strName;

	strName = pMsg;
	int nNameLen = strName.GetLength()+1;
	
	strMsg =  (pMsg+nNameLen);

	if(strMsg.IsEmpty() && strName.IsEmpty()) return 0;

	if(GetDocument()->m_strName.Find(strName, 0) == 0){

		UpdateAverageLag(FALSE);
	}
	
	for(int i = 0; i < g_aIgnored.GetSize(); i++){

		if(strName.Find(g_aIgnored[i], 0) == 0)
			return 1;
	}


	strName.Replace("\\rtf", "#rtf");
	strMsg.Replace("\\rtf", "#rtf");

	for(i = 0; i < g_aPlugins.GetSize(); i++){

		PLUGIN->OnMessage((DWORD)m_hWnd, &strName, &strMsg, TRUE);
	}
	
	if(g_sSettings.GetPrintTime()){


		CString strTime;
		strTime.Format("[%s]", GetMyLocalTime());
		m_rChat.SetText(strTime, g_sSettings.GetRGBTime(), g_sSettings.GetRGBActionMsg());
	}
	m_rChat.SetText(" " + g_sSettings.GetBrActionFront(), g_sSettings.GetRGBBrAction(), g_sSettings.GetRGBBg());
	m_rChat.SetText(strName, (g_sSettings.GetColorAcName() ? g_sSettings.GetRGBNormalName() : g_sSettings.GetRGBActionMsg()), g_sSettings.GetRGBBg());
	m_rChat.SetText(g_sSettings.GetBrActionEnd() + " ", g_sSettings.GetRGBBrAction(), g_sSettings.GetRGBBg());
	m_rChat.SetText(strMsg + "\n", g_sSettings.GetRGBActionMsg(), g_sSettings.GetRGBBg());
	HandleHiLite();

	if(g_sSettings.GetSfxChatSfx()){

		for(int i = 0; i < g_aSounds.GetSize(); i++){

			if(strMsg.Find(g_aSounds[i].strTrigger, 0) >= 0){

				PlaySound(g_aSounds[i].strSound, NULL, SND_FILENAME|SND_ASYNC);
				break;
			}
		}
	}
	return 1;
}

LRESULT CMetis3View::OnUnknown(WPARAM wParam, LPARAM lParam)
{
	
	WORD wType = LOWORD(wParam);
	WORD wLen  = HIWORD(wParam);
	char* pCmd = (char*)lParam;

	CString strCmd, strOut;
	unsigned char c;
	for(int i = 0; i < wLen; i++)
	{

		c = (unsigned char)pCmd[i];
		if(c == '\0') strCmd+="0";
		else strCmd.Insert(i, c);
	}
	strOut.Format("Fixme: Unknown Command 0x%X len=%d cmd=%s", wType, wLen, strCmd);
	WriteSystemMsg(strOut);

	return 1;
}

void CMetis3View::Input(CString strText)
{

	//m_eInput.SetWindowText(strText);
	//m_eInput.PostMessage(WM_KEYDOWN, 13, 256);
	OnInput(1, (LPARAM)(LPCTSTR)strText);
}

LRESULT CMetis3View::OnInput(WPARAM wParam, LPARAM lParam)
{

	if(wParam == 1){

		m_strInput = (LPCTSTR)lParam;
	}
	else{

		UpdateData(TRUE);
	}
	if(m_strInput.GetLength() > 400){

		m_strInput = m_strInput.Left(400);
	}
	
	BOOL bAction = FALSE;

	if(m_strInput.Find("{\\rtf", 0) >= 0){

		m_strInput = "Sorry, I made a futile attemp to send an RTF exploit to the room. RoboMX outsmarted me though and blocked it. Please kick my butt!";
	}

	if(m_strInput.Find("/me ", 0) == 0){

		m_strInput = m_strInput.Mid(4);
		bAction = TRUE;
	}
	else if(m_strInput.Find("/action ", 0) == 0){

		m_strInput = m_strInput.Mid(9);
		bAction = TRUE;
	}
	else if(m_strInput.Find("/emote ", 0) == 0){

		m_strInput = m_strInput.Mid(8);
		bAction = TRUE;
	}
	else if(m_strInput.Find("/all ", 0) == 0){

		m_strInput = m_strInput.Mid(5);
		((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.InputAll(m_strInput);
		m_strInput = "";
		UpdateData(FALSE);
		return 1;
	}
	else if(m_strInput == "/clear"){

		m_strInput = "";
		UpdateData(FALSE);
		m_rChat.SetSel(0,-1);
		m_rChat.ReplaceSel("");
	}

	for(int i = 0; i < g_aPlugins.GetSize(); i++){

		PLUGIN->OnInputHook((DWORD)m_hWnd, &m_strInput);
	}

	if(!m_strInput.IsEmpty()){

		m_mxClient.SendMessage(m_strInput, m_strInput.GetLength(), bAction);
		UpdateAverageLag();
		m_strInput = "";
	}
	UpdateData(FALSE);
	OnUpdate(0,0,0);
	return 1;
}


void CMetis3View::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	ReCalcLayout();	
}

void CMetis3View::OnRename() 
{

	CRenameDlg dlg;
	dlg.m_dwFiles = m_mxClient.m_dwFiles;
	dlg.m_nLine   = m_mxClient.m_wLineType;
	dlg.m_strName = m_mxClient.m_strUser;

	if(dlg.DoModal() == IDOK){

		if(dlg.m_bAllRooms){

			((CMainFrame*)GetApp()->m_pMainWnd)->m_wndDocSelector.BroadcastMessage(UWM_RENAMECL, 0, 0);
		}
		else{

			m_mxClient.SendRename(dlg.m_strName, dlg.m_dwFiles, dlg.m_nLine);
		}

		GetDocument()->m_dwFiles = dlg.m_dwFiles;
		GetDocument()->m_strName = dlg.m_strName;
		GetDocument()->m_wLine   = dlg.m_nLine;
		GetDocument()->SetTitle(GetDocument()->m_strRoom);
	}
}


LRESULT CMetis3View::OnSystem(WPARAM wParam, LPARAM lParam)
{

	WriteSystemMsg((LPCSTR)lParam, (COLORREF)wParam);

	return 1;
}

LRESULT CMetis3View::OnRedirect(WPARAM wParam, LPARAM lParam)
{

	CString strOut, strTarget;
	
	strTarget = (LPCTSTR)lParam;
	strOut.Format("Redirecting to %s", strTarget);
	WriteSystemMsg(strOut, g_sSettings.GetRGBPend());

	// 0100007F1A2B
	if((strTarget.GetLength() - strTarget.ReverseFind('_') -1) != 12){

		strOut.Format("%s does not appear to be a valid room name. Redirect aborted.", strTarget);
		WriteSystemMsg(strOut, g_sSettings.GetRGBErr());
		if(m_mxClient.m_bListen){

			m_mxClient.Disconnect();
		}
	}
	else{

		if(m_mxClient.m_bListen){

			m_mxClient.Disconnect();
		}
		m_bHasJoined = FALSE;
		m_lcUsers.DeleteAllItems();
		m_mxClient.SetRoom(strTarget);
		GetDocument()->m_strRoom = strTarget;
		GetDocument()->SetTitle(GetDocument()->m_strRoom);
		m_mxClient.Connect();
		if(g_sSettings.GetSoundFX()){

			PlaySound(g_sSettings.GetSfxRedirect(), NULL, SND_FILENAME|SND_ASYNC);
		}

	}

	return 1;
}

void CMetis3View::OnLeave() 
{

	GetDocument()->OnCloseDocument();
}

void CMetis3View::ReCalcLayout()
{

	if(!m_rSys.m_hWnd || !m_rChat.m_hWnd || !m_sSplitter1.m_hWnd || !m_sSplitter2.m_hWnd){

		return;
	}

	CRect	rcItem;
	CRect	rcItem2;
	CRect	rcParent;

	GetClientRect(rcParent);
	ShowScrollBar(SB_BOTH , FALSE);
	// Get Button Position
	m_btExit.GetWindowRect(rcItem2);
	ScreenToClient(rcItem2);
	// Get Edit Position
	m_eInput.GetWindowRect(rcItem);
	ScreenToClient(rcItem);

	int nHeight = rcItem.Height();  // Save height of button
	rcItem.bottom = rcParent.bottom - 1;
	rcItem.top = rcItem.bottom - nHeight;
	rcItem.right = rcParent.right - 6 - rcItem2.Width();

	nHeight = rcItem2.Height();
	int nWidth = rcItem2.Width();
	rcItem2.bottom = rcParent.bottom - 1;
	rcItem2.right  = rcParent.right - 1;
	rcItem2.top    = rcItem2.bottom - nHeight;
	rcItem2.left   = rcItem2.right  - nWidth;

	m_eInput.MoveWindow(rcItem);
	m_btExit.MoveWindow(rcItem2);

	// move userlist
	m_lcUsers.GetWindowRect(rcItem);
	ScreenToClient(rcItem);

	nWidth = rcItem.Width();
	rcItem.right = rcParent.right;
	rcItem.left  = rcParent.right - nWidth;
	rcItem.bottom = rcItem2.top - 2;
	m_lcUsers.MoveWindow(rcItem);

	// move/size splitter1
	m_sSplitter1.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - nWidth - 1;
	rcItem.left  = rcParent.right - nWidth - 6;
	rcItem.bottom = rcParent.bottom - 6 - nHeight;
	m_sSplitter1.MoveWindow(rcItem);

	// resize system out
	m_rSys.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - 6 - nWidth;
	m_rSys.MoveWindow(rcItem);
	
	// resize splitter2
	m_sSplitter2.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - 6 - nWidth;
	m_sSplitter2.MoveWindow(rcItem);

	// resize chat
	m_rChat.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right  = rcParent.right - 6 - nWidth;
	rcItem.bottom = rcItem2.top - 2;
	m_rChat.MoveWindow(rcItem);

}

void CMetis3View::OnRclickUserlist(NMHDR* pNMHDR, LRESULT* pResult) 
{

	CMenu mContextMenu;
	mContextMenu.LoadMenu(IDR_USER);

	POINT point;
	GetCursorPos(&point);

	if(m_lcUsers.GetNextItem(-1, LVNI_FOCUSED) >= 0){

		mContextMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
											point.x,
											point.y,
											AfxGetMainWnd());
	}

	*pResult = 0;
}

LRESULT CMetis3View::OnRclickChat(WPARAM w, LPARAM l)
{

	CMenu mContextMenu;
	mContextMenu.LoadMenu(IDR_CHAT);

	POINT point;
	GetCursorPos(&point);

	mContextMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
											point.x,
											point.y,
											AfxGetMainWnd());

	return 1;
}

void CMetis3View::OnUserlistSendmessage() 
{

	AfxMessageBox("Not implemented\n");
}

void CMetis3View::OnUserlistRedirect() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[0], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistKick() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[1], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistKickban() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[2], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistBan() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[3], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistUnban() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[4], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistPrintuserstat() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[5], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistPrintip() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[6], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistAddadmin() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[7], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistRemoveadmin() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[8], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistReadusermessage() 
{

	
	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd.Format("%s %s", m_aRCMSMenu[9], m_lcUsers.GetItemText(n, 0));
		m_mxClient.SendMessage(strCmd, strCmd.GetLength(), FALSE);
	}
}

void CMetis3View::OnUserlistCustomizethismenu() 
{
	CString strIniFile, strRoom;

	strRoom = GetDocument()->m_strRoomShort;
	strRoom.Remove('?');
	strRoom.Remove(':');
	strRoom.Remove(',');
	strRoom.Remove('\\');
	strRoom.Remove('/');
	strRoom.Remove('<');
	strRoom.Remove('>');
	strRoom.Remove('\"');
	strRoom.Remove('*');
	strRoom.Remove('|');
	strRoom.Replace(1, '-');

	strIniFile.Format("%s.rcms", strRoom);


	CStdioFile ini;
	CString strBuffer;
	//m_aRCMSMenu.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText|CFile::shareExclusive);

		if(ini.GetLength() == 0){

			if(m_nServerType != SERVER_RCMS){

				m_aRCMSMenu.Add("#UserCmd Redirect");
				m_aRCMSMenu.Add("/kick");
				m_aRCMSMenu.Add("/kickban");
				m_aRCMSMenu.Add("/ban");
				m_aRCMSMenu.Add("/unban");
				m_aRCMSMenu.Add("/level");
				m_aRCMSMenu.Add("#AdminCmd PrintIP");
				m_aRCMSMenu.Add("/setuserlevel 190");
				m_aRCMSMenu.Add("/setuserlevel 60");
				m_aRCMSMenu.Add("#UserCmd Readmsg");
			}
			else{
				ini.WriteString("#UserCmd Redirect\n");
				ini.WriteString("#UserCmd /kick\n");
				ini.WriteString("#AdminCmd /kickban\n");
				ini.WriteString("#AdminCmd /ban\n");
				ini.WriteString("#UserCmd /unban\n");
				ini.WriteString("#userCmd PrintUserStat\n");
				ini.WriteString("#AdminCmd PrintIP\n");
				ini.WriteString("#AdminCmd AddAdmin\n");
				ini.WriteString("#AdminCmd RemoveAdmin\n");
				ini.WriteString("#UserCmd Readmsg\n");
			}
		}

		ini.Close();

		
	}
	CATCH(CFileException, e){

	}END_CATCH;


	AfxMessageBox("RoboMX will now open the a configuration file in notepad. Please adjust the RCMS commands to the one used in this room, save the document and re-join the channel. Do not change the order of the commands in the file.", MB_ICONINFORMATION);

	ShellExecute(0, "open", "notepad.exe", strIniFile, 0, SW_SHOW);
}

void CMetis3View::OnUserlistCopyusername() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strCmd;
	if(n >= 0){

		strCmd = m_lcUsers.GetItemText(n, 0);
		CClipboard::SetText((LPSTR)(LPCSTR)strCmd);
	}	
}


void CMetis3View::OnUserlistIgnore() 
{
	
	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strUser;
	if(n >= 0){

		strUser = m_lcUsers.GetItemText(n, 0);
		strUser = m_lcUsers.GetItemText(n, 0);
		n = strUser.ReverseFind('_');
		if(n > 0){

			strUser = strUser.Left(n);
		}
		for(int i = 0; i < g_aIgnored.GetSize(); i++){

			if(g_aIgnored[i] == strUser)
				return;
		}
		g_aIgnored.Add(strUser);
		if(GetKeyState(VK_SHIFT) < 0){

			CString strIgnored;
			strIgnored.Format("ignored user '%s'", strUser);
			m_mxClient.SendMessage(strIgnored, strIgnored.GetLength(), TRUE);
		}
	}	
}

void CMetis3View::OnUserlistUnignore() 
{

	int n = m_lcUsers.GetNextItem(-1, LVNI_FOCUSED);
	CString strUser;
	if(n >= 0){

		strUser = m_lcUsers.GetItemText(n, 0);
		n = strUser.ReverseFind('_');
		if(n > 0){

			strUser = strUser.Left(n);
		}
		for(int i = 0; i < g_aIgnored.GetSize(); i++){

			if(g_aIgnored[i] == strUser){

				g_aIgnored.RemoveAt(i);
				if(GetKeyState(VK_SHIFT) < 0){

					CString strIgnored;
					strIgnored.Format("un-ignored user '%s'", strUser);
					m_mxClient.SendMessage(strIgnored, strIgnored.GetLength(), TRUE);
				}
			}
		}
	}	
}

void CMetis3View::OnUpdateUserlistMenu(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(m_lcUsers.GetNextItem(-1, LVNI_SELECTED) != -1);
}

void CMetis3View::LoadRCMSMenu()
{

	CString strIniFile, strRoom;

	strRoom = GetDocument()->m_strRoomShort;
	strRoom.Remove('?');
	strRoom.Remove(':');
	strRoom.Remove(',');
	strRoom.Remove('\\');
	strRoom.Remove('/');
	strRoom.Remove('<');
	strRoom.Remove('>');
	strRoom.Remove('\"');
	strRoom.Remove('*');
	strRoom.Remove('|');
	strRoom.Replace(1, '-');

	strIniFile.Format("%s.rcms", strRoom);


	CStdioFile ini;
	CString strBuffer;
	m_aRCMSMenu.RemoveAll();

	TRY{

		if(ini.Open(strIniFile, CFile::modeRead|CFile::typeText|CFile::shareExclusive)){


			while(ini.ReadString(strBuffer)){

				if(!strBuffer.IsEmpty()){

					strBuffer.TrimLeft();
					strBuffer.TrimRight();
					m_aRCMSMenu.Add(strBuffer);
				}
			}
			ini.Close();
		}
		
	}
	CATCH(CFileException, e){

	}END_CATCH;

	if(m_aRCMSMenu.GetSize() == 0){

		if(m_nServerType != SERVER_RCMS){

			m_aRCMSMenu.Add("#UserCmd Redirect");
			m_aRCMSMenu.Add("/kick");
			m_aRCMSMenu.Add("/kickban");
			m_aRCMSMenu.Add("/ban");
			m_aRCMSMenu.Add("/unban");
			m_aRCMSMenu.Add("/level");
			m_aRCMSMenu.Add("#AdminCmd PrintIP");
			m_aRCMSMenu.Add("/setuserlevel 190");
			m_aRCMSMenu.Add("/setuserlevel 60");
			m_aRCMSMenu.Add("#UserCmd Readmsg");
		}
		else{

			// Fill in defaults
			m_aRCMSMenu.Add("#UserCmd Redirect");
			m_aRCMSMenu.Add("#UserCmd /kick");
			m_aRCMSMenu.Add("#AdminCmd /kickban");
			m_aRCMSMenu.Add("#AdminCmd /ban");
			m_aRCMSMenu.Add("#UserCmd /unban");
			m_aRCMSMenu.Add("#UserCmd PrintUserStat");
			m_aRCMSMenu.Add("#AdminCmd PrintIP");
			m_aRCMSMenu.Add("#AdminCmd AddAdmin");
			m_aRCMSMenu.Add("#AdminCmd RemoveAdmin");
			m_aRCMSMenu.Add("#UserCmd Readmsg");
		}
	}
}


void CMetis3View::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	if(((CMainFrame*)GetApp()->m_pMainWnd)->GetActiveFrame() != GetParentFrame()){

		return;
	}

	if(m_pStatusBar){

		CString strText;
		
		m_pStatusBar->SetPaneText(1, GetDocument()->m_strRoomShort);

		strText.Format("Average Lag: %u ms", m_dwAvLag);
		COLORREF cr = 0;
		if(m_dwAvLag <= 500) cr = RGB(0, 100, 0);
		else if((m_dwAvLag > 500) && (m_dwAvLag <= 1000)) cr = 0;
		else if((m_dwAvLag > 1000) && (m_dwAvLag < 1500)) cr = RGB(255, 192, 64);
		else cr = RGB(200, 0, 0);
		m_pStatusBar->SetPaneText(2, strText, cr);

		strText.Format("%0d users", m_lcUsers.GetItemCount());
		m_pStatusBar->SetPaneText(3, strText);
		//m_pStatusBar->Invalidate();
	}
}

void CMetis3View::UpdateAverageLag(BOOL bStart)
{

	if(bStart){

		m_dwLastTic = GetTickCount();
	}
	else if(m_dwLastTic != 0){


		DWORD dwTime = GetTickCount() - m_dwLastTic;
		CString strText;

		m_dwAvLag = (m_dwAvLag + dwTime) / 2;
		m_dwLastTic = 0;
		
		strText.Format("Average Lag: %u ms", m_dwAvLag);
		
		COLORREF cr = 0;
		if(m_dwAvLag <= 500) cr = RGB(0, 100, 0);
		else if((m_dwAvLag > 500) && (m_dwAvLag <= 1000)) cr = 0;
		else if((m_dwAvLag > 1000) && (m_dwAvLag < 1500)) cr = RGB(255, 192, 64);
		else cr = RGB(200, 0, 0);
		
		m_pStatusBar->SetPaneText(2, strText, cr);
	}
}


void CMetis3View::OnReconnect() 
{

	if(m_mxClient.m_bListen){

		m_mxClient.Disconnect();
	}
	WriteSystemMsg("Reconnecting. Stand by...", g_sSettings.GetRGBPend());
	m_lcUsers.DeleteAllItems();
	m_mxClient.Connect();
}

void CMetis3View::OnUpdateReconnect(CCmdUI* pCmdUI) 
{

	//pCmdUI->Enable(!m_mxClient.m_bListen);
}

CString CMetis3View::GetUpTime()
{

	CString strUp;
	DWORD nMS = GetTickCount();
	int nSec = nMS / 1000;
	int nMin = nMS / 60000;
	int nHour = nMS / 3600000;
	strUp.Format("System Uptime %02d days %02d hours %02d minutes %02d seconds %03d milliseconds",
		nHour/24, nHour%24, nMin - nHour*60, nSec - nMin*60, nMS-nSec*1000);

	return strUp;
}

CString CMetis3View::GetWinampSong()
{

	CString strWinamp, strOut;
	HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);
	if(hwndWinamp != NULL){

		::SendMessage(hwndWinamp, WM_WA_IPC,0,IPC_UPDTITLE);
		TCHAR *buff = new TCHAR[250];
		::GetWindowText(hwndWinamp, buff, 250);
		strWinamp = buff;
		strWinamp = strWinamp.Mid(strWinamp.Find(" ", 0)+1, strWinamp.Find(" - Winamp") - strWinamp.Find(" ", 0)-1);
		delete buff;
		buff = NULL;
	}
	else{

		strWinamp = "Winamp - Not active";
	}

	return strWinamp;
}


BOOL CMetis3View::IsVideoPlaying()
{

	HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);
	BOOL bReturn = FALSE;
	switch(::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_IS_PLAYING_VIDEO)){

	case 0:
	case 1:
		bReturn = FALSE;
		break;
	default:
		bReturn = TRUE;
	}

	return bReturn;

}


CString CMetis3View::GetMySystemInfo()
{

	CString strInfo;
	strInfo.Format(
				"%s build=%d «» CPUs %d Speed %d MHz «» %s %s %s",
				g_sSystem.GetOSType(), g_sSystem.GetBuildNumber(),
				g_sSystem.GetNumProcessors(), g_sSystem.GetCPUSpeed(),
				g_sSystem.GetCPUIdentifier(), g_sSystem.GetCPUVendorIdentifier(), g_sSystem.GetCPUNameString()
			);
	
	return strInfo;
}


CString CMetis3View::GetMyLocalTime()
{

	CString strTime;
	SYSTEMTIME time;
	
	GetLocalTime(&time);
	char szTime[9];
	ZeroMemory(&szTime, 9);
	GetTimeFormat(
					LOCALE_SYSTEM_DEFAULT, 
					(g_sSettings.GetTimeFmt() ? TIME_NOSECONDS : TIME_FORCE24HOURFORMAT),
					&time,
					(g_sSettings.GetTimeFmt() ? "hh':'mm tt" : "HH':'mm':'ss"),
					(char*)&szTime, 8
				 );

	strTime = szTime;


	//strTime.Format("%02d:%02d:%02d", time.wHour, time.wMinute, time.wSecond);

	return strTime;
}

void CMetis3View::ReplaceVars(CString &strMsg)
{

	if(strMsg.Find("%", 0) < 0 ) return;

	CString strArtist, strSong, strVersion, strPlayTime, strTotalTime, strRemTime, strSampleRate, strBitrate, strNumChannels, strStatus = "not running";
	
	strMsg.Replace(_T("%TIME%"), CMetis3View::GetMyLocalTime());
	
	
	CString strTmp = CMetis3View::GetWinampSong();


	if(strMsg.Find("%WA-", 0) >= 0){


		CTokenizer token(strTmp, "-");
		token.Next(strArtist);
		strSong = token.Tail();
		strArtist.TrimRight();
		strSong.TrimLeft();
		
		HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);

		if(hwndWinamp != NULL){


			strVersion.Format("%x", ::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETVERSION));
			strVersion.SetAt(1, '.');

			int nTotal = 0, nRem = 0, nEla = 0;
			nTotal = ::SendMessage(hwndWinamp, WM_WA_IPC, 1, IPC_GETOUTPUTTIME);
			strTotalTime.Format("%02d:%02d", nTotal/60, nTotal%60);

			nEla= ::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETOUTPUTTIME) / 1000;
			strPlayTime.Format("%02d:%02d", nEla/60, nEla%60);
			
			nRem = nTotal - nEla;
			strRemTime.Format("%02d:%02d", nRem/60, nRem%60);

			strSampleRate.Format("%d", ::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETINFO));
			strBitrate.Format("%d", ::SendMessage(hwndWinamp, WM_WA_IPC, 1, IPC_GETINFO));
			strNumChannels = (::SendMessage(hwndWinamp, WM_WA_IPC, 2, IPC_GETINFO) == 1 ? "Mono" : "Stereo");

			switch(::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_ISPLAYING)){

			case 1: strStatus = "playing";
				break;
			case 3: strStatus = "paused";
				break;
			default: strStatus = "stopped";
			}

		}
		strMsg.Replace(_T("%WA-ARTIST%"), strArtist);
		strMsg.Replace(_T("%WA-SONG%"), strSong);
		strMsg.Replace(_T("%WA-VERSION%"), strVersion);
		strMsg.Replace(_T("%WA-ELATIME%"), strPlayTime);
		strMsg.Replace(_T("%WA-REMTIME%"), strRemTime);
		strMsg.Replace(_T("%WA-TOTALTIME%"), strTotalTime);
		strMsg.Replace(_T("%WA-SAMPLERATE%"), strSampleRate);
		strMsg.Replace(_T("%WA-BITRATE%"), strBitrate);
		strMsg.Replace(_T("%WA-CHANNELS%"), strNumChannels);
		strMsg.Replace(_T("%WA-STATUS%"), strStatus);
	}	
}

void CMetis3View::OnDisplayWinampsong() 
{

	if(!m_mxClient.m_bListen) return;
	
	CString strMsg;
	if(IsVideoPlaying()){

		strMsg = g_sSettings.GetVideoMsg();
	}
	else{

		strMsg = g_sSettings.GetWinampMsg();
	}

	ReplaceVars(strMsg);

	Input(strMsg);
}

void CMetis3View::OnDisplaySystemuptime() 
{
	if(!m_mxClient.m_bListen) return;
	
	Input(GetUpTime());
}

void CMetis3View::OnDisplaySysteminfo() 
{
	if(!m_mxClient.m_bListen) return;
	
	Input(GetMySystemInfo());
}

void CMetis3View::OnDisplayAveragelag() 
{
	if(!m_mxClient.m_bListen) return;
	
	CString strMsg;
	strMsg.Format("Average Lag in channel is %d seconds and %03d milliseconds", m_dwAvLag / 1000, m_dwAvLag % 1000);
	Input(strMsg);
}

void CMetis3View::OnUpdateWinampsongMenu(CCmdUI* pCmdUI) 
{

	HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);
	pCmdUI->Enable(hwndWinamp != NULL);	
}

void CMetis3View::OnChatroomSelectall() 
{

	m_rChat.SetSel(0, -1);
	m_rChat.HideSelection(FALSE, FALSE);
}

void CMetis3View::OnChatroomSearchongoogle() 
{

	CString strSearch = m_rChat.GetSelText();
	if(!strSearch.IsEmpty()){

		CString strTmp;
		strTmp.Format("http://www.google.com/search?hl=en&ie=UTF-8&oe=UTF-8&q=%s&btnG=Google+Search", strSearch);
		ShellExecute(0, "open", strTmp, 0, 0, SW_SHOW);
	}
}

void CMetis3View::OnChatroomQuote() 
{

	CString strQuote = m_rChat.GetSelText();
	if(!strQuote.IsEmpty()){

		m_eInput.SetWindowText(strQuote);
	}
}

void CMetis3View::OnChatroomCopy() 
{

	m_rChat.Copy();
}

void CMetis3View::OnChatroomClearscreen() 
{

	m_rChat.SetSel(0, -1);
	m_rChat.ReplaceSel("");
}

void CMetis3View::OnChatroomCopyroomname() 
{

	CClipboard::SetText((LPSTR)(LPCSTR)GetDocument()->m_strRoom);
}

void CMetis3View::InputWelcome()
{

	if(g_sSettings.GetSoundFX()){

		PlaySound(g_sSettings.GetSfxJoin(), 0, SND_ASYNC|SND_FILENAME);
	}
	if(g_sSettings.GetDoEnterMsg()){
		
		CString strMsg = g_aGreetings[rand() % g_aGreetings.GetSize()];
		ReplaceVars(strMsg);
		Input(strMsg);
	}
	else{

		for(int i = 0; i < g_aRooms.GetSize(); i++){

			if(GetDocument()->m_strRoom.Find(g_aRooms[i], 0) >= 0){

				CString strMsg = g_aGreetings[rand() % g_aGreetings.GetSize()];
				ReplaceVars(strMsg);
				Input(strMsg);
				break;
			}
		}
	}
}


void CMetis3View::OnSetFocus(CWnd* pOldWnd) 
{
	CFormView::OnSetFocus(pOldWnd);
	

	OnUpdate(this, 0, 0);
}

void CMetis3View::OnChatroomTexttricksHacker() 
{

	
	CInputRequest dlg;
	dlg.SetMode(0, &m_fFont);
	if(dlg.DoModal() == IDOK){

		Input(dlg.m_strInput);
	}
}

void CMetis3View::OnChatroomTexttricksBubbles() 
{

	CInputRequest dlg;
	dlg.SetMode(1, &m_fFont);
	if(dlg.DoModal() == IDOK){

		Input(dlg.m_strInput);
	}
}

void CMetis3View::OnChatroomTexttricksBox() 
{

	
	CInputRequest dlg;
	dlg.SetMode(2, &m_fFont);
	if(dlg.DoModal() == IDOK){

		Input(dlg.m_strInput);
	}
}

void CMetis3View::OnChatTexttricks3dbuttonsnormal() 
{
	
	CInputRequest dlg;
	dlg.SetMode(3, &m_fFont);
	if(dlg.DoModal() == IDOK){

		Input(dlg.m_strInput);
	}
}

void CMetis3View::OnChatTexttricks3dbuttonsaction() 
{
	
	CInputRequest dlg;
	dlg.SetMode(4, &m_fFont);
	if(dlg.DoModal() == IDOK){

		Input(dlg.m_strInput);
	}
}

void CMetis3View::OnChatroomAsciiartdesign() 
{

	
	CInputRequest dlg;
	dlg.SetMode(5, &m_fFont);
	if(dlg.DoModal() == IDOK){

		Input(dlg.m_strInput);
	}
}

void CMetis3View::OnEnLinkChat(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENLINK *pEnLink = reinterpret_cast<ENLINK *>(pNMHDR);
	
	AfxMessageBox(":-)");
	*pResult = 0;
}

LRESULT CMetis3View::OnEchoText(WPARAM wParam, LPARAM lParam)
{

	m_rChat.SetText(((EXT_MSG_STRUCT*)lParam)->lpszMsg, (COLORREF)((EXT_MSG_STRUCT*)lParam)->wParam, (COLORREF)((EXT_MSG_STRUCT*)lParam)->lParam);
	return 1;
}

LRESULT CMetis3View::OnEchoSysText(WPARAM wParam, LPARAM lParam)
{

	m_rSys.SetText(((EXT_MSG_STRUCT*)lParam)->lpszMsg, (COLORREF)((EXT_MSG_STRUCT*)lParam)->wParam, (COLORREF)((EXT_MSG_STRUCT*)lParam)->lParam);
	return 1;
}

void CMetis3View::HandleHiLite(void)
{

	if(GetApp()->GetMainFrame()->MDIGetActive() != GetParentFrame()){

		GetApp()->GetMainFrame()->m_wndDocSelector.SetHiLite(this);
	}
}

LRESULT CMetis3View::OnSetServerType(WPARAM wParam, LPARAM lParam)
{

	m_nServerType = (int)wParam;
	switch((int)wParam){

		case SERVER_RCMS:
			m_eInput.SetCommands(&g_aRCMSCommands);
			break;
		case SERVER_WINMX352:
			m_eInput.SetCommands(&g_aWinMXCommands);
			break;
		case SERVER_ROBOMX:
			m_eInput.SetCommands(&g_aWinMXCommands);
			break;
		default:
			ASSERT(FALSE);
	}
	LoadRCMSMenu();
	return 1;
}

LRESULT CMetis3View::OnRenameCl(WPARAM wParam, LPARAM lParam)
{

	GetDocument()->m_strName = g_sSettings.GetNickname();
	GetDocument()->m_wLine   = g_sSettings.GetLine();
	GetDocument()->m_dwFiles = g_sSettings.GetFiles();

	m_mxClient.SendRename(GetDocument()->m_strName, GetDocument()->m_dwFiles, GetDocument()->m_wLine);

	return 1;
}
