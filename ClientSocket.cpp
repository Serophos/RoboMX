#include "stdafx.h"
#include ".\clientsocket.h"
#include "MXSock.h"
#include "util.h"

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
	m_wClientID  = 0;
	m_hMsgTarget = 0;
	m_eDone.SetEvent();
	m_wClientType = CLIENT_WINMX331;

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

	if(m_wClientType = CLIENT_WINMX353){

		*(WORD*)m_cBuffer = 0x68;
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

		wLen = Util::FormatMXMessage(0x00CB, (char*)m_cBuffer, "SS", "Server Notice:", "This channel is password protected. Please enter the keyword now.\nYou have 60 seconds. After that you will get automatically disconnected.");

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
		if(wType != 0x00C8){

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
	
	wLen = Util::FormatMXMessage(0x00C9, (char*)m_cBuffer, "SS", "Server Notice", "\nThis room is powered by RoboMX. Note that the software is still Alpha and not all features work properly yet. This message will be removed in the stable version.\nEnjoy your stay! :-)\n\n");

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
	AfxBeginThread(CClientSocket::RecvProc, (LPVOID)this, THREAD_PRIORITY_NORMAL);
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
				pE->wIndex = pClient->m_wClientID;
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
				pE->wIndex = pClient->m_wClientID;
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

void CClientSocket::HandlePacket(WORD wType, WORD wLen, char* buffer)
{

	if((wType == 0x1450) && ((m_uMode % UM_SPEAKPERMIT) || (m_uMode & UM_OPERATOR))){

		// this is the new WinMX 3.53 send method, we have to handle the /me part too -sigh-
		BOOL bAction = FALSE;

		ClientMessage* cm = new ClientMessage();
		cm->dwIP	= m_dwSrcIP;
		cm->wPort   = m_wPort;
		cm->strName = m_strName;
		cm->strText = buffer;
		cm->strText.Replace("{\\rtf", "#####");
		if(cm->strText.Find("/me ", 0) == 0){

			cm->strText = cm->strText.Mid(4);
			bAction = TRUE;
		}
		else if(cm->strText.Find("/action ", 0) == 0){

			cm->strText = cm->strText.Mid(9);
			bAction = TRUE;
		}
		else if(cm->strText.Find("/emote ", 0) == 0){

			cm->strText = cm->strText.Mid(8);
			bAction = TRUE;
		}

		SendNotify(bAction ? CC_ACTION : CC_MSG, (LPARAM)cm);

	}
	if((wType == 0x00C8) && ((m_uMode & UM_SPEAKPERMIT) || (m_uMode & UM_OPERATOR))){ // message

		ClientMessage* cm = new ClientMessage();
		cm->dwIP = m_dwSrcIP;
		cm->wPort = m_wPort;
		cm->strName = m_strName;
		cm->strText = buffer;
		cm->strText.Replace("{\\rtf", "#####");
		SendNotify(CC_MSG, (LPARAM)cm);
	}
	else if((wType == 0x00CA) && ((m_uMode & UM_SPEAKPERMIT) || (m_uMode & UM_OPERATOR))){ // action

		ClientAction* cm = new ClientAction();
		cm->dwIP = m_dwSrcIP;
		cm->wPort = m_wPort;
		cm->strName = m_strName;
		cm->strText = buffer;
		cm->strText.Replace("{\\rtf", "#####");
		SendNotify(CC_ACTION, (LPARAM)cm);
	}
	else if(wType == 0x0065){ // rename

		ClientRename* ur = new ClientRename();
		LPSTR lpName = 0;
		if(Util::ScanMessage(buffer, MAX_BUFFER_SIZE, "WDWDS", &ur->wNewLine, &ur->dwNewIP, &ur->wNewPort, &ur->dwNewFiles, &lpName) == 5){

			ur->dwIP		= m_dwSrcIP;
			ur->wPort		= m_wSrcPort;
			ur->dwOldFiles  = m_dwFiles;
			ur->dwOldIP     = m_dwIP;
			ur->wOldLine    = m_wLineType;
			ur->wOldPort    = m_wPort;
			ur->strOldName  = m_strName;
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

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = Util::FormatMXMessage(0x012C, (char*)buffer, "S", (LPCSTR)strTopic);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendMotd(const CString strMotd)
{

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = Util::FormatMXMessage(0x00078, (char*)buffer, "S", (LPCSTR)strMotd);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendMsg(const CString strUser, const CString strMsg)
{

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = Util::FormatMXMessage(0x00C9, (char*)buffer, "SS", (LPCSTR)strUser, (LPCSTR)strMsg);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendAction(const CString strUser, const CString strMsg)
{

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = Util::FormatMXMessage(0x00CB, (char*)buffer, "SS", (LPCSTR)strUser, (LPCSTR)strMsg);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendUserlist(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	char buffer[MAX_BUFFER_SIZE];

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

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = 0;

	if((m_wClientType == CLIENT_WINMX353) && (m_uMode & UM_OPERATOR)){

		// user is admin so send IP too
		wLen = Util::FormatMXMessage(0x006E, (char*)buffer, "SDWWDBD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	}
	else if(m_wClientType == CLIENT_WINMX353){

		// this user is no admin so dont send IP :P
		wLen = Util::FormatMXMessage(0x006E, (char*)buffer, "SDWWDB", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel);
	}
	else{

		wLen = Util::FormatMXMessage(0x006E, (char*)buffer, "SDWWD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendPart(const CString strUser, DWORD dwIP, WORD wPort)
{

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = Util::FormatMXMessage(0x0073, (char*)buffer, "SDW", (LPCSTR)strUser, dwIP, wPort);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendRename(const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles)
{

	char buffer[MAX_BUFFER_SIZE];
	WORD wLen = Util::FormatMXMessage(0x0070, (char*)buffer, "SDWSDWWD", 
									(LPCTSTR)strOldName,
									dwOldIP,
									wOldPort,
									(LPCTSTR)strNewName,
									dwNewIP,
									wNewPort,
									wLine,
									dwFiles							
								);

	return SendCrypted(buffer, wLen, 5) == wLen;

}

void CClientSocket::LogOut(void)
{

	m_bListen = FALSE;
	Close();
	TRACE("CClientSocket: Waiting for Listen thread to exit\n");
	WaitForSingleObject(m_eDone, 5000);
	TRACE("LogOut Complete :-)\n");
}

BOOL CClientSocket::SendNotify(WPARAM wParam, LPARAM lParam)
{

	::SendMessage(m_hMsgTarget, UWM_CLNNOTIFY, wParam, lParam);
	return TRUE;
}

int CClientSocket::SendCrypted(char *pBuff, int nLen, int nWait)
{

	m_dwDWKey = EncryptMXTCP((BYTE*)pBuff, nLen, m_dwDWKey);

	return Send(pBuff, nLen, nWait);
}

