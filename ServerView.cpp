// ServerView.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include ".\serverview.h"
#include ".\clientsocket.h"
#include ".\tokenizer.h"
#include ".\settings.h"
#include "MainFrm.h"
#include "util.h"
#include "RichElement.h"

// CServerView

UINT UWM_INCOMMING = ::RegisterWindowMessage("UWM_INCOMMING-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
UINT UWM_CLNNOTIFY = ::RegisterWindowMessage("UWM_CLNNOTIFY-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");

IMPLEMENT_DYNCREATE(CServerView, CFormView)

extern CSettings g_sSettings;
extern UINT UWM_LOAD_SETTINGS;
extern UINT UWM_INPUT;

CServerView::CServerView()
	: CFormView(CServerView::IDD)
{

	m_bHosted = FALSE;
	m_eDone.SetEvent();
	m_eHosted.ResetEvent();
	m_eNotifyDone.SetEvent();
	m_strRoomBase = "";
	m_strRoomFull = "";
	m_dwIP        = 0;
	m_wPort       = 0;
	m_dwLimit      = 40;
	m_strTopic    = "";
	m_strMotd     = "";
	m_uMode		  = CM_NORMAL;
	m_bShutdown	  = FALSE;
	m_pServerThread = NULL;
	m_pNotifyThread = NULL;
}

CServerView::~CServerView()
{
	m_fFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(CServerView, CFormView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(UWM_CLNNOTIFY, ClientCallback)
	ON_REGISTERED_MESSAGE(UWM_LOAD_SETTINGS, LoadSettings)
	ON_REGISTERED_MESSAGE(UWM_INPUT, OnInput)
END_MESSAGE_MAP()

void CServerView::DoDataExchange(CDataExchange* pDX)
{

	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_INPUT, m_eInput);
	DDX_Text(pDX, IDC_SERVER_INPUT, m_strInput);
}

// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CServerView message handlers


void CServerView::OnInitialUpdate()
{

	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.AddButton(this, IDR_SERVER);
	

	CRect rc;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_OUT);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_fFont.CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_STRING_PRECIS, 
		CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_DONTCARE, g_sSettings.GetFont());

	m_rSys.Create(WS_CHILD|WS_VISIBLE, rc, this, IDC_STATIC_OUT);
	m_rSys.SetDocument(&m_rSysDoc);
	m_rSys.SetSelectable(TRUE);
	m_rSys.SetFollowBottom(TRUE);
	m_rSysDoc.m_szMargin = CSize(1, 10);
	m_rSysDoc.m_crBackground = g_sSettings.GetRGBBg();

	m_eInput.SetFont(&m_fFont);
	m_eInput.SetBkColor(g_sSettings.GetRGBBg());
	m_eInput.SetCommands(&g_sSettings.m_aRoboMXCommands);
	LoadSettings(0,0);
	ReCalcLayout();
	WriteText(IDS_ROBOMX_ROOMSERVER);

	InitializeCriticalSection(&m_csLock);
}

LRESULT CServerView::LoadSettings(WPARAM wParam, LPARAM lParam)
{

	m_fFont.DeleteObject();
	m_fFont.CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_STRING_PRECIS, 
		CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_DONTCARE, g_sSettings.GetFont());
	m_rSysDoc.m_crBackground = g_sSettings.GetRGBBg();
	m_eInput.SetFont(&m_fFont);
	m_eInput.SetBkColor(g_sSettings.GetRGBBg());
	return 1;
}

void CServerView::OnDestroy()
{
	
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.RemoveButton(this);

	if(m_bHosted){	 // last chance to stop

		m_bShutdown = TRUE;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			m_aClients[i]->m_hMsgTarget = NULL;
		}
		Stop();
	}
	DeleteCriticalSection(&m_csLock);
	CFormView::OnDestroy();
}

void CServerView::OnCloseDocument()
{
	

	if(m_bHosted){

		m_bShutdown = TRUE;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			m_aClients[i]->m_hMsgTarget = NULL;
		}
		Stop();
	}
}

void CServerView::OnSize(UINT nType, int cx, int cy) 
{

	CFormView::OnSize(nType, cx, cy);

	ReCalcLayout();
}

void CServerView::ReCalcLayout()
{

	if(!m_rSys.m_hWnd || !m_eInput.m_hWnd) return;

	CRect	rcParent;
	CRect   rcEdit, rcSys;
	int nHeight = 0;

	GetWindowRect(&rcParent);
	ScreenToClient(&rcParent);

	m_eInput.GetWindowRect(&rcEdit);
	ScreenToClient(&rcEdit);

	nHeight			= rcEdit.Height();
	rcEdit.right	= rcParent.right - 1;
	rcEdit.bottom	= rcParent.bottom - 1;
	rcEdit.top		= rcEdit.bottom-nHeight;
	
	m_eInput.MoveWindow(&rcEdit);

	m_rSys.GetWindowRect(&rcSys);
	ScreenToClient(&rcSys);

	rcSys.right		= rcParent.right - 1;
	rcSys.bottom	= rcEdit.top - 3;

	m_rSys.MoveWindow(&rcSys);
}
void CServerView::Stop(void)
{

	m_bHosted = FALSE;
	m_mServer.Close();

	TRACE("Waiting for Worker threads to exit\n");	
	DWORD n = WaitForSingleObject(m_eDone, 1000);
	if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

		TerminateThread(m_pServerThread->m_hThread, 0);
	}
	
	n = WaitForSingleObject(m_eNotifyDone, 1000);
	if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

		TerminateThread(m_pNotifyThread->m_hThread, 0);
	}

	TRACE("Cleaning up...\n");
	
	while(m_aClients.GetSize() > 0){

		CClientSocket* pTmp = m_aClients[m_aClients.GetSize()-1];
		m_aClients.RemoveAt(m_aClients.GetSize()-1);
		pTmp->LogOut();
		delete pTmp;
		pTmp = NULL;
	}
	
	ClientNotify* pN = NULL;
	while(!m_qNotifies.empty()){

		pN = m_qNotifies.front();
		m_qNotifies.pop();
		delete pN;
		pN = NULL;
	}
	m_pServerThread = NULL;
	m_pNotifyThread = NULL;
}


void CServerView::EchoChat(CString strName, CString strMsg)
{

	if(m_bShutdown) return;

	CString strTime, strOut;
	strTime.Format("[%s]", Util::GetMyLocalTime());
	
	CRichElement *pEl = 0;

	CTokenizer token(strMsg, "\n");
	while(token.Next(strOut)){

		pEl = m_rSysDoc.Add(retText, strTime, NULL, g_sSettings.GetDefaultStyle());
		pEl->m_cBgColor = RGB(150, 150, 150);
		pEl->m_cColor   = g_sSettings.GetRGBTime();

		pEl = m_rSysDoc.Add(retName, " " + strName, NULL, g_sSettings.GetDefaultStyle());
		pEl->m_cBgColor = g_sSettings.GetRGBBg();
		pEl->m_cColor   = RGB(120, 120, 120);

		pEl = m_rSysDoc.Add(retText, ": " + strOut, NULL, g_sSettings.GetDefaultStyle());
		pEl->m_cBgColor = g_sSettings.GetRGBBg();
		pEl->m_cColor   = RGB(150, 150, 150);
		m_rSysDoc.Add(retNewline, NEWLINE_FORMAT);
	}
	m_rSys.InvalidateIfModified();
}

void CServerView::WriteText(LPCTSTR lpszText)
{

	if(m_bShutdown) return;

	CString strTime, strMsg = lpszText, strOut;
	strTime.Format("[%s]", Util::GetMyLocalTime());
	
	CRichElement *pEl = 0;

	CTokenizer token(strMsg, "\n");
	while(token.Next(strOut)){

		pEl = m_rSysDoc.Add(retText, strTime, NULL, g_sSettings.GetDefaultStyle());
		pEl->m_cBgColor = g_sSettings.GetRGBNormalMsg();
		pEl->m_cColor   = g_sSettings.GetRGBTime();

		pEl = m_rSysDoc.Add(retText, " " + strOut, NULL, g_sSettings.GetDefaultStyle());
		pEl->m_cBgColor = g_sSettings.GetRGBBg();
		pEl->m_cColor   = g_sSettings.GetRGBNormalMsg();
		m_rSysDoc.Add(retNewline, NEWLINE_FORMAT);
	}
	m_rSys.InvalidateIfModified();
}

void CServerView::WriteText(UINT nID, ...)
{

    TCHAR szBuffer[8192];
    TCHAR szFormat[8192];
	::LoadString(GetApp()->m_hInstance, nID, szFormat, 8192);
	va_list ap;

	ZeroMemory(szBuffer,8192);
    va_start(ap, nID);
	
    _vsntprintf(szBuffer, sizeof(szBuffer), szFormat, ap);
	va_end(ap);

	WriteText(szBuffer);
}

LRESULT CServerView::OnInput(WPARAM wParam, LPARAM lParam)
{

	UpdateData(TRUE);
	WriteText(m_strInput);
	HandleCommand(m_strInput);
	m_strInput.Empty();
	UpdateData(FALSE);
	return 1;
}

BOOL CServerView::PreTranslateMessage(MSG* pMsg)
{

	return CFormView::PreTranslateMessage(pMsg);
}

void CServerView::HandleCommand(CString strCmd)
{

	strCmd.TrimRight();

	if(strCmd.CompareNoCase("help") == 0){

		PrintHelp();
	}
	else if(strCmd.Find("host") == 0){

		if(m_bHosted){

			WriteText(IDS_ERROR_SERVER_ONEROOMPERCONSOLE);
		}
		else{

			strCmd.Replace("host ", "");
			strCmd.TrimLeft();

			CString strTmp;
			DWORD  dwIndex = 0;
			
			dwIndex = strCmd.Find(" ", 0);
			if(dwIndex > 0){


				strTmp = strCmd.Left(dwIndex);
				m_dwIP = TranslateIP(strTmp);
            }

			strCmd.Replace(strTmp, "");
			strCmd.TrimLeft();

			dwIndex = strCmd.Find(" ", 0);
			if(dwIndex > 0){

				strTmp = strCmd.Left(dwIndex);
				m_wPort = (WORD)atoi((LPTSTR)(LPCTSTR)strTmp);
			}

			strCmd.Replace(strTmp + " ", "");
			m_strRoomBase = strCmd;
			
			if(m_dwIP == 0 || m_wPort < 1024 || m_strRoomBase.IsEmpty()){

				WriteText(IDS_ERROR_SERVER_SYNTAXERROR);
				return;
			}

			m_strRoomFull.Format("%s_%08X%04X", m_strRoomBase, m_dwIP, m_wPort);
			
			m_strTopic = g_sSettings.GetTopic();
			m_strMotd  = g_sSettings.GetMotd();
			m_strMotd.Replace("\\n", "\n");
			if(g_sSettings.GetModerated()){

				m_uMode	   = CM_MODERATED;
			}
			m_dwLimit = g_sSettings.GetLimit();
			
			m_eHosted.ResetEvent();
			m_bHosted = FALSE;
			m_pServerThread = AfxBeginThread(CServerView::ServerThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
			WaitForSingleObject(m_eHosted, INFINITE);
			m_pNotifyThread = AfxBeginThread(CServerView::NotifyThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);

			if(m_bHosted){

				CString strMsg;
				strMsg.Format(IDS_SERVER_SUCCESS, m_strRoomFull);
				WriteText(strMsg);
				GetDocument()->SetTitle(m_strRoomFull + " [hosted]");
				((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->GetTitle());
			}
			else{

				CString strMsg;
				strMsg.Format(IDS_ERROR_SERVER_HOST, m_strRoomFull);
				WriteText(strMsg);
				GetDocument()->SetTitle(m_strRoomFull + " [error]");
				((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->GetTitle());
			}
		}
	}
	else if((strCmd.Find("channelname") == 0) && m_bHosted){

		CString strOldName, strNewName;
		CTokenizer token(strCmd, " ");
		token.Next(strNewName);
		token.Next(strNewName);
		if(strNewName.IsEmpty()){

			WriteText(IDS_ERROR_SERVER_SYNTAXERROR);
			return;
		}
		strOldName = m_strRoomBase;
		m_strRoomFull.Replace(m_strRoomBase, strNewName);
		m_strRoomBase = strNewName;
		CString strMsg;
		strMsg.Format(IDS_SERVER_ROOMRENAME, strOldName, strNewName, m_strRoomFull);
		WriteText(strMsg);
		SendChannelRename();
	}
	else if((strCmd.Find("/say ", 0) == 0) && m_bHosted){

		CString strMsg = strCmd.Mid(5);
		if(strMsg.Find("/me ", 0) == 0){

			strMsg = strMsg.Mid(4);
			SendAction(g_sSettings.GetGodName(), strMsg);
		}
		else{

			SendMsg(g_sSettings.GetGodName(), strMsg);
		}                            		

	}
	else if((strCmd.Find("/impose ", 0) == 0) && m_bHosted){

		CTokenizer token(strCmd, " ");
		CString strName, strMsg, strTmp;
		token.Next(strTmp);

		if(!token.Next(strName)){

			WriteText(IDS_ERROR_SERVER_SYNTAXERROR);
			return;
		}
		strMsg = token.Tail();
		if(strMsg.Find("/me ", 0) == 0){

			strMsg = strMsg.Mid(4);
			SendAction(strName, strMsg);
		}
		else{

			SendMsg(strName, strMsg);
		}                            		
	}
	else if((strCmd.Find("/", 0) == 0) && m_bHosted){


		if(ExecuteChannelCommand(g_sSettings.GetGodName(), strCmd)){

			WriteText(IDS_COMMAND_EXECUTED, strCmd);
		}
		else{

			WriteText(IDS_COMMAND_NOTUNDERSTOOD, strCmd);
		}
	}
	else if(strCmd.CompareNoCase("stop") == 0){

		if(m_bHosted){


			Stop();
			CString strMsg;
			strMsg.Format(IDS_SERVER_TERMINATED, m_strRoomFull);
			WriteText(strMsg);
			GetDocument()->SetTitle(m_strRoomFull + " [stopped]");
			((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->GetTitle());
		}
		else{

			WriteText(IDS_ERROR_SERVER_NOROOM);
		}
	}
	else{

		PrintCmdNotUnderstood(strCmd);
	}
}

void CServerView::PrintCmdNotUnderstood(const CString strCmd)
{

	if(strCmd.IsEmpty()){

		WriteText(IDS_ERROR_CMDEMPTY);
	}
	else{

		CString strError;
		strError.Format(IDS_ERROR_NOTUNDERSTOOD, strCmd);
		WriteText(strError);
	}
}

void CServerView::PrintHelp(void)
{

	WriteText(IDS_SERVER_HELP);
}

UINT CServerView::ServerThread(LPVOID pParam)
{

	CServerView* pServer = (CServerView*)pParam;
	ASSERT(pServer);


	if(pServer->m_mServer.Listen(pServer->m_wPort) == 0){

		TRACE("Error creating socket\n");
		pServer->m_eHosted.SetEvent();
		pServer->m_bHosted = FALSE;
		return 0;
	}

	pServer->m_bHosted = TRUE;
	pServer->m_eHosted.SetEvent();
	pServer->m_eDone.ResetEvent();

	pServer->m_sIn = INVALID_SOCKET;

	while(pServer->m_bHosted){


		pServer->m_sIn = pServer->m_mServer.Accept();
		if(pServer->m_aClients.GetSize() >= (int)pServer->m_dwLimit){

			continue;
		}
		if(pServer->m_sIn != INVALID_SOCKET){

			AfxBeginThread(LoginThread, (LPVOID)pServer, THREAD_PRIORITY_NORMAL);
		}
		else
			break;
	}

	pServer->m_bHosted = FALSE;
	pServer->m_eDone.SetEvent();
	TRACE("Leaving ServerThread\n");
	return 0;
}


UINT CServerView::LoginThread(LPVOID pParam)
{

	CServerView* pServer = (CServerView*)pParam;
	ASSERT(pServer);

	CClientSocket* mClient = new CClientSocket(pServer->m_sIn);
	

	if(!mClient->HandShake(pServer->m_strRoomBase)){

		// Login failed
		//pServer->WriteText(IDS_ERROR_SERVER_HANDSHAKE);
		delete mClient;
		mClient = 0;
		return 0;
	}
	
	if(g_sSettings.GetBlockNushi() && (mClient->m_wClientType != CLIENT_WINMX353)){
	
		mClient->LogOut();
		delete mClient;
		mClient = 0;
		return 0;
	}

	if(!pServer->CheckUserName(mClient->m_strName, mClient->m_strSrcHost, mClient->m_dwSrcIP)){

		// Client has illegal name
		mClient->LogOut();
		delete mClient;
		mClient = 0;
		return 0;
	}

	if(!mClient->HighlevelHandshake(pServer->m_strKeyword)){

		// Login failed
		pServer->WriteText(IDS_ERROR_SERVER_HANDSHAKE, mClient->m_strName, Util::FormatIP(mClient->m_dwSrcIP), mClient->m_strSrcHost);
		delete mClient;
		mClient = 0;
		return 0;
	}
	
	mClient->SetUserMode(UM_NORMAL);
	/*if(!(pServer->m_uMode & CM_MODERATED)){

		mClient->AddMode(UM_SPEAKPERMIT);
	}*/
	if(g_sSettings.GetLocalIsOp() && (mClient->m_dwSrcIP == 16777343)){

		mClient->AddMode(UM_OPERATOR);
	}

	mClient->StartUp();

	CString strSend = pServer->m_strTopic;
	Util::ReplaceVars(strSend);
	strSend.Replace("%USERS%", Util::FormatInt(pServer->m_aClients.GetSize()));
	strSend.Replace("%NAME%", mClient->m_strName);
	strSend.Replace("%IP%", Util::FormatIP(mClient->m_dwSrcIP));
	strSend.Replace("%HOSTNAME%", mClient->m_strSrcHost);
	mClient->SendTopic(strSend);

	pServer->CheckClients();

	if(g_sSettings.GetGodVisible()){

		mClient->SendUserlist(g_sSettings.GetGodName(), 0, 0, 
							  g_sSettings.GetGodLine(),
							  g_sSettings.GetGodFiles(), 1);
	}

	for(int i = 0; i < pServer->m_aClients.GetSize(); i++){

		mClient->SendUserlist(pServer->m_aClients[i]->m_strName, 
							  pServer->m_aClients[i]->m_dwIP, 
							  pServer->m_aClients[i]->m_wPort, 
			                  pServer->m_aClients[i]->m_wLineType,
							  pServer->m_aClients[i]->m_dwFiles,
							  pServer->m_aClients[i]->m_uMode & UM_OPERATOR ? 1 : 0);

	}
	
	strSend = pServer->m_strMotd;
	Util::ReplaceVars(strSend);
	strSend.Replace("%USERS%", Util::FormatInt(pServer->m_aClients.GetSize()));
	strSend.Replace("%NAME%", mClient->m_strName);
	strSend.Replace("%IP%", Util::FormatIP(mClient->m_dwSrcIP));
	strSend.Replace("%HOSTNAME%", mClient->m_strSrcHost);
	mClient->SendMotd(strSend);

	WORD wPos = pServer->m_aClients.Add(mClient);
	

	CoCreateGuid(&pServer->m_aClients[wPos]->m_guID);
	pServer->m_aClients[wPos]->m_hMsgTarget = pServer->m_hWnd;

	pServer->SendJoin(mClient->m_strName, mClient->m_dwIP, mClient->m_wPort, mClient->m_wLineType, mClient->m_dwFiles, (mClient->m_uMode & UM_OPERATOR ? 1 : 0), mClient->m_dwSrcIP);
	TRACE("Login Complete\n");
	return 1;
}

DWORD CServerView::TranslateIP(CString strIP)
{
	CString strTmp;

	CTokenizer ipToken(strIP, ".");
	ipToken.Next(strTmp);
	WORD wA = (WORD)atoi(strTmp);
	ipToken.Next(strTmp);
	WORD wB = (WORD)atoi(strTmp);
	ipToken.Next(strTmp);
	WORD wC = (WORD)atoi(strTmp);
	ipToken.Next(strTmp);
	WORD wD = (WORD)atoi(strTmp);
	strTmp.Format("%02X%02X%02X%02X", wD, wC, wB, wA);
	
	DWORD dwIP = Util::axtoi((LPTSTR)(LPCTSTR)strTmp, 8);

	return dwIP;
}

void CServerView::CheckClients(void)
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(!m_aClients[i]->m_bListen){

			CClientSocket *pTmp = m_aClients[i];
			m_aClients.RemoveAt(i);
			pTmp->LogOut(); // just to be on the save side....
			SendPart(pTmp->m_strName, pTmp->m_dwIP, pTmp->m_wPort);
			delete pTmp;
			pTmp = NULL;
			i--;
		}
	}
}

void CServerView::SetTopic(const CString strTopic)
{

	m_strTopic = strTopic;
}

void CServerView::SetMotd(const CString strMotd)
{

	m_strMotd = strMotd;
}

// Broadcast Send
void CServerView::SendTopic()
{

	CString strTopic, strSend;
	strTopic.LoadString(IDS_TOPIC2);
	EchoChat(strTopic, m_strTopic);

	for(int i = 0; i < m_aClients.GetSize(); i++){

		strSend = m_strTopic;
		Util::ReplaceVars(strSend);
		strSend.Replace("%USERS%", Util::FormatInt(m_aClients.GetSize()));
		strSend.Replace("%NAME%", m_aClients[i]->m_strName);
		strSend.Replace("%IP%", Util::FormatIP(m_aClients[i]->m_dwSrcIP));
		strSend.Replace("%HOSTNAME%", m_aClients[i]->m_strSrcHost);
		m_aClients[i]->SendTopic(strSend);
	}
}

void CServerView::SendMotd()
{

	m_strMotd.Replace("\\n", "\n");
	
	EchoChat("Motd", m_strMotd);
	
	CString strSend;
	for(int i = 0; i < m_aClients.GetSize(); i++){

		strSend = m_strMotd;
		Util::ReplaceVars(strSend);
		strSend.Replace("%USERS%", Util::FormatInt(m_aClients.GetSize()));
		strSend.Replace("%NAME%", m_aClients[i]->m_strName);
		strSend.Replace("%IP%", Util::FormatIP(m_aClients[i]->m_dwSrcIP));
		strSend.Replace("%HOSTNAME%", m_aClients[i]->m_strSrcHost);
		m_aClients[i]->SendMotd(strSend);
	}
}

void CServerView::SendMsg(CString strUser, CString strMsg)
{

	if(CheckCommands(strUser, strMsg)){

		// do not relay the message when it was a /mode command
		return;
	}

	FixString(strMsg);
	EchoChat(strUser, strMsg);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendMsg(strUser, strMsg);
	}
}

void CServerView::SendChannelRename(void)
{
    
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendChannelRename(m_strRoomBase);
	}
}

void CServerView::SendOpMsg(CString strUser, CString strMsg)
{

	FixString(strMsg);
	BOOL bEcho = TRUE;
	if(!strMsg.IsEmpty() && !strUser.IsEmpty()){

		strUser.Insert(0, '<');
		strUser.Append("> ");
		bEcho = FALSE;
	}

	EchoChat(strUser, strMsg);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendOperator(strUser, strMsg, bEcho);
	}
}

void CServerView::SendAction(CString strUser, CString strMsg)
{

	FixString(strMsg);
	EchoChat(strUser, strMsg);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendAction(strUser, strMsg);
	}
}

void CServerView::SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP)
{

	EchoChat("+", strUser);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendJoin(strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	}
}

void CServerView::SendPart(const CString strUser, DWORD dwIP, WORD wPort)
{

	EchoChat("-", strUser);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendPart(strUser, dwIP, wPort);
	}
}

void CServerView::SendRename(DWORD dwSrcIP, WORD wSrcPort, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	// Name already exists, so kick him
	if(!CheckUserName(strNewName, dwSrcIP, wSrcPort)){

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if((m_aClients[i]->m_dwSrcIP == dwSrcIP) &&
				(m_aClients[i]->m_wSrcPort == wSrcPort)){

					CClientSocket *pTmp = m_aClients[i];
					m_aClients.RemoveAt(i);
					pTmp->LogOut();
					delete pTmp;

					SendPart(strOldName, dwOldIP, wOldPort); 

					CString strWarn;
					strWarn.Format(IDS_WARN_USERNAMEHIJACK, strOldName, Util::FormatIP(dwSrcIP));
					SendOpMsg("", strWarn);
					return;
				}
		}
	}

	EchoChat(strOldName + " -> ", " <- " + strNewName);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendRename(strOldName, dwOldIP, wOldPort, strNewName, dwNewIP, wNewPort, wLine, dwFiles, wUserLevel);
	}
}

void CServerView::SendMode(const CString strSender, const CString strMode)
{

	CString strModeSend;
	strModeSend.Format(IDS_SERVER_SETMODE, strMode);

	ServerAction(strSender, strModeSend);
}

void CServerView::ServerAction(const CString strSender, const CString strMsg)
{

	ClientOpMsg* pN = new ClientOpMsg();
	pN->dwIP = 0;
	pN->wPort = 0;
	pN->uMode = UM_OPERATOR;
	pN->strName = strSender;
	pN->strText = strMsg;
	SendMessage(UWM_CLNNOTIFY, (WPARAM)pN->wType, (LPARAM)pN);
}

UINT CServerView::NotifyThread(LPVOID pParam)
{

	CServerView* pServer = (CServerView*)pParam;
	ASSERT(pServer);

	pServer->m_eNotifyDone.ResetEvent();

	int n = -1;

	while(pServer->m_bHosted){

		if(pServer->m_qNotifies.empty()){

            Sleep(50);
			continue;
		}

		ClientNotify* pN = pServer->m_qNotifies.front();
		pServer->m_qNotifies.pop();

		switch(pN->wType){
		
			case CC_MSG:
				if(pServer->HasSpeakPermission(pN->uMode))
					pServer->SendMsg(((ClientMessage*)pN)->strName, ((ClientMessage*)pN)->strText);
				else
					pServer->SendNoVoice(pN->guid);
				break;
			case CC_ACTION:
				if(pServer->HasSpeakPermission(pN->uMode))
					pServer->SendAction(((ClientAction*)pN)->strName, ((ClientAction*)pN)->strText);
				else
					pServer->SendNoVoice(pN->guid);
				break;
			case CC_OPMSG:
				if(pServer->HasSpeakPermission(pN->uMode))
					pServer->SendOpMsg(((ClientOpMsg*)pN)->strName, ((ClientAction*)pN)->strText);
				else
					pServer->SendNoVoice(pN->guid);
				break;
			case CC_RENAME:
				pServer->SendRename(((ClientRename*)pN)->dwIP, ((ClientRename*)pN)->wPort,
									((ClientRename*)pN)->strOldName, ((ClientRename*)pN)->dwOldIP, ((ClientRename*)pN)->wOldPort,
									((ClientRename*)pN)->strNewName, ((ClientRename*)pN)->dwNewIP, ((ClientRename*)pN)->wNewPort,
									((ClientRename*)pN)->wNewLine, ((ClientRename*)pN)->dwNewFiles, ((ClientRename*)pN)->wUserLevel);
				break;
			case CC_ERROR:

				n = pServer->GetByID(((ClientError*)pN)->guID);
				if((n < pServer->m_aClients.GetSize()) && (n != -1)){

					CClientSocket* pTmp = pServer->m_aClients[n];
					pServer->m_aClients.RemoveAt(n, 1);
					pServer->SendPart(pTmp->m_strName, pTmp->m_dwIP, pTmp->m_wPort);
					pServer->SendOpMsg(pTmp->m_strName, ((ClientError*)pN)->strCause);
					delete pTmp;
					pTmp = NULL;
				}
				break;
			default:
				ASSERT(FALSE);
		}


		// end
		delete pN;
		pN = NULL;

		pServer->CheckClients();
	}

	TRACE("Leaving Notify thread\n");
	pServer->m_eNotifyDone.SetEvent();
	return 0;
}

LRESULT CServerView::ClientCallback(WPARAM wParam, LPARAM lParam)
{

	EnterCriticalSection(&m_csLock);
	
	ClientNotify* pNotify = (ClientNotify*)lParam;
	ASSERT(pNotify);

	m_qNotifies.push(pNotify);

	LeaveCriticalSection(&m_csLock);
	return 1;
}

BOOL CServerView::CheckUserName(const CString strName, CString strHost, DWORD dwIP)
{

	if(strName.IsEmpty())		return FALSE; // username may not be empty
	if(strName[0] == '@')		return FALSE; // username may not start with operator prefix
	if(strName.Find(" ") >= 0)	return FALSE; // username may not contain a space
	if(strName.Find("/mode ") >= 0) return FALSE; // username may not contain /mode
	if(strName.Find("\n") >= 0) return FALSE;
	if(strName.Find("\r") >= 0) return FALSE;
	if(strName.Find("\t") >= 0) return FALSE;
    if(strName == g_sSettings.GetGodName()) return FALSE;

	// check bans
	for(int i = 0; i < m_aBans.GetSize(); i++){

		if(m_aBans[i].strName == strName){

			return FALSE;
		}
		if(m_aBans[i].strHost == strHost){

			return FALSE;
		}
		if(m_aBans[i].strIP == Util::FormatIP(dwIP)){

			return FALSE;
		}
	}

	for(i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_strName == strName){

			return FALSE;
		}
		if(!g_sSettings.GetMultiIPOk() && (m_aClients[i]->m_strSrcHost == strHost)){

			return FALSE;
		}
		if(!g_sSettings.GetMultiIPOk() && (m_aClients[i]->m_dwSrcIP == dwIP)){

			return FALSE;
		}
	}

	return TRUE;
}

BOOL CServerView::CheckUserName(const CString strName, DWORD dwIP, WORD wPort)
{
	
	if(strName.IsEmpty()) return FALSE; // username may not be empty
	if(strName[0] == '@') return FALSE; // username may not start with operator prefix
	if(strName.Find(" ") >= 0) return FALSE; // username may not contain a space
	if(strName.Find("/mode ") >= 0) return FALSE; // username may not contain /mode
	if(strName.Find("\n") >= 0) return FALSE;
	if(strName.Find("\r") >= 0) return FALSE;
	if(strName.Find("\t") >= 0) return FALSE;
    if(strName == g_sSettings.GetGodName()) return FALSE;

	// check if user already exists.
	if(dwIP == 0){

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName == strName){

				return FALSE;
			}
		}
	}
	else{

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if((m_aClients[i]->m_strName == strName) &&	(m_aClients[i]->m_dwSrcIP != dwIP) && (m_aClients[i]->m_wSrcPort != wPort))
			{
                		
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CServerView::CheckCommands(const CString strUser, const CString strMsg)
{

	if(strMsg.IsEmpty()) return FALSE;
	if(strMsg.Find("/", 0) != 0)  return FALSE;

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_strName == strUser){
			
			if(m_aClients[i]->GetUserMode() & UM_OPERATOR){

				break;
			}
			else{
				
				return FALSE;
			}
		} // == strUser
	} // for

	if(i >= m_aClients.GetSize()) return FALSE;

	// We are authorized
	
	return ExecuteChannelCommand(strUser, strMsg);
}


BOOL CServerView::ExecuteChannelCommand(const CString strUser, const CString strMsg)
{


	if(strMsg == "/mode"){

		// if it is exactly /mode print all the channelmodes
		CString strModes;
		if(m_uMode & CM_OPTOPIC) strModes += "t";
		if(m_uMode & CM_MODERATED) strModes += "m";
		if(m_uMode & CM_KEYWORD) strModes += "k " + m_strKeyword;
		if(!strModes.IsEmpty()) strModes.Insert(0, "+");

		CString strTmp;
		strTmp.LoadString(IDS_MODES);
		ServerAction(strTmp, strModes);
		return TRUE;
	}

	if(strMsg == "/listbans"){

		CString strMsg;
		ServerAction(strUser, "/listbans");
		strMsg.LoadString(IDS_BANLISTINGSTART);
		ServerAction("", strMsg);
		for(int i = 0; i < m_aBans.GetSize(); i++){

			strMsg.Format(IDS_BANLISTING, m_aBans[i].strName, m_aBans[i].strIP, m_aBans[i].strHost);
			ServerAction("", strMsg);
		}
		strMsg.LoadString(IDS_BANLISTINGEND);
		ServerAction("", strMsg);
		return TRUE;
	}

	CTokenizer token(strMsg, " ");
	CString strAddRem, strMode, strParam, strCmd;

	if(!token.Next(strCmd)) return FALSE; // first ist /mode.
	if(!token.Next(strAddRem)) return FALSE; // note this is <param> for topic motd, kick, muzzle

	CString strTmp;
	while(token.Next(strTmp)){

		strParam += strTmp + " ";
	}
	
	if(strCmd == "/topic"){

		strParam = strMsg;
		strParam.Replace("/topic ", "");
		SetTopic(strParam);
		SendTopic();
		return TRUE;
	}
	else if(strCmd == "/motd"){

		strParam = strMsg;
		strParam.Replace("/motd ", "");
		SetMotd(strParam);
		SendMotd();
		return TRUE;
	}
	else if(strCmd == "/kick"){

		if(strAddRem == strUser) return FALSE; // user may not kick himself

		CClientSocket *pKick = NULL;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName == strAddRem){

				pKick = m_aClients[i];
				m_aClients.RemoveAt(i);
				break;
			}
		}
		if(pKick > NULL){

			pKick->LogOut();
			CString strKick;
			strKick.Format(IDS_SERVER_KICKED, strUser, strParam);
			ServerAction(strAddRem, strKick);
			SendPart(pKick->m_strName, pKick->m_dwIP, pKick->m_wPort);
			delete pKick;
		}
		return TRUE;
	}
	else if(strCmd != "/mode"){

		return FALSE;
	}
		
	if(strAddRem.GetLength() != 2){
	
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName == strAddRem){

				CString strModes, strOut;
				UINT uMode = m_aClients[i]->m_uMode;
				if(uMode & UM_SPEAKPERMIT) strModes += "v";
				if(uMode & UM_OPERATOR) strModes += "o";
				if(uMode & UM_BANNED) strModes += "b";
				if(!strModes.IsEmpty()) strModes.Insert(0, "+");

				strOut.Format(IDS_USERMODES, m_aClients[i]->m_strName, strModes);
				ServerAction(strUser, strOut);
				return TRUE;
			}

		}
		return FALSE;
	}


	strMode = strAddRem.Mid(1, 1);
	strAddRem = strAddRem.Left(1);
	strParam.TrimRight();
	if(strAddRem == "+"){

		// add channel mode
		if(strMode == "t"){  // topic op-only

			m_uMode |= CM_OPTOPIC;
			SendMode(strUser, "+t");
		}
		else if(strMode == "k"){ // keyword

			if(m_uMode & CM_KEYWORD){

				// Keyword already set
				CString strMsg;
				strMsg.LoadString(IDS_ERROR_SERVER_KEYEXISTS);
				ServerAction("", strMsg);
			}
			else{

				m_uMode |= CM_KEYWORD;
				m_strKeyword = strParam;
				SendMode(strUser, "+k " + strParam);
			}

		}
		else if(strMode == "m"){ // moderated

			m_uMode |= CM_MODERATED;
			SendMode(strUser, "+m");
		}
		else if(strMode == "v"){ // speak permission

			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strParam){

					m_aClients[i]->AddMode(UM_SPEAKPERMIT);
					SendMode(strUser, "+v " + strParam);
					break;
				}
			}
		}
		else if(strMode == "o"){ // operator status

			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strParam){

					m_aClients[i]->AddMode(UM_OPERATOR);
					SendMode(strUser, "+o " + strParam);
					break;
				}
			}
		}
		else if(strMode == "l"){ // limit

			m_dwLimit = atoi((LPTSTR)(LPCTSTR)strParam);
			SendMode(strUser, "+l " + strParam);
		}
		else if(strMode == "b"){ // ban

			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strParam){

					BAN ban;
					ban.strName = m_aClients[i]->m_strName;
					ban.strIP   = Util::FormatIP(m_aClients[i]->m_dwSrcIP);
					ban.strHost = m_aClients[i]->m_strSrcHost;
    				m_aBans.Add(ban);

					m_aClients[i]->AddMode(UM_BANNED);
					SendMode(strUser, "+b " + strParam + " (" + ban.strHost + ")");
					break;
				}
			}
		}
	} // end if +
	else if(strAddRem == "-"){

		// remove channel mode
		if(strMode == "t"){  // topic op-only

			m_uMode &= ~CM_OPTOPIC;
			SendMode(strUser, "-t");
		}
		else if(strMode == "k"){ // keyword

			if(m_uMode | CM_KEYWORD){

				if(m_strKeyword.Compare(strParam) == 0){

					m_uMode &= ~CM_KEYWORD;
					m_strKeyword.Empty();
					SendMode(strUser, "-k ");

				}
				else{

					CString strMsg;
					strMsg.LoadString(IDS_ERROR_SERVER_WRONGKEY);
					ServerAction("", strMsg);
				}
			}
			else{

				CString strMsg;
				strMsg.LoadString(IDS_ERROR_SERVER_NOKEY);
				ServerAction("", strMsg);
			}

		}
		else if(strMode == "m"){ // moderated

			m_uMode &= ~CM_MODERATED;
			for(int i = 0; i < m_aClients.GetSize(); i++){

				m_aClients[i]->AddMode(UM_SPEAKPERMIT);
			}
			SendMode(strUser, "-m");
		}
		else if(strMode == "v"){ // speak permission

			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strParam){

					m_aClients[i]->RemoveMode(UM_SPEAKPERMIT);
					TRACE("%X", m_aClients[i]->GetUserMode());
					SendMode(strUser, "-v " + strParam);
					break;
				}
			}
		}
		else if(strMode == "o"){ // operator status

			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strParam){

					m_aClients[i]->RemoveMode(UM_OPERATOR);
					SendMode(strUser, "-o " + strParam);
					break;
				}
			}
		}
		else if(strMode == "l"){ // limit

			m_dwLimit = 1000;
			SendMode(strUser, "-l");
		}
		else if(strMode == "b"){ // ban

			CString strMsg;
			int nBans = m_aBans.GetSize();
			for(int i = 0; i < nBans; i++){

				if((m_aBans[i].strName == strParam) || (m_aBans[i].strHost == strParam) || (m_aBans[i].strIP == strParam)){

					strMsg.Format(IDS_SERVER_UNBAN, m_aBans[i].strName, m_aBans[i].strIP, m_aBans[i].strHost);
					m_aBans.RemoveAt(i, 1);
                    ServerAction(strUser, strMsg);
					break;
				}

			}
			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strParam){

					m_aClients[i]->RemoveMode(UM_BANNED);
				}
			}
			if(i >= nBans){

				strMsg.Format(IDS_NOSUCHBAN, strParam);
				ServerAction("", strMsg);
			}
		}
	}
	
	return TRUE;
}

void CServerView::FixString(CString& strFix)
{
	strFix.Replace("\n", " ");
	strFix.Replace("\r", " ");
	strFix.Replace("\t", " ");
	strFix.Replace("{\rtf", "####");
}

int CServerView::GetByID(GUID guid)
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_guID == guid){

			return i;
		}
	}
	
	return -1;
}


BOOL CServerView::HasSpeakPermission(UINT uMode)
{

	if((m_uMode & CM_MODERATED) == CM_MODERATED){

		return ((uMode & UM_SPEAKPERMIT) == UM_SPEAKPERMIT) || ((uMode & UM_OPERATOR) == UM_OPERATOR);
	}
	else{

		return TRUE;
	}
}

void CServerView::SendNoVoice(GUID guid)
{

	int n = GetByID(guid);
	if(n >= 0){

		CString strMsg;
		strMsg.LoadString(IDS_NOVOICE);
		m_aClients[n]->SendMsg(g_sSettings.GetGodName(), strMsg);
	}
}
