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
#include "ChatClient.h"
#include "Metis3Doc.h"
#include "Metis3View.h"
#include "Settings.h"
#include "util.h"
#include ".\chatclient.h"
//#include "ClientSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSettings g_sSettings;

#ifdef _DEBUG
extern Hex_Trace(PVOID pBuffer, int nLen);
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT UWM_MESSAGE	= ::RegisterWindowMessage("UWM_MESSAGE-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_ACTION		= ::RegisterWindowMessage("UWM_ACTION-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_TOPIC		= ::RegisterWindowMessage("UWM_TOPIC-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_MOTD		= ::RegisterWindowMessage("UWM_MOTD-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_JOIN		= ::RegisterWindowMessage("UWM_JOIN-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_PART		= ::RegisterWindowMessage("UWM_PART-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_RENAME		= ::RegisterWindowMessage("UWM_RENAME-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_ADDUSER    = ::RegisterWindowMessage("UWM_ADDUSER-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_REDIRECT   = ::RegisterWindowMessage("UWM_REDIRECT-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_UNKNOWN    = ::RegisterWindowMessage("UWM_UNKNOWN-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_SYSTEM     = ::RegisterWindowMessage("UWM_SYSTEM-229F871A-7B27-44C5-8879-AF881DE94891");
// new commands for winmx 3.52 beta
UINT UWM_ROOMRENAME = ::RegisterWindowMessage("UWM_ROOMRENAME-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_OPMESSAGE   = ::RegisterWindowMessage("UWM_OPMESSAGE-229F871A-7B27-44C5-8879-AF881DE94891");
UINT UWM_SERVERTYPE = ::RegisterWindowMessage("UWM_SERVERTYPE-229F871A-7B27-44C5-8879-AF881DE94891");


CChatClient::CChatClient()
{

	m_dwClientIP	 = 16777343;
	m_wClientUDPPort = 6257;
	m_dwDWKey		 = 0;
	m_dwUPKey		 = 0;
	m_dwFiles		 = 0;
	m_wLineType		 = 0;
	m_strUser        = "RoboMX[:]..494";
	m_wRoomTCPPort   = 6699;
	m_strRoomIP      = "127.0.0.1";
	m_pThread        = 0;
	m_bListen        = FALSE;
	m_wServerType	 = SERVER_UNKNOWN;
	m_bOldJoin		 = FALSE;
	m_bAbort		 = FALSE;
	m_bListen		 = FALSE;
}

CChatClient::~CChatClient()
{

}

BOOL CChatClient::Connect()
{

	m_eClose.ResetEvent();
	m_wServerType = SERVER_UNKNOWN;
	m_bAbort  = FALSE;
	m_bListen = FALSE;
	m_pThread = AfxBeginThread(RecvProc, (PVOID)this, THREAD_PRIORITY_NORMAL);

	return TRUE;
}

BOOL CChatClient::Disconnect()
{

	m_bAbort = TRUE;
	if(m_bListen){

		m_bListen = FALSE;
		m_mSocket.Close();
		DWORD n = WaitForSingleObject(m_eClose.m_hObject, 1000);
		if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

			TerminateThread(m_pThread->m_hThread, 0);
		}
		m_pThread = NULL;
	}

	return TRUE;
}

BOOL CChatClient::SendRename(CString strUser, DWORD dwFiles, WORD wLine)
{

	//0x0065][Line-Type:2][IP-Address:4][UDP-Port:2][Shared-Files:4][New-Name:N][00:1]

	m_dwFiles	= dwFiles;
	m_wLineType = wLine;
	m_strUser   = strUser;

	if(m_bListen){

		char buffer[1024];
		ZeroMemory(buffer, 1024);
		
		WORD wLen = Util::FormatMXMessage(0x0065, (char*)&buffer, "WDWDS", 
									  m_wLineType, m_dwClientIP,
									  m_wClientUDPPort, m_dwFiles,
									  (LPCTSTR)m_strUser);

		m_dwUPKey = EncryptMXTCP((BYTE*)buffer, wLen, m_dwUPKey);

		int nSend = m_mSocket.Send(buffer, wLen,0);

		if(nSend == SOCKET_ERROR){

			m_bListen  = FALSE;
			CString strError;
			strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
			return FALSE;
		}
	}
	return TRUE;
}

void CChatClient::SendMessage(LPCTSTR lpszMessage, int nLen, BOOL bAction)
{

	/*
	Normal Message (Client)
	0x00C8][Message:N]
	Action Message (Client)
	0x00CA][Message:N][00:1]
	*/
	if(m_bListen){

		char buffer[1024];
		ZeroMemory(buffer, 1024);

		WORD wLen = Util::FormatMXMessage((bAction ? 0x00CA : 0x00C8), (char*)&buffer, "S", lpszMessage);
		m_dwUPKey = EncryptMXTCP((BYTE*)buffer, wLen, m_dwUPKey);
		
		int nSend = m_mSocket.Send(buffer, wLen, 0);

		if(nSend == SOCKET_ERROR){

			m_bListen  = FALSE;
			CString strError;
			strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
		}
	}
}


BOOL CChatClient::SendNew(LPCTSTR lpszKey)
{

	// admin cmd(client)
	// 0x1450][LOGIN:N]
	if(m_bListen){

		char buffer[1024];
		ZeroMemory(buffer, 1024);

		WORD wLen = Util::FormatMXMessage(0x1450, (char*)&buffer, "S", lpszKey);
		m_dwUPKey = EncryptMXTCP((BYTE*)buffer, wLen, m_dwUPKey);

		int nSend = m_mSocket.Send(buffer, wLen, 0);

		if(nSend == SOCKET_ERROR){

			m_bListen  = FALSE;
			CString strError;
			strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
		}
	}
	return 0;
}

void CChatClient::Ping()
{

	// 0xFDE8
	if(m_bListen){

		char buffer[4];
		ZeroMemory(buffer, 4);
		WORD wType = 0xFDE8;
		memcpy(buffer, &wType, 2);

		m_dwUPKey = EncryptMXTCP((BYTE*)buffer, 4, m_dwUPKey);
		
		if(g_sSettings.GetPing()){

			WriteMessage("PING", RGB(150,150,150));
		}

		int nSend = m_mSocket.Send(buffer, 4,0);

		if(nSend == SOCKET_ERROR){

			m_bListen  = FALSE;
			CString strError;
			strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
		}
	}
}


BOOL CChatClient::SetRoom(CString strRoom)
{

	// testroom2_0100007F1A2B
	// 0100007F IP
	// 1A2B = Port
	m_strRoom = strRoom;

	int nIndex = m_strRoom.ReverseFind('_')+1;

	if(nIndex <= 0) return FALSE;

	CString strTmp = m_strRoom.Mid(nIndex, 8);
	
	int nA = 0, nB = 0, nC = 0, nD = 0;
	
	nA = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
	nB = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
	nC = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
	nD = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);

	m_strRoomIP.Format("%d.%d.%d.%d", nD, nC, nB, nA);

	m_wRoomTCPPort = Util::axtoi((LPSTR)(LPCSTR)strRoom.Mid(m_strRoom.GetLength()-4), 4);

	m_dwClientIP = g_sSettings.GetNodeIP();

	nA = ((m_dwClientIP >> 24) & 0xff);
	nB = ((m_dwClientIP >> 16) & 0xff);
	nC = ((m_dwClientIP >> 8) & 0xff);
	nD = (m_dwClientIP & 0xff);

	m_dwClientIP = MAKEIPADDRESS(nA, nB, nC, nD);

	m_wClientUDPPort = (WORD)g_sSettings.GetNodePort();
	
	if(m_wRoomTCPPort < 1024) return FALSE;
	return TRUE;
}


void CChatClient::DecodeCommand(WORD wType, WORD wLen, char *cmd)
{

	switch(wType){

	case 0x012C: // topic
		::SendMessage(m_hView, UWM_TOPIC, wLen, (LPARAM)cmd);
		break;
	case 0x0078: // motd
		::SendMessage(m_hView, UWM_MOTD, wLen, (LPARAM)cmd);
		break;
	case 0x006F: // userlist
	case 0x0072: // new userlist
		::SendMessage(m_hView, UWM_ADDUSER, MAKEWPARAM(wType, wLen), (LPARAM)cmd);
		break;
	case 0x006E: // user join
	case 0x0071: // new join
	case 0x0075: // new user join with IP
		DecodeJoin(wType, wLen, cmd);
		break;
	case 0x0073: // user part
		::SendMessage(m_hView, UWM_PART, wLen, (LPARAM)cmd);
		break;
	case 0x0070: // user rename
	case 0x0074: // new user rename
		::SendMessage(m_hView, UWM_RENAME, MAKEWPARAM(wType, wLen), (LPARAM)cmd);
		break;
	case 0x00D2: //  admin messages (xy is now know as ab)
	case 0x00D3:  // command echo
		::SendMessage(m_hView, UWM_OPMESSAGE, MAKEWPARAM(wType, wLen), (LPARAM)cmd);
		break;
	case 0x00C9: // normal message
		::SendMessage(m_hView, UWM_MESSAGE, wLen, (LPARAM)cmd);
		break;
	case 0x00CB: // Action
		::SendMessage(m_hView, UWM_ACTION, wLen, (LPARAM)cmd);
		break;
	case 0x00C8: // rcms garbage
		if(m_wServerType != SERVER_RCMS){

			WriteMessage(IDS_SERVER_RCMS, g_sSettings.GetRGBOp());
			::SendMessage(m_hView, UWM_SERVERTYPE, SERVER_RCMS, 0);
			m_wServerType = SERVER_RCMS;
		}
		break;
	case 0x0068: // winmx server id
		if(m_wServerType != SERVER_ROBOMX){

			WriteMessage(IDS_SERVER_WINMX353, g_sSettings.GetRGBOp());
			if(m_bOldJoin){

				WriteMessage(IDS_WINMX353_WRONGJOIN_WARNING, RGB(150,0,0));
			}
			m_wServerType = SERVER_WINMX353;
			::SendMessage(m_hView, UWM_SERVERTYPE, SERVER_WINMX353, 0);
		}
		break;
	case 0x0069: // robomx join id
		WriteMessage(IDS_SERVER_ROBOMX, g_sSettings.GetRGBOp());
		::SendMessage(m_hView, UWM_SERVERTYPE, SERVER_ROBOMX, 0);
		m_wServerType = SERVER_ROBOMX;
		break;
	case 0x012D: // room name changed
		::SendMessage(m_hView, UWM_ROOMRENAME, wLen, (LPARAM)cmd);
		break;
	case 0x0190: // redirect
		::SendMessage(m_hView, UWM_REDIRECT, wLen, (LPARAM)cmd);
		break;
	default: // unknown
		::SendMessage(m_hView, UWM_UNKNOWN, MAKEWPARAM(wType, wLen), (LPARAM)cmd);
	}
}
    
int CChatClient::DecodeJoin(WORD wType, WORD wLen, char* buffer)
{

	ClientJoin* pJoin = new ClientJoin();
	
	LPSTR lpUser  = 0;
	BYTE  btDummy = 0;

	if(wType == 0x071){ // winmx 3.52 and later (no-ip display)

		if(Util::ScanMessage(buffer, wLen, "SDWWDB", &lpUser, &pJoin->dwNodeIP, &pJoin->wNodePort, &pJoin->wLine, &pJoin->dwFiles, &btDummy) < 6){

			return 0;
		}
	}
	else if(wType == 0x075){ // winmx 3.52 and later (ip display)

		if(Util::ScanMessage(buffer, wLen, "SDWWDBD", &lpUser, &pJoin->dwNodeIP, &pJoin->wNodePort, &pJoin->wLine, &pJoin->dwFiles, &btDummy, &pJoin->dwSrcIP) < 6){

			return 0;
		}
		pJoin->strHost = CMySocket::GetHostName(Util::FormatIP(pJoin->dwSrcIP));
	}
	else{  // old winmx

		if(Util::ScanMessage(buffer, wLen, "SDWWD", &lpUser, &pJoin->dwNodeIP, &pJoin->wNodePort, &pJoin->wLine, &pJoin->dwFiles) != 5){

			return 0;
		}
	}

	pJoin->strName = lpUser;
	if((btDummy >= 0) && (btDummy <= 2))
		pJoin->wLevel  = (WORD)btDummy;
	else
		pJoin->wLevel  = 0;

	::SendMessage(m_hView, UWM_JOIN, 0, (LPARAM)pJoin);
	delete pJoin; 

	return 1;
}

void CChatClient::WriteMessage(LPCTSTR lpszMsg, COLORREF rColor)
{

	if(!m_hView) return;
	if(!::IsWindow(m_hView)) return;

	::SendMessage(m_hView, UWM_SYSTEM, (WPARAM)rColor, (LPARAM)lpszMsg);
}

void CChatClient::WriteMessage(UINT nID, COLORREF rColor)
{

	CString strText;
	strText.LoadString(nID);
	WriteMessage(strText, rColor);
}

#define BUFFER_SIZE 10000

UINT CChatClient::RecvProc(PVOID pParam)
{

	CChatClient* pClient = (CChatClient*)pParam;
	ASSERT(pClient);

	pClient->m_eClose.ResetEvent();

	int nTries = g_sSettings.GetRetry() ? g_sSettings.GetRetries() : 1;

	for(int nTry = 0; (nTry <= nTries) && !pClient->m_bAbort;  nTry++){

		if(nTry > 0){

			CString strOut;
			strOut.Format(IDS_CONNECTION_RETRY, nTry, nTries);
			pClient->WriteMessage(strOut, g_sSettings.GetRGBPend());
		}
		if(pClient->ConnectInternal()){

			pClient->ListenInternal();
		}
	}
	
	TRACE("Setting Event and exiting thread\n");
	pClient->m_bListen = FALSE;
	pClient->m_eClose.SetEvent();
	TRACE("Bye\n");
	return 0;
}

BOOL CChatClient::ConnectInternal(void)
{

	int nRecv = 0;
	char buffer[BUFFER_SIZE] = {'\0'};
	WORD	wType = 0;
	WORD	wLen  = 0;

	if(!m_mSocket.Connect(m_strRoomIP, m_wRoomTCPPort, IPPROTO_TCP)){

		CString strError;
		strError.Format(IDS_ERROR_CONNECTING, m_strRoomIP, m_mSocket.GetLastErrorStr());
		WriteMessage(strError, g_sSettings.GetRGBErr());
		return FALSE;
	}

	if(m_bAbort) return FALSE;

	WriteMessage(IDS_HANDSHAKE_START, g_sSettings.GetRGBPend());
	m_mSocket.Recv(buffer, 1, 5);
	
	if(buffer[0] != 0x31){
		
		WriteMessage(IDS_ERROR_LOGIN_UNKNOWN,  g_sSettings.GetRGBErr());
		return FALSE;
	}

	if(m_bAbort) return FALSE;
	CreateCryptKeyID(0x57, (BYTE *)buffer);

	// Send UP Key Block
	if(m_mSocket.Send(buffer, 16, 5) != 16){
		
		WriteMessage(IDS_ERROR_LOGIN_KEY,  g_sSettings.GetRGBErr());
		return FALSE;
	}

	// Recv DW Key Block
	if(m_bAbort) return FALSE;
	if(m_mSocket.Recv(buffer, 16, 5) != 16){
		
		WriteMessage(IDS_ERROR_LOGIN_KEY,  g_sSettings.GetRGBErr());
		return FALSE;
	}

	// Check Cryptkey
	if(m_bAbort) return FALSE;
	if(GetCryptKeyID((BYTE *)buffer) != 0x58){
	
		// this was not crypt key from Chatserver :-(
		WriteMessage(IDS_ERROR_NO_CHATSERVER,  g_sSettings.GetRGBErr());
		return FALSE;
	}

	// Get the keys
	GetCryptKey((BYTE *)buffer, &m_dwUPKey, &m_dwDWKey);

	// Prepare login buffer...
	ZeroMemory(buffer, BUFFER_SIZE);

	// Send new pre-login packet
	// ED 13 01 00 31	wLen = 5;
	
	if(m_bAbort) return FALSE;

	if(!m_bOldJoin){

		*(WORD*)buffer = 0xED;
		*(WORD*)(buffer+1) = 0x13;
		*(WORD*)(buffer+2) = 0x01;
		*(WORD*)(buffer+3) = 0x00;
		*(WORD*)(buffer+4) = 0x31;
		wLen = 5;

		m_dwUPKey = EncryptMXTCP((BYTE*)buffer, wLen, m_dwUPKey);
		
		if(m_mSocket.Send(buffer, wLen, 5) != wLen){

			CString strError;
			strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
			return FALSE;
		}
	}

	if(m_bAbort) return FALSE;
	//Login-Request: (Client)
	//0x0064][00:1][RoomName:N][LineType:2][Room-IP-Address:4][UDP-Port:2][SharedFiles:4][Username:N][00:1]
	wLen = Util::FormatMXMessage(0x0064, (char*)&buffer, "SWDWDS", 
					(LPCTSTR)m_strRoom, 
					m_wLineType,
					m_dwClientIP,
					m_wClientUDPPort,
					m_dwFiles, 
					(LPCTSTR)m_strUser);

	m_dwUPKey = EncryptMXTCP((BYTE*)buffer, wLen, m_dwUPKey);

	if(m_bAbort) return FALSE;
	if(m_mSocket.Send(buffer, wLen, 5) != wLen){

		CString strError;
		strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
		WriteMessage(strError, g_sSettings.GetRGBErr());
		return FALSE;
	}

	if(m_bAbort) return FALSE;
	ZeroMemory(buffer, BUFFER_SIZE);
	if(m_mSocket.Recv(buffer, 5, 5) != 5){

		CString strError;
		strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
		WriteMessage(strError, g_sSettings.GetRGBErr());
		return FALSE;
	}
	
	if(m_bAbort) return FALSE;
	m_dwDWKey = DecryptMXTCP((BYTE *)buffer, 5, m_dwDWKey);

	if((*(WORD*)buffer) != 0x0066){

		WriteMessage(IDS_ERROR_LOGIN_REJECTED, RGB(150,0,0));
		return FALSE;
	}

 	m_wNumUsers = *(WORD*)(buffer+2);
	WriteMessage(IDS_LOGIN_OK, RGB(0, 120, 0));
	m_bListen = TRUE;
	return TRUE;
}

BOOL CChatClient::ListenInternal(void)
{

	int nRecv = 0;
	char buffer[BUFFER_SIZE] = {'\0'};
	WORD	wType = 0;
	WORD	wLen  = 0;

	m_bListen = TRUE;

	ZeroMemory(buffer, BUFFER_SIZE);
	
	// main recv loop
	while(m_bListen) {


		nRecv = m_mSocket.Recv(buffer, 4, 15);
		if(nRecv == SOCKET_ERROR){

			if((m_mSocket.GetLastError() != SOCK_TIMEOUT) && m_bListen){

				m_bListen  = FALSE;
				CString strError;
				strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
				WriteMessage(strError, g_sSettings.GetRGBErr());
				break;
			}
			else continue;
		}
		

		if(nRecv != 4){
			
			Sleep(100);
			CString strError;
			strError.Format(IDS_ERROR_FIXME, buffer);
			WriteMessage(strError, RGB(150, 150, 150));
			continue;
		}
		
		m_dwDWKey = DecryptMXTCP((BYTE*)(buffer), 4, m_dwDWKey);

		memcpy(&wType, buffer, 2);
		if(wType == 0xFDE8){

			if(g_sSettings.GetPing())
				WriteMessage("PONG", RGB(150,150,150));
			continue;
		}

		wLen = *(WORD*)(buffer+2);
		
		if(wLen == 0) continue;

		if(!m_bListen) break;

		nRecv = m_mSocket.Recv(buffer, wLen, 20);

		if(nRecv == SOCKET_ERROR){

			if(m_mSocket.GetLastError() != SOCK_TIMEOUT){

				m_bListen  = FALSE;
				CString strError;
				strError.Format(IDS_ERROR_NETWORK, m_mSocket.GetLastErrorStr());
				WriteMessage(strError, g_sSettings.GetRGBErr());
				break;
			}
		}
		if(nRecv != wLen) continue;

		m_dwDWKey = DecryptMXTCP((BYTE*)(buffer), wLen, m_dwDWKey);

		DecodeCommand(wType, wLen, buffer);
		ZeroMemory(buffer,1024);

	}

	return m_bListen;
}
