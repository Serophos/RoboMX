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
// CServerView

UINT UWM_INCOMMING = ::RegisterWindowMessage("UWM_INCOMMING-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
UINT UWM_CLNNOTIFY = ::RegisterWindowMessage("UWM_CLNNOTIFY-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");

IMPLEMENT_DYNCREATE(CServerView, CRichEditView)

extern CSettings g_sSettings;
extern UINT UWM_LOAD_SETTINGS;

CServerView::CServerView()
{

	m_bHosted = FALSE;
	m_eDone.SetEvent();
	m_eHosted.ResetEvent();
	m_eNotifyDone.SetEvent();
	m_strRoomBase = "";
	m_strRoomFull = "";
	m_dwIP        = 0;
	m_wPort       = 0;
	m_wLimit      = 40;
	m_strTopic    = "";
	m_strMotd     = "";
	m_uMode		  = CM_NORMAL;
}

CServerView::~CServerView()
{
}

BEGIN_MESSAGE_MAP(CServerView, CRichEditView)
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(UWM_CLNNOTIFY, ClientCallback)
	ON_REGISTERED_MESSAGE(UWM_LOAD_SETTINGS, LoadSettings)
END_MESSAGE_MAP()


// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG


// CServerView message handlers


void CServerView::OnInitialUpdate()
{

	CRichEditView::OnInitialUpdate();
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.AddButton( this, IDR_SERVER );
	
	LoadSettings(0,0);

	WriteText("RoboMX Roomserver v0.1\nType help for available commands...\n");
	WriteText("> ");

	InitializeCriticalSection(&m_csLock);
}

LRESULT CServerView::LoadSettings(WPARAM wParam, LPARAM lParam)
{

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = g_sSettings.GetRGBNormalMsg();

	GetRichEditCtrl().SetBackgroundColor(FALSE, g_sSettings.GetRGBBg());

	GetRichEditCtrl().SetDefaultCharFormat(cf);
	GetRichEditCtrl().SetSelectionCharFormat(cf);

	return 1;
}

void CServerView::OnDestroy()
{
	
	((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.RemoveButton(this);

	if(m_bHosted){

		Stop();
	}
	DeleteCriticalSection(&m_csLock);
	CRichEditView::OnDestroy();
}

void CServerView::Stop(void)
{

	m_bHosted = FALSE;
	m_mServer.Close();

	TRACE("Waiting for Worker threads to exit\n");	
	WaitForSingleObject(m_eDone, INFINITE);
	WaitForSingleObject(m_eNotifyDone, INFINITE);
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
}

void CServerView::WriteText(LPCTSTR lpszText)
{

	int nLen = GetRichEditCtrl().GetTextLength();
	GetRichEditCtrl().SetSel(nLen, nLen);
	GetRichEditCtrl().ReplaceSel(lpszText, FALSE);
}

BOOL CServerView::PreTranslateMessage(MSG* pMsg)
{

	if(pMsg->message == WM_KEYDOWN){

		if(pMsg->wParam == VK_RETURN){

			WriteText("\n");
			int n = GetRichEditCtrl().GetLineCount();
			char pszLine[256];
			GetRichEditCtrl().GetLine(n-2, (LPTSTR)&pszLine, 256);
			HandleCommand(pszLine);
			return 1;
		}
		else if(pMsg->wParam == VK_BACK){

			int n = GetRichEditCtrl().GetLineCount();
			char pszLine[256];
			GetRichEditCtrl().GetLine(n-1, (LPTSTR)&pszLine, 256);
			if(strlen((const char*)&pszLine) <= 3){

				return 1;
			}
		}
	}
	return 0;
}

void CServerView::HandleCommand(CString strCmd)
{

	strCmd = strCmd.Mid(2);
	strCmd.TrimLeft();
	strCmd.TrimRight();

	if(strCmd.CompareNoCase("help") == 0){

		PrintHelp();
	}
	else if(strCmd.Find("host") == 0){

		if(m_bHosted){

			WriteText("You can only host one room per console.\n");
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

				WriteText("Syntax Error or one Parameter is invalid. Correct call is:\nhost <IP> <PORT> <ROOM>\n");
				WriteText("> ");
				return;
			}

			m_strRoomFull.Format("%s_%08X%04X", m_strRoomBase, m_dwIP, m_wPort);
			

			m_eHosted.ResetEvent();
			m_bHosted = FALSE;
			AfxBeginThread(CServerView::ServerThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
			WaitForSingleObject(m_eHosted, INFINITE);
			AfxBeginThread(CServerView::NotifyThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);

			if(m_bHosted){

				CString strMsg;
				strMsg.Format("Room '%s' was successfully hosted\n", m_strRoomFull);
				WriteText(strMsg);
				GetDocument()->SetTitle(m_strRoomFull + " [hosted]");
				((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->GetTitle());
			}
			else{

				CString strMsg;
				strMsg.Format("Error: Could not host room'%s'\n", m_strRoomFull);
				WriteText(strMsg);
				GetDocument()->SetTitle(m_strRoomFull + " [error]");
				((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->GetTitle());
			}
		}
	}
	else if((strCmd.Find("/", 0) == 0) && m_bHosted){


		if(ExecuteChannelCommand("Master", strCmd)){

			WriteText("Command '" + strCmd + "' was executed.\n");
		}
		else{

			WriteText("Command '" + strCmd + "' not understood. Type Help for available commands\n\n");
		}
	}
	else if(strCmd.CompareNoCase("stop") == 0){

		if(m_bHosted){


			Stop();
			CString strMsg;
			strMsg.Format("Room '%s' was terminated\n", m_strRoomFull);
			WriteText(strMsg);
			GetDocument()->SetTitle(m_strRoomFull + " [stopped]");
			((CMainFrame*)AfxGetMainWnd())->m_wndDocSelector.UpdateTitle(this, GetDocument()->GetTitle());
		}
		else{

			WriteText("No room to stop.\n");
		}
	}
	else{

		PrintCmdNotUnderstood(strCmd);
	}
	WriteText("> ");
}

void CServerView::PrintCmdNotUnderstood(const CString strCmd)
{

	if(strCmd.IsEmpty()){

		WriteText("Empty command string. Type Help for available commands\n");
	}
	else{

		CString strError;
		strError.Format(" Command '%s' not understood. Type Help for available commands\n", strCmd);
		WriteText(strError);
	}
}

void CServerView::PrintHelp(void)
{

	WriteText("\nAvailable Commands:\n");
	WriteText("host <IP> <PORT> <ROOM> \t- Host a new room\n");
	WriteText("stop \t\t\t\t- Stop currently hosted room\n");
	WriteText("/mode {#channel|nickname} [[+|-]modechars [parameters]] - Sets channel or user modes.\n");
	WriteText("/topic [topic] - Set channel topic\n");
	WriteText("/motd [motd] - Set channel motd\n");
	WriteText("/kick [user] [reason] - Kick a user\n");
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
		if(pServer->m_aClients.GetSize() >= pServer->m_wLimit){

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
	

	if(!mClient->HandShake(pServer->m_strRoomFull)){

		// Login failed
		pServer->WriteText("\nHandshake with incoming client failed\n> ");
		delete mClient;
		mClient = 0;
		return 0;
	}
	
	if(!pServer->CheckUserName(mClient->m_strName)){

		// Client has illegal name
		mClient->LogOut();
		delete mClient;
		mClient = 0;
		return 0;
	}

	if(!mClient->HighlevelHandshake(pServer->m_strKeyword)){

		// Login failed
		pServer->WriteText("\nHandshake with incoming client failed\n> ");
		delete mClient;
		mClient = 0;
		return 0;
	}
	
	mClient->SetUserMode(UM_NORMAL);
	if(!(pServer->m_uMode & CM_MODERATED)){

		mClient->AddMode(UM_SPEAKPERMIT);
	}
	mClient->StartUp();
	mClient->SendTopic(pServer->m_strTopic);
	pServer->CheckClients();

	for(int i = 0; i < pServer->m_aClients.GetSize(); i++){

		mClient->SendUserlist(pServer->m_aClients[i]->m_strName, 
							  pServer->m_aClients[i]->m_dwIP, 
							  pServer->m_aClients[i]->m_wPort, 
			                  pServer->m_aClients[i]->m_wLineType,
							  pServer->m_aClients[i]->m_dwFiles,
							  pServer->m_aClients[i]->m_uMode & UM_OPERATOR ? 1 : 0);

	}
	
	mClient->SendMotd(pServer->m_strMotd);

	WORD wPos = pServer->m_aClients.Add(mClient);
	pServer->m_aClients[wPos]->m_wClientID = wPos;
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

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendTopic(m_strTopic);
	}
}

void CServerView::SendMotd()
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendMotd(m_strMotd);
	}
}

void CServerView::SendMsg(CString strUser, CString strMsg)
{

	if(CheckCommands(strUser, strMsg)){

		// do not relay the message when it was a /mode command
		return;
	}

	FixString(strMsg);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendMsg(strUser, strMsg);
	}
}

void CServerView::SendAction(CString strUser, CString strMsg)
{

	FixString(strMsg);
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendAction(strUser, strMsg);
	}
}

void CServerView::SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP)
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendJoin(strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	}
}

void CServerView::SendPart(const CString strUser, DWORD dwIP, WORD wPort)
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendPart(strUser, dwIP, wPort);
	}
}

void CServerView::SendRename(DWORD dwSrcIP, WORD wSrcPort, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles)
{

	// Name already exists, so kick him
	if(!CheckUserName(strNewName, dwSrcIP, wSrcPort)){

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if((m_aClients[i]->m_dwSrcIP == dwSrcIP) &&
				(m_aClients[i]->m_wSrcPort == wSrcPort)){

					m_aClients[i]->m_strName = strOldName;
					m_aClients[i]->LogOut();
					m_aClients.RemoveAt(i);
					CString strWarn;
					strWarn.Format("Username hijack blocked [%s %s]", strOldName, Util::FormatIP(dwSrcIP));
					SendAction(" ", strWarn);
					return;
				}
		}
	}

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendRename(strOldName, dwOldIP, wOldPort, strNewName, dwNewIP, wNewPort, wLine, dwFiles);
	}
}

void CServerView::SendMode(const CString strSender, const CString strMode)
{

	CString strModeSend;
	strModeSend.Format("sets mode: %s", strMode);

	ServerAction(strSender, strModeSend);
}

void CServerView::ServerAction(const CString strSender, const CString strMsg)
{

	ClientAction* pN = new ClientAction();
	pN->dwIP = 0;
	pN->wPort = 0;
	pN->strName = strSender;
	pN->strText = strMsg;
	SendMessage(UWM_CLNNOTIFY, (WPARAM)pN->wType, (LPARAM)pN);
}

UINT CServerView::NotifyThread(LPVOID pParam)
{

	CServerView* pServer = (CServerView*)pParam;
	ASSERT(pServer);

	pServer->m_eNotifyDone.ResetEvent();

	while(pServer->m_bHosted){

		if(pServer->m_qNotifies.empty()){

            Sleep(50);
			continue;
		}

		ClientNotify* pN = pServer->m_qNotifies.front();
		pServer->m_qNotifies.pop();

		switch(pN->wType){
		
			case CC_MSG:
				pServer->SendMsg(((ClientMessage*)pN)->strName, ((ClientMessage*)pN)->strText);
				break;
			case CC_ACTION:
				pServer->SendAction(((ClientAction*)pN)->strName, ((ClientAction*)pN)->strText);
				break;
			case CC_RENAME:
				pServer->SendRename(((ClientRename*)pN)->dwIP, ((ClientRename*)pN)->wPort,
									((ClientRename*)pN)->strOldName, ((ClientRename*)pN)->dwOldIP, ((ClientRename*)pN)->wOldPort,
									((ClientRename*)pN)->strNewName, ((ClientRename*)pN)->dwNewIP, ((ClientRename*)pN)->wNewPort,
									((ClientRename*)pN)->wNewLine, ((ClientRename*)pN)->dwNewFiles);
				break;
			case CC_ERROR:

				if(((ClientError*)pN)->wIndex < pServer->m_aClients.GetSize()){

					CClientSocket* pTmp = pServer->m_aClients[((ClientError*)pN)->wIndex];
					pServer->m_aClients.RemoveAt(((ClientError*)pN)->wIndex, 1);
					pServer->SendPart(pTmp->m_strName, pTmp->m_dwIP, pTmp->m_wPort);
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

BOOL CServerView::CheckUserName(const CString strName, DWORD dwIP, WORD wPort)
{
	
	if(strName.IsEmpty()) return FALSE; // username may not be empty
	if(strName[0] == '@') return FALSE; // username may not start with operator prefix
	if(strName.Find(" ") >= 0) return FALSE; // username may not contain a space
	if(strName.Find("/mode ") >= 0) return FALSE; // username may not contain /mode
	if(strName.Find("\n") >= 0) return FALSE;
	if(strName.Find("\r") >= 0) return FALSE;
	if(strName.Find("\t") >= 0) return FALSE;

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

		ServerAction("> Channel Modes:", strModes);
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
			strKick.Format("has been kicked by %s (%s)", strUser, strParam);
			ServerAction(strAddRem, strKick);
			SendPart(pKick->m_strName, pKick->m_dwIP, pKick->m_wPort);
			delete pKick;
		}
		return TRUE;
	}
	else if(strCmd != "/mode"){

		return FALSE;
	}
	
	if(strAddRem.GetLength() != 2) return FALSE;

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
				ServerAction("Error>", "Key already set for this channel.");
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

			if(m_uMode & CM_MODERATED){

				for(int i = 0; i < m_aClients.GetSize(); i++){

					if(m_aClients[i]->m_strName == strParam){

						m_aClients[i]->AddMode(UM_SPEAKPERMIT);
						SendMode(strUser, "+v " + strParam);
						break;
					}
				}
			}
			else{

				ServerAction("Error>", "Mode v requires Mode m");
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

			m_wLimit = atoi((LPTSTR)(LPCTSTR)strParam);
			SendMode(strUser, "+l " + strParam);
		}
		else if(strMode == "b"){ // ban

			ServerAction("Alpha Message>", "This has not been implemented yet.");
		}

	}
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

					ServerAction("Error>", "Key already set for this channel.");
				}
			}
			else{

				ServerAction("Error>", "Key already set for this channel.");
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

			if(m_uMode & CM_MODERATED){

				for(int i = 0; i < m_aClients.GetSize(); i++){

					if(m_aClients[i]->m_strName == strParam){

						m_aClients[i]->RemoveMode(UM_SPEAKPERMIT);
						TRACE("%X", m_aClients[i]->GetUserMode());
						SendMode(strUser, "-v " + strParam);
						break;
					}
				}
			}
			else{

				ServerAction("Error>", "Mode v requires Mode m");
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

			m_wLimit = 1000;
			SendMode(strUser, "-l");
		}
		else if(strMode == "b"){ // ban

			ServerAction("Alpha Message>", "This has not been implemented yet.");
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
