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

#include "Metis3Doc.h"
#include "Metis3View.h"
#include "RenameDlg.h"
#include <fstream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_CONNECT 10334
#define TIMER_PING    10035
const char* line_types[] = {"Unknown", "14.4K", "28.8K", "33.3K", "56K", 
                            "64K ISDN", "128K ISDN", "Cable", "DSL", "T1", "T3"};

#define NUM_LINES 11

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

extern axtoi(char *hexStg, int nLen);
/////////////////////////////////////////////////////////////////////////////
// CMetis3View

IMPLEMENT_DYNCREATE(CMetis3View, CFormView)

BEGIN_MESSAGE_MAP(CMetis3View, CFormView)
	//{{AFX_MSG_MAP(CMetis3View)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LEAVE, OnLeave)
	//}}AFX_MSG_MAP
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

}

CMetis3View::~CMetis3View()
{

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
    ResizeParentToFit(FALSE);
	
	//GetApp()->m_pView = this;
	
	LoadEmoticons();

	m_rChat.Init();
	CRect rc;
	CWnd* pWnd = GetDlgItem(IDC_SPLITTER_1);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_sSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER_1);
	m_sSplitter1.SetStyle(SPS_HORIZONTAL);
	m_sSplitter1.SetRange(100, 100, -1);
	
	pWnd = GetDlgItem(IDC_SPLITTER_2);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_sSplitter2.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER_2);
	m_sSplitter2.SetStyle(SPS_HORIZONTAL);
	m_sSplitter2.SetRange(150, 150, -1);
	//m_sSplitter2.SetRange(-150, 150);

	pWnd = GetDlgItem(IDC_STATIC_SYSOUT);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	m_rSys.CreateEx(0, NULL, NULL, WS_VISIBLE | WS_CHILD |
					WS_CLIPCHILDREN | WS_VSCROLL | ES_MULTILINE | ES_READONLY |
					ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN, rc, this, IDC_SYSOUT, NULL);

	pWnd = GetDlgItem(IDC_STATIC_CHAT);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	m_rChat.CreateEx(0, NULL, NULL, WS_VISIBLE | WS_CHILD |
					WS_CLIPCHILDREN | WS_VSCROLL | ES_MULTILINE | ES_READONLY |
					ES_AUTOVSCROLL | ES_LEFT | ES_WANTRETURN, rc, this, IDC_CHAT, NULL);


	m_fFont.CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_STRING_PRECIS, 
		CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_DONTCARE, "Arial");
	ListView_SetExtendedListViewStyle(m_lcUsers.m_hWnd, LVS_EX_FULLROWSELECT);

	// Set up layout
	m_lcUsers.InsertColumn(0, "Name", LVCFMT_LEFT, 300);
	m_lcUsers.InsertColumn(1, "Files", LVCFMT_RIGHT, 60);
	m_lcUsers.InsertColumn(2, "Line", LVCFMT_CENTER, 80);
	m_lcUsers.InsertColumn(3, "Node-IP", LVCFMT_RIGHT, 120);
	m_lcUsers.InsertColumn(4, "Node-Port", LVCFMT_RIGHT, 80);

	m_lcUsers.SetFont(&m_fFont);
	m_eInput.SetFont(&m_fFont);
	
	// Set up client
	m_sSplitter1.SetRange(150, 50, -1);
	m_mxClient.m_pView		= this;
	m_mxClient.m_dwFiles	= GetDocument()->m_dwFiles;
	m_mxClient.m_wLineType	= GetDocument()->m_wLine;
	m_mxClient.m_strUser	= GetDocument()->m_strName;

	m_mxClient.SetRoom(GetDocument()->m_strRoom);
	WriteSystemMsg("Connecting. Stand by...");
	
/*	CBitmap		bitmap;

	// Set up toolbar image lists.
	// Create and set the normal toolbar image list.
	bitmap.LoadMappedBitmap(IDB_ROBOMX);
	m_rChat.InsertBitmap((HBITMAP)bitmap.m_hObject);
	bitmap.Detach();
	m_rChat.SetText("\n", 0);*/

	SetTimer(TIMER_CONNECT, 500, 0);
}


BOOL CMetis3View::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{

	if (wParam == IDC_SPLITTER_1)
	{	

		SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
		DoResize1(pHdr->delta);
	}	
	else if (wParam == IDC_SPLITTER_2)
	{	

		SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
		DoResize2(pHdr->delta);
	}	
	return CFormView::OnNotify(wParam, lParam, pResult);
}

void CMetis3View::DoResize1(int delta)
{

	CSplitterControl::ChangeHeight(&m_lcUsers, delta, CW_TOPALIGN);
	CSplitterControl::ChangeHeight(&m_rSys, -delta, CW_BOTTOMALIGN);
	CSplitterControl::ChangeHeight(&m_rChat, 0);
	CSplitterControl::ChangeHeight(&m_eInput, 0);
	Invalidate();
	UpdateWindow();
}

void CMetis3View::DoResize2(int delta)
{

	CSplitterControl::ChangeHeight(&m_lcUsers, 0);
	CSplitterControl::ChangeHeight(&m_rSys, delta, CW_TOPALIGN);
	CSplitterControl::ChangeHeight(&m_rChat, -delta, CW_BOTTOMALIGN);
	CSplitterControl::ChangeHeight(&m_eInput, 0);
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
	CFormView::OnDestroy();
	
	m_mxClient.Disconnect();
	DeleteEmoticons();
	
}

void CMetis3View::OnTimer(UINT nIDEvent) 
{

	if(nIDEvent == TIMER_CONNECT){

		if(m_mxClient.Connect()){

			WriteSystemMsg("Successfully connected to room.");
		}
		else{

			WriteSystemMsg("Could not join room.");
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
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	m_rSys.SetText(strTime, RGB(255, 255, 255), crText);
	m_rSys.SetText(" " + strMsg + "\n", crText);
}
void CMetis3View::RemoveUser(CString strUser)
{

	LVFINDINFO fi;
	fi.flags = LVFI_STRING;
	fi.psz = strUser;

	int nPos = m_lcUsers.FindItem(&fi, -1);

	if(nPos >= 0){

		m_lcUsers.DeleteItem(nPos);
	}
}

void CMetis3View::AddUser(CString strUsername, WORD wLine, DWORD dwFiles, CString strNodeIP, WORD wNodePort)
{

	//int nPos = m_lcUsers.GetItemCount();
	CString strTmp;
	int nPos = m_lcUsers.InsertItem(0, strUsername, 0);

	strTmp.Format("%03d", dwFiles);
	m_lcUsers.SetItemText(nPos, 1, strTmp);

	if((wLine >= 0) && (wLine < NUM_LINES)){
		
		strTmp = line_types[wLine];
	}
	else{

		strTmp = "N/A";
	}

	m_lcUsers.SetItemText(nPos, 2, strTmp);

	
	m_lcUsers.SetItemText(nPos, 3, strNodeIP);

	strTmp.Format("%d", wNodePort);
	m_lcUsers.SetItemText(nPos, 4, strTmp);

}


LRESULT CMetis3View::OnTopic(WPARAM wParam, LPARAM lParam)
{

	CString strTopic = (LPCSTR)lParam;

	strTopic.Replace("\\rtf", "#rtf");
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	m_rChat.SetText(strTime, RGB(255, 255, 255), RGB(0, 0, 120));
	m_rChat.SetText(" Topic: " + strTopic + "\n", RGB(0, 0, 120));

	return 1;
}

LRESULT CMetis3View::OnMotd(WPARAM wParam, LPARAM lParam)
{

	CString strMotd = (LPCSTR)lParam;
	strMotd.Replace("\\rtf", "#rtf");
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	m_rChat.SetText(strTime, RGB(255, 255, 255), RGB(0, 0, 180));
	m_rChat.SetText(" " + strMotd + "\n", RGB(0, 0, 180));

	return 1;
}

LRESULT CMetis3View::OnAddUser(WPARAM wParam, LPARAM lParam)
{

	char* pUserInfo = (char*)lParam;
	WORD  wLen      = (WORD)wParam;
	//Userlist-Notification (Server)
    // 0x006F][Username:N][00:1][IP-Address:4][UDP-Port:2][Line-Type:2][Shared-Files:4]
	CString strUser = pUserInfo, strTmp;
	int nLen = strUser.GetLength()+1;

	MX_USERINFO user;
	user.strUser = strUser;
	
	DWORD dwTest;
	memcpy(&dwTest,   pUserInfo+nLen,   4);
	strTmp.Format("%X", dwTest);
	if(strTmp.GetLength() < 8){
	
		strTmp.Insert(0, "0");
	}
	int nA = 0, nB = 0, nC = 0, nD = 0;
	
	nA = axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
	nB = axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
	nC = axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
	nD = axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);

	user.strNodeIP.Format("%d.%d.%d.%d", nD, nC, nB, nA);
	memcpy(&user.wNodePort,  pUserInfo+nLen+4, 2);
	memcpy(&user.wLineType,  pUserInfo+nLen+6, 2);
	memcpy(&user.dwNumFiles, pUserInfo+nLen+8, 4);
	m_aUsers.Add(user);
	m_aUsers.Add(user);
	
	AddUser(strUser, user.wLineType, user.dwNumFiles, user.strNodeIP, user.wNodePort);

	return 1;
}

LRESULT CMetis3View::OnJoin(WPARAM wParam, LPARAM lParam)
{

	char* pUserInfo = (char*)lParam;
	WORD  wLen		= (WORD)wParam;
	//User-Join notification (Server)
	//0x006E][Username:N][00:1][IP-Address:4][UDP-Port:2][Line-Type:2][Shared-Files:4]
	CString strUser = pUserInfo, strTmp;
	int nLen = strUser.GetLength()+1;
	
	if(nLen >= wLen) return 0;

	MX_USERINFO user;
	user.strUser = strUser;

	DWORD dwTest;
	memcpy(&dwTest,   pUserInfo+nLen,   4);
	strTmp.Format("%X", dwTest);
	if(strTmp.GetLength() < 8){
	
		strTmp.Insert(0, "0");
	}
	int nA = 0, nB = 0, nC = 0, nD = 0;
	
	nA = axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
	nB = axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
	nC = axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
	nD = axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);

	user.strNodeIP.Format("%d.%d.%d.%d", nD, nC, nB, nA);
	memcpy(&user.wNodePort,  pUserInfo+nLen+4, 2);
	memcpy(&user.wLineType,  pUserInfo+nLen+6, 2);
	memcpy(&user.dwNumFiles, pUserInfo+nLen+8, 4);
	m_aUsers.Add(user);
	
	
	strUser.Replace("\\rtf", "#rtf");
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	CString strJoin;
	if((user.wLineType >= 0) && (user.wLineType< NUM_LINES)){
		
		strTmp = line_types[user.wLineType];
	}
	else{

		strTmp = "N/A";
	}
	strJoin.Format(" >>> Joins: %s (%d Files, %s)\n", strUser, user.dwNumFiles, strTmp);
	m_rChat.SetText(strTime, RGB(255,255,255), RGB(255, 150, 0));
	m_rChat.SetText(strJoin, RGB(255, 150, 0));

	AddUser(strUser, user.wLineType, user.dwNumFiles, user.strNodeIP, user.wNodePort);

	return 1;
}

LRESULT CMetis3View::OnRenameMsg(WPARAM wParam, LPARAM lParam)
{

	char* pRenameInfo = (char*)lParam;
	WORD  wLen		  = (WORD)wParam;
	//Rename notification (Server)
	//0x0070]
	//[Name:N][00:1][IP-Address:4][UDP-Port:2] // old
	//[Name:N][00:1][IP-Address:4][UDP-Port:2][Line-Type:2][Shared-Files:4] // new
	CString strOldname = pRenameInfo;
	DWORD   dwOldIP, dwNewIP, dwFiles;
	WORD    wOldPort, wNewPort, wLine;
	int nLen = strOldname.GetLength();

	if(nLen+7 >= wLen) return 0; // error
	
	memcpy(&dwOldIP, (pRenameInfo+nLen+1), 4);
	memcpy(&wOldPort, (pRenameInfo+nLen+5), 2);

	nLen+=7;
		
	CString strNewname = (pRenameInfo+nLen);
	nLen+= strNewname.GetLength();

	if(nLen+7 > wLen) return 0; // error

	memcpy(&dwNewIP, (pRenameInfo+nLen+1), 4);
	memcpy(&wNewPort, (pRenameInfo+nLen+5), 2);
	memcpy(&wLine, (pRenameInfo+nLen+7), 2);
	memcpy(&dwFiles, (pRenameInfo+nLen+9), 4);

	LVFINDINFO fi;
	fi.flags = LVFI_STRING;
	fi.psz = strOldname;

	int nPos = m_lcUsers.FindItem(&fi, -1);
	
	CString strTmp;
	if(nPos >= 0){

		for(int i = 0; i < m_aUsers.GetSize(); i++){

			if(m_aUsers[i].strUser == strOldname) break;
		}

		m_lcUsers.SetItemText(nPos, 0, strNewname); // name
		
		strTmp.Format("%03d", dwFiles);
		m_lcUsers.SetItemText(nPos, 1, strTmp); // files
		m_aUsers[i].strUser = strNewname;

		if((wLine >= 0) && (wLine < NUM_LINES)){
			
			strTmp = line_types[wLine];
		}
		else{

			strTmp = "N/A";
		}
		m_lcUsers.SetItemText(nPos, 2, strTmp); // line
		m_aUsers[i].wLineType = wLine;

		strTmp.Format("%X", dwNewIP);
		if(strTmp.GetLength() < 8){
		
			strTmp.Insert(0, "0");
		}
		int nA = 0, nB = 0, nC = 0, nD = 0;
		
		nA = axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
		nB = axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
		nC = axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
		nD = axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);

		strTmp.Format("%d.%d.%d.%d", nD, nC, nB, nA);
		m_lcUsers.SetItemText(nPos, 3, strTmp); // nodeip
		m_aUsers[i].strNodeIP = strTmp;

		strTmp.Format("%d", wNewPort);
		m_lcUsers.SetItemText(nPos, 4, strTmp); // nodeport
		m_aUsers[i].wNodePort = wNewPort;
	}

	return 1;
}

LRESULT CMetis3View::OnPart(WPARAM wParam, LPARAM lParam)
{

	//User-Part notification (Server)
	//0x0073][Username:N][00:1][IP-Address:4][UDP-Port:2]

	CString strUser = (LPCSTR)lParam;

	strUser.Replace("\\rtf", "#rtf");
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strPart, strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	strPart.Format(" <<< Parts: %s\n", strUser);
	m_rChat.SetText(strTime, RGB(255,255,255), RGB(120, 0, 0));
	m_rChat.SetText(strPart, RGB(120, 0, 0));

	RemoveUser(strUser);

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
	strName.Replace("\\rtf", "#rtf");
	strMsg.Replace("\\rtf", "#rtf");
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	m_rChat.SetText(strTime, RGB(255,255,255), RGB(255, 0, 0));
	m_rChat.SetText(" " + strName + " :  ", RGB(255, 0, 0));
	m_rChat.SetText(strMsg + "\n", RGB(0, 0, 0));

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
	strName.Replace("\\rtf", "#rtf");
	strMsg.Replace("\\rtf", "#rtf");
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString strTime;
	strTime.Format("[%02d:%02d.%02d]", time.wHour, time.wMinute, time.wSecond);
	
	m_rChat.SetText(strTime, RGB(255,255,255), RGB(0, 91, 183));
	m_rChat.SetText(" * " + strName + " ", RGB(0, 91, 183));
	m_rChat.SetText(strMsg + "\n", RGB(0, 91, 183));

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
		if(c == '\0') strCmd+="[00]";
		else strCmd.Insert(i, c);
	}
	strOut.Format("Unknown Command 0x%X len=%d cmd=%s", wType, wLen, strCmd);
	WriteSystemMsg(strOut);

	return 1;
}



LRESULT CMetis3View::OnInput(WPARAM wParam, LPARAM lParam)
{

	UpdateData(TRUE);
	if(m_strInput.GetLength() > 400){

		m_strInput = m_strInput.Left(400);
	}

	BOOL bAction = FALSE;

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

	m_mxClient.SendMessage(m_strInput, m_strInput.GetLength(), bAction);
	m_strInput = "";
	UpdateData(FALSE);
	return 1;
}


void CMetis3View::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if(!m_rSys.m_hWnd || !m_rChat.m_hWnd || !m_sSplitter1.m_hWnd || !m_sSplitter2.m_hWnd){

		return;
	}

	CRect	rcItem;
	CRect	rcItem2;
	CRect	rcParent;

	GetClientRect(rcParent);
	

	// resize userlist
	m_lcUsers.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - 1;
	m_lcUsers.MoveWindow(rcItem);
	// resize splitter1
	m_sSplitter1.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - 1;
	m_sSplitter1.MoveWindow(rcItem);
	// resize system out
	m_rSys.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - 1;
	m_rSys.MoveWindow(rcItem);
	// resize splitter2
	m_sSplitter2.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right = rcParent.right - 1;
	m_sSplitter2.MoveWindow(rcItem);
	
	// Get Button Position
	m_btExit.GetWindowRect(rcItem2);
	ScreenToClient(rcItem2);
	// Get Edit Position
	m_eInput.GetWindowRect(rcItem);
	ScreenToClient(rcItem);

	int nHeight = rcItem.Height();
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

	// resize chat
	m_rChat.GetWindowRect(rcItem);
	ScreenToClient(rcItem);
	rcItem.right  = rcParent.right - 1;
	rcItem.bottom = rcItem2.top - 2;
	m_rChat.MoveWindow(rcItem);
	
}

void CMetis3View::OnRename() 
{

	CRenameDlg dlg;
	dlg.m_dwFiles = m_mxClient.m_dwFiles;
	dlg.m_nLine   = m_mxClient.m_wLineType;
	dlg.m_strName = m_mxClient.m_strUser;

	if(dlg.DoModal() == IDOK){

		m_mxClient.SendRename(dlg.m_strName, dlg.m_dwFiles, dlg.m_nLine);
		GetDocument()->m_dwFiles = dlg.m_dwFiles;
		GetDocument()->m_strName = dlg.m_strName;
		GetDocument()->m_wLine   = dlg.m_nLine;
		GetDocument()->SetTitle(dlg.m_strName + "@" + GetDocument()->m_strRoom);
	}
}

void CMetis3View::LoadEmoticons()
{

	fstream f;
	char szFileName[1024];
	char szActivationText[64];

	f.open("emoticons.ini", ios::in);

	if (f.eof()) 
		return;

	while (!f.eof())
	{
		f.getline(szActivationText, sizeof(szActivationText));

		if (f.eof())
			return;

		f.getline(szFileName, sizeof(szFileName));

		AddEmoticon(szFileName, szActivationText);
	}
}

void CMetis3View::AddEmoticon(char *szFileName, char *szActivationText)
{

	EMOTICON *eEmoticon = new EMOTICON;

	strcpy(eEmoticon->szActivationText, szActivationText);
	strcpy(eEmoticon->szFileName, szFileName);

	eEmoticon->hBitmap = (HBITMAP)::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_DEFAULTCOLOR);

	if (!eEmoticon->hBitmap)
	{
		delete eEmoticon;
		return;
	}

	m_lEmoticons.AddTail(eEmoticon);
}

void CMetis3View::DeleteEmoticons()
{

	POSITION pos;
	pos = m_lEmoticons.GetHeadPosition();
	while (pos)
	{
		EMOTICON *eEmoticon = m_lEmoticons.GetNext(pos);
		FreeResource(eEmoticon->hBitmap);
		m_lEmoticons.RemoveAt(m_lEmoticons.Find(eEmoticon));
		delete eEmoticon;
	}
}


LRESULT CMetis3View::OnSystem(WPARAM wParam, LPARAM lParam)
{

	WriteSystemMsg((LPCSTR)lParam, (COLORREF)wParam);

	return 1;
}

void CMetis3View::OnLeave() 
{

	GetDocument()->OnCloseDocument();
}

