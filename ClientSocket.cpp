#include "stdafx.h"
#include ".\clientsocket.h"
#include "MXSock.h"
#include "util.h"
#include "settings.h"


extern CSettings g_sSettings;

extern UINT UWM_CLNNOTIFY;

#ifdef _DEBUG
void Hex_Trace(PVOID pBuffer, int nLen)
{

	char* pOut = (char*)pBuffer;

	TRACE("> ");

	for(int i = 0; i < nLen; i++){

		TRACE("%02X ", pOut[i]);
	}

	TRACE("\n");
	
}
#else
#define Hex_Trace
#endif


CClientSocket::CClientSocket(SOCKET sSocket) :
	CMySocket(sSocket)
{

	m_strName    = "";
	m_strSrcHost = "";
	m_dwSrcIP	 = 0;
	m_wSrcPort  = 0;
	m_dwIP       = 0;
	m_wPort      = 0;
	m_wLineType  = 0;
	m_dwFiles    = 0;
	m_uMode      = UM_PENDING;
	m_bListen    = FALSE;
	m_dwUPKey    = 0;
	m_dwDWKey    = 0;
	m_hMsgTarget = NULL;
	m_eDone.SetEvent();
	m_wClientType = CLIENT_WINMX331;
	m_pThread = NULL;

	ZeroMemory(m_cBuffer, MAX_BUFFER_SIZE);
}

CClientSocket::~CClientSocket(void)
{
}

BOOL CClientSocket::HandShake(const CString strRoom)
{

	WORD wType = 0;
	WORD wLen  = 0;

	*(WORD*)m_cBuffer = 0x31;

	if(Send(m_cBuffer, 1, 10) != 1){

		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	GetPeerAddr(&m_dwSrcIP, &m_wSrcPort);
	m_strSrcHost = CClientSocket::GetHostName(Util::FormatIP(m_dwSrcIP));

	BYTE btKey[16];
	CreateCryptKeyID(0x0058, (BYTE*)btKey);
	
	GetCryptKey((BYTE*)btKey, &m_dwUPKey, &m_dwDWKey);


	// Recieve Client Keyblock
	if(Recv(m_cBuffer, 16, 5) != 16){

		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	// Send up key block
	if(Send((char*)btKey, 16, 5) != 16){

		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	if(GetCryptKeyID((BYTE*)m_cBuffer) != 0x57){

		TRACE("Invalid CryptKey ID\n");
		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	ZeroMemory(m_cBuffer, MAX_BUFFER_SIZE);

	// Recv Login info
	if(Recv(m_cBuffer, 4, 5) != 4){

		TRACE("Login recv error\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	m_dwUPKey = DecryptMXTCP((BYTE*)m_cBuffer, 4, m_dwUPKey);

	wType = *(WORD*)m_cBuffer;
	wLen  = *(WORD*)(m_cBuffer+2);

	// changed for WinMX 3.53 support:
	if((wType == 0x13ED) && (wLen == 0x0001)){

		m_wClientType  = CLIENT_WINMX353;
		if(Recv(m_cBuffer, 1, 5) != 1){	  // recieve rest of data

			TRACE("Login recv error\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
		m_dwUPKey = DecryptMXTCP((BYTE*)m_cBuffer, 1, m_dwUPKey);		
		// now recieve start of next package ;)
 		if(Recv(m_cBuffer, 4, 5) != 4){

			TRACE("Login recv error\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
		
		m_dwUPKey = DecryptMXTCP((BYTE*)m_cBuffer, 4, m_dwUPKey);
		wType = *(WORD*)m_cBuffer;
		wLen  = *(WORD*)(m_cBuffer+2);

	}

	if(wType != 0x0064 || wLen > MAX_BUFFER_SIZE){

		TRACE("Error: Invalid Packet, or wLen too big for buffer\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	if(Recv(m_cBuffer, wLen, 5) != wLen){

		TRACE("Login recv error #2\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}
	
	m_dwUPKey = DecryptMXTCP((BYTE*)m_cBuffer, wLen, m_dwUPKey);

	LPSTR lpszRoom = 0;
	LPSTR lpszName = 0;
	if(Util::ScanMessage(m_cBuffer, MAX_BUFFER_SIZE, "SWDWDS", &lpszRoom, &m_wLineType, &m_dwIP, &m_wPort, &m_dwFiles, &lpszName) != 6){

		TRACE("Invalid argument count in login\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	if(lpszRoom != 0){

		if(strstr(lpszRoom, strRoom) == NULL){
			
			TRACE("Invalid room name\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
	}

	m_strName = lpszName;

	ZeroMemory(m_cBuffer, 512);
	*(WORD*)m_cBuffer = 0x66;
	*(WORD*)(m_cBuffer+2) = 1;

	if(SendCrypted(m_cBuffer, 5, 5) != 5){

		TRACE("Error sending login grant\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	if(m_wClientType == CLIENT_WINMX353){

		*(WORD*)m_cBuffer = 0x0069;
		*(WORD*)(m_cBuffer+2) = 1;

		if(SendCrypted(m_cBuffer, 5, 5) != 5){

			TRACE("Error sending login grant 3\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
	}
	if(m_wClientType == CLIENT_WINMX353){

		*(WORD*)m_cBuffer = 0x0068;
		*(WORD*)(m_cBuffer+2) = 1;

		if(SendCrypted(m_cBuffer, 5, 5) != 5){

			TRACE("Error sending login grant 2\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CClientSocket::HighlevelHandshake(const CString strKey)
{

	WORD wLen = 0;

	if(!strKey.IsEmpty()){

		CString strKeyMsg = g_sSettings.GetKeyMsg();
		strKeyMsg.Replace("\\n", "\n");
		wLen = Util::FormatMXMessage(0x00CB, (char*)m_cBuffer, "SS", "", strKeyMsg);

		if(SendCrypted(m_cBuffer, wLen, 5) != wLen){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}
		
		if(Recv(m_cBuffer, 4, 60) != 4){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}

		m_dwUPKey = DecryptMXTCP((BYTE*)(m_cBuffer), 4, m_dwUPKey);

		WORD wType = *(WORD*)m_cBuffer;
		wLen = *(WORD*)(m_cBuffer+2);
		if((wType != 0x00C8) && (wType != 0x1450)){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}

		ZeroMemory(&m_cBuffer, MAX_BUFFER_SIZE);
		if(Recv(m_cBuffer, wLen, 60) != wLen){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}

		m_dwUPKey = DecryptMXTCP((BYTE*)(m_cBuffer), wLen, m_dwUPKey);

		LPSTR lpString = 0;

		if(Util::ScanMessage(m_cBuffer, MAX_BUFFER_SIZE, "S", &lpString) == 1){

			
			if(strKey.Compare(lpString) != 0){

				TRACE("Wrong keyword\n");
				Close();
				m_eDone.SetEvent();
				return FALSE;
			}
		}

	}		
	
	CString strNotice;
	strNotice.Format("\nThis room is powered by %s. Note that the software is still Alpha and not all features work properly yet. This message will be removed in the stable version. Enjoy your stay! :-)", STR_VERSION_DLG);
	wLen = Util::FormatMXMessage(0x00C9, (char*)m_cBuffer, "SS", "Server Notice", strNotice);

	if(SendCrypted(m_cBuffer, wLen, 5) != wLen){

		TRACE("Error performing highlevel Handshake\n");
		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	return TRUE;
}

void CClientSocket::StartUp(void)
{

	m_bListen = TRUE;
	m_eDone.ResetEvent();
	m_pThread = AfxBeginThread(CClientSocket::RecvProc, (LPVOID)this, THREAD_PRIORITY_NORMAL);
}

UINT CClientSocket::RecvProc(LPVOID pParam)
{
	
	CClientSocket* pClient = (CClientSocket*)pParam;
	ASSERT(pClient);

	char buffer[MAX_BUFFER_SIZE];
	int nRecv = 0;
	WORD wType = 0;
	WORD wLen  = 0;

	while(pClient->m_bListen){

		ZeroMemory(buffer, MAX_BUFFER_SIZE);
		nRecv = pClient->Recv(buffer, 4, 5);
		if(nRecv == SOCKET_ERROR){

			if((pClient->GetLastError() != SOCK_TIMEOUT) && pClient->m_bListen){

				TRACE("Socket Error\n");
				pClient->m_bListen  = FALSE;
				ClientError* pE = new ClientError();
				pE->guID = pClient->m_guID;
				if(pClient->GetLastError() !=  SOCK_NOERROR){

					pE->strCause	= pClient->GetLastErrorStr();
				}
				else{

					pE->strCause.LoadString(IDS_CLIENTDISC);
				}
				pClient->SendNotify(CC_ERROR, (LPARAM)pE);
				break;
			}
			else if((pClient->GetLastError() == SOCK_TIMEOUT) && pClient->m_bListen){

				Sleep(20);
				continue;
			}
		}
	
		if(nRecv != 4){
			
			Sleep(20);
			CString strError;
			strError.Format("Stream Error? %s\n", buffer);
			TRACE(strError);
			continue;
		}
		
		pClient->m_dwUPKey = DecryptMXTCP((BYTE*)(buffer), 4, pClient->m_dwUPKey);

		wType = *(WORD*)buffer;
		wLen = *(WORD*)(buffer+2);
		
		if(wLen == 0) continue;
		if(!pClient->m_bListen) break;
		
		ZeroMemory(buffer, 5);
		nRecv = pClient->Recv(buffer, wLen, 0);

		if(nRecv == SOCKET_ERROR){

			if(pClient->GetLastError() != SOCK_TIMEOUT){

				CString strError;
				strError.Format("Robo-Panic [b]: %s :'(\n", pClient->GetLastErrorStr());
				TRACE(strError);
				pClient->m_bListen  = FALSE;
				ClientError* pE = new ClientError();
				pE->guID = pClient->m_guID;
				if(pClient->GetLastError() !=  SOCK_NOERROR){

					pE->strCause	= pClient->GetLastErrorStr();
				}
				else{

					pE->strCause.LoadString(IDS_CLIENTDISC);
				}
				pClient->SendNotify(CC_ERROR, (LPARAM)pE);
				break;
			}
		}
		if(nRecv != wLen) continue;

		pClient->m_dwUPKey = DecryptMXTCP((BYTE*)(buffer), wLen, pClient->m_dwUPKey);
		
		pClient->HandlePacket(wType, wLen, buffer);
		
		Sleep(20);
	}

	TRACE("Leaving Client %s\n", pClient->m_strName);
	pClient->m_eDone.SetEvent();
	return 0;
}

void CClientSocket::AddMode(UINT uMode)
{ 
	m_uMode |= uMode;

	ClientRename* ur = new ClientRename();
	ur->dwIP		= m_dwSrcIP;
	ur->wPort		= m_wSrcPort;
	ur->dwOldFiles  = m_dwFiles;
	ur->dwOldIP     = m_dwIP;
	ur->wOldLine    = m_wLineType;
	ur->wOldPort    = m_wPort;
	ur->strOldName  = m_strName;
	ur->dwNewFiles  = m_dwFiles;
	ur->dwNewIP     = m_dwIP;
	ur->wNewLine    = m_wLineType;
	ur->wNewPort    = m_wPort;
	ur->strNewName  = m_strName;
	if((m_uMode & UM_OPERATOR) == UM_OPERATOR)
		ur->wUserLevel =  1;
	else if((m_uMode & UM_SPEAKPERMIT) == UM_SPEAKPERMIT)
		ur->wUserLevel  = 2;
	else
		ur->wUserLevel = 0;

	SendNotify(CC_RENAME, (LPARAM)ur);
}
void CClientSocket::RemoveMode(UINT uMode)
{ 
	m_uMode &= ~uMode; 

	ClientRename* ur = new ClientRename();
	ur->dwIP		= m_dwSrcIP;
	ur->wPort		= m_wSrcPort;
	ur->dwOldFiles  = m_dwFiles;
	ur->dwOldIP     = m_dwIP;
	ur->wOldLine    = m_wLineType;
	ur->wOldPort    = m_wPort;
	ur->strOldName  = m_strName;
	ur->dwNewFiles  = m_dwFiles;
	ur->dwNewIP     = m_dwIP;
	ur->wNewLine    = m_wLineType;
	ur->wNewPort    = m_wPort;
	ur->strNewName  = m_strName;
	if((m_uMode & UM_OPERATOR) == UM_OPERATOR)
		ur->wUserLevel  = 1;
	else if((m_uMode & UM_SPEAKPERMIT) == UM_SPEAKPERMIT)
		ur->wUserLevel =  2;
	else
		ur->wUserLevel = 0;

	SendNotify(CC_RENAME, (LPARAM)ur);
}

void CClientSocket::HandlePacket(WORD wType, WORD wLen, char* buffer)
{

	//if((wType == 0x1450) && ((m_uMode & UM_SPEAKPERMIT) || (m_uMode & UM_OPERATOR))){
	if(wType == 0x1450){

		// this is the new WinMX 3.53 send method, we have to handle the /me part too -sigh-
		int nMsgID = CC_MSG;
		CString strText = buffer;

		strText.Replace("{\\rtf", "#####");

		if(strText.Find("/me ", 0) == 0){

			strText = strText.Mid(4);
			nMsgID = CC_ACTION;
		}
		else if(strText.Find("/action ", 0) == 0){

			strText = strText.Mid(8);
			nMsgID = CC_ACTION;
		}
		else if(strText.Find("/emote ", 0) == 0){

			strText = strText.Mid(7);
			nMsgID = CC_ACTION;
		}
		else if(strText.Find("/opmsg ", 0) == 0){

			strText = strText.Mid(7);
			nMsgID = CC_OPMSG;
		}

		if(nMsgID == CC_ACTION){

			ClientAction* cm = new ClientAction();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = m_strName;
			cm->strText = strText;
			SendNotify(CC_ACTION, (LPARAM)cm);
		}
		else if(nMsgID == CC_OPMSG){

			ClientOpMsg* cm = new ClientOpMsg();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = m_strName;
			cm->strText = strText;
			SendNotify(CC_OPMSG, (LPARAM)cm);
		}
		else{

			ClientMessage* cm = new ClientMessage();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = m_strName;
			cm->strText = strText;
			SendNotify(CC_MSG, (LPARAM)cm);
		}

	}
	//else if((wType == 0x00C8) && ((m_uMode & UM_SPEAKPERMIT) || (m_uMode & UM_OPERATOR))){ // message
	else if(wType == 0x00C8){ // message

		ClientMessage* cm = new ClientMessage();
		cm->guid	= m_guID; 
		cm->dwIP = m_dwSrcIP;
		cm->wPort = m_wPort;
		cm->uMode	= m_uMode;
		cm->strName = m_strName;
		cm->strText = buffer;
		cm->strText.Replace("{\\rtf", "#####");
		SendNotify(CC_MSG, (LPARAM)cm);
	}
	//else if((wType == 0x00CA) && ((m_uMode & UM_SPEAKPERMIT) || (m_uMode & UM_OPERATOR))){ // action
	else if(wType == 0x00CA){ // action

		ClientAction* cm = new ClientAction();
		cm->guid	= m_guID; 
		cm->dwIP = m_dwSrcIP;
		cm->wPort = m_wPort;
		cm->uMode	= m_uMode;
		cm->strName = m_strName;
		cm->strText = buffer;
		cm->strText.Replace("{\\rtf", "#####");
		SendNotify(CC_ACTION, (LPARAM)cm);
	}
	else if(wType == 0x0065){ // rename

		ClientRename* ur = new ClientRename();
		LPSTR lpName = 0;
		if(Util::ScanMessage(buffer, MAX_BUFFER_SIZE, "WDWDS", &ur->wNewLine, &ur->dwNewIP, &ur->wNewPort, &ur->dwNewFiles, &lpName) == 5){

			ur->guid		= m_guID; 
			ur->dwIP		= m_dwSrcIP;
			ur->wPort		= m_wSrcPort;
			ur->uMode       = m_uMode;
			ur->dwOldFiles  = m_dwFiles;
			ur->dwOldIP     = m_dwIP;
			ur->wOldLine    = m_wLineType;
			ur->wOldPort    = m_wPort;
			ur->strOldName  = m_strName;
			ur->wUserLevel  = (WORD)(m_uMode & UM_OPERATOR);

			m_dwFiles		= ur->dwNewFiles;
			m_dwIP			= ur->dwNewIP;
			m_wPort			= ur->wNewPort;
			m_wLineType		= ur->wNewLine;
			m_strName		= lpName;
			SendNotify(CC_RENAME, (LPARAM)ur);
		}
	}
}

BOOL CClientSocket::SendTopic(const CString strTopic)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = Util::FormatMXMessage(0x012C, (char*)buffer, "S", (LPCSTR)strTopic);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendMotd(const CString strMotd)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = Util::FormatMXMessage(0x00078, (char*)buffer, "S", (LPCSTR)strMotd);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendMsg(const CString strUser, const CString strMsg)
{

	char buffer[MAX_SEND_SIZE];
	
	WORD wLen = Util::FormatMXMessage(0x00C9, (char*)buffer, "SS", (LPCSTR)strUser, (LPCSTR)strMsg);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendOperator(const CString strUser, const CString strMsg, BOOL bEcho)
{

	if((m_uMode & UM_OPERATOR) != UM_OPERATOR) return FALSE;

	if(m_wClientType == CLIENT_WINMX353){

		char buffer[MAX_SEND_SIZE];

		WORD wLen = 0;
		if(bEcho){
			
			wLen = Util::FormatMXMessage(0x00D3, (char*)buffer, "S", (strUser.IsEmpty() ? strMsg : strUser));
		}
		else{
			
			wLen = Util::FormatMXMessage(0x00D2, (char*)buffer, "sss", (LPCSTR)strUser, "", (LPCSTR)strMsg);
		}
		
		return SendCrypted(buffer, wLen, 5) == wLen;
	}
	else{

		return SendAction(strUser, strMsg);
	}
}

BOOL CClientSocket::SendAction(const CString strUser, const CString strMsg)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = Util::FormatMXMessage(0x00CB, (char*)buffer, "SS", (LPCSTR)strUser, (LPCSTR)strMsg);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendUserlist(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	char buffer[MAX_SEND_SIZE];

	WORD wLen = 0;

	if(m_wClientType == CLIENT_WINMX353){

		wLen = Util::FormatMXMessage(0x0072, (char*)buffer, "SDWWDW", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel);
	}
	else{

		wLen = Util::FormatMXMessage(0x006F, (char*)buffer, "SDWWD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = 0;

	if((m_wClientType == CLIENT_WINMX353) && (m_uMode & UM_OPERATOR)){

		// user is admin so send IP too
		wLen = Util::FormatMXMessage(0x0075, (char*)buffer, "SDWWDBD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	}
	else if(m_wClientType == CLIENT_WINMX353){

		// this user is no admin so dont send IP :P
		wLen = Util::FormatMXMessage(0x0071, (char*)buffer, "SDWWDB", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel);
	}
	else{

		wLen = Util::FormatMXMessage(0x006E, (char*)buffer, "SDWWD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendPart(const CString strUser, DWORD dwIP, WORD wPort)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = Util::FormatMXMessage(0x0073, (char*)buffer, "SDW", (LPCSTR)strUser, dwIP, wPort);

	return SendCrypted(buffer, wLen, 5) == wLen;
}


BOOL CClientSocket::SendChannelRename(CString strRoom)
{

	char buffer[MAX_SEND_SIZE];

	WORD wLen = 0;

	if(m_wClientType == CLIENT_WINMX353){

		wLen = Util::FormatMXMessage(0x012D, (char*)buffer, "S", (LPCSTR)strRoom);		
	}
	else{

		wLen = Util::FormatMXMessage(0x00CB, (char*)buffer, "SS", (LPCSTR)"", (LPCSTR)strRoom);
	}

    return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendRename(const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = 0;
	
	if(m_wClientType == CLIENT_WINMX353){

		wLen = Util::FormatMXMessage(0x0074, (char*)buffer, "SDWSDWWDW", 
										(LPCTSTR)strOldName,
										dwOldIP,
										wOldPort,
										(LPCTSTR)strNewName,
										dwNewIP,
										wNewPort,
										wLine,
										dwFiles,
										wUserLevel
									);
	}
	else{

		wLen = Util::FormatMXMessage(0x0070, (char*)buffer, "SDWSDWWD", 
										(LPCTSTR)strOldName,
										dwOldIP,
										wOldPort,
										(LPCTSTR)strNewName,
										dwNewIP,
										wNewPort,
										wLine,
										dwFiles							
									);
	}
	return SendCrypted(buffer, wLen, 5) == wLen;

}

void CClientSocket::LogOut(void)
{

	m_bListen = FALSE;
	Close();
	TRACE("CClientSocket: Waiting for Listen thread to exit\n");
	DWORD n = WaitForSingleObject(m_eDone, 5000);
	if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

		TerminateThread(m_pThread->m_hThread, 0);
	}
	m_pThread = NULL;
	TRACE("LogOut Complete :-)\n");
}

BOOL CClientSocket::SendNotify(WPARAM wParam, LPARAM lParam)
{

	if(m_hMsgTarget == NULL) return FALSE;
	if(!::IsWindow(m_hMsgTarget)) return FALSE;

	::SendMessage(m_hMsgTarget, UWM_CLNNOTIFY, wParam, lParam);
	return TRUE;
}

int CClientSocket::SendCrypted(char *pBuff, int nLen, int nWait)
{

	m_dwDWKey = EncryptMXTCP((BYTE*)pBuff, nLen, m_dwDWKey);

	int nSend = Send(pBuff, nLen, nWait);

	if(nSend != nLen){

		ClientError* ce = new ClientError();
		ce->dwIP		= m_dwSrcIP;
		ce->wPort		= m_wSrcPort;
		ce->guID		= m_guID;
		if(GetLastError() !=  SOCK_NOERROR){

			ce->strCause	= GetLastErrorStr();
		}
		else{

			ce->strCause.LoadString(IDS_CLIENTDISC);
		}
		SendNotify(CC_ERROR, (LPARAM)ce);
	}

	return nSend;
}
