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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSettings g_sSettings;

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
}

CChatClient::~CChatClient()
{

}

BOOL CChatClient::Connect()
{

	m_eClose.ResetEvent();
	//char buffer[1024] = {'\0'};
	

	m_pThread = AfxBeginThread(RecvProc, (PVOID)this, THREAD_PRIORITY_NORMAL);
	return TRUE;
}

UINT CChatClient::RecvProc(PVOID pParam)
{

	TRACE("Entering recv proc\n");
	CChatClient* pClient = (CChatClient*)pParam;
	ASSERT(pClient);

	pClient->m_eClose.ResetEvent();

	int nRecv = 0;
	int nLen  = 0;
	char command[4] = {'\0'};
	char buffer[1024] = {'\0'};
	int nValue  = 0, nSize = sizeof(int);
	int nWait  = 15;
	WORD wType = 0;

	if(!pClient->m_mSocket.Connect(pClient->m_strRoomIP, pClient->m_wRoomTCPPort, 5)){

		CString strError;
		strError.Format("Robo-Panic: Could not Connect to %s. %s", pClient->m_strRoomIP, pClient->m_mSocket.GetLastErrorStr());
		pClient->WriteMessage(strError, g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}
	// recv protocoll version
	//recv(m_sSocket, (char*)&buffer, 1, 0);
	pClient->WriteMessage("Starting handshake", g_sSettings.GetRGBPend());
	pClient->m_mSocket.Recv(buffer, 1, 5);
	
	if(buffer[0] != 0x31){
		
		pClient->WriteMessage("Robo-Panic: Error nogotiating. Login failed.",  g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}


	CreateCryptKeyID(0x57, (BYTE *)buffer);

	// Send UP Key Block
	if(pClient->m_mSocket.Send(buffer, 16, 5) != 16){
		
		pClient->WriteMessage("Robo-Panic: Error nogotiating. Send Keyblock failed.",  g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}

	// Recv DW Key Block
	if(pClient->m_mSocket.Recv(buffer, 16, 5) != 16){
		
		//AfxMessageBox(pClient->m_mSocket.GetLastErrorStr());
		pClient->WriteMessage("Robo-Panic: Error nogotiating. Recieve Keyblock failed.",  g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}

	// Check Cryptkey
	if(GetCryptKeyID((BYTE *)buffer) != 0x58){
	
		// this was not crypt key from Chatserver :-(
		pClient->WriteMessage("Robo-Panic: Error nogotiating. ID missmatch.",  g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}

	// Get the keys
	GetCryptKey((BYTE *)buffer, &pClient->m_dwUPKey, &pClient->m_dwDWKey);

	//Login-Request: (Client)
	//0x0064][00:1][RoomName:N][LineType:2][Room-IP-Address:4][UDP-Port:2][SharedFiles:4][Username:N][00:1]

	// Prepare login buffer...
	ZeroMemory(buffer, sizeof(buffer));
	

	nLen = Util::FormatMXMessage(0x0064, (char*)&buffer, "SWDWDS", 
					(LPCTSTR)pClient->m_strRoom, 
					pClient->m_wLineType,
					pClient->m_dwClientIP,
					pClient->m_wClientUDPPort,
					pClient->m_dwFiles, 
					(LPCTSTR)pClient->m_strUser);

	pClient->m_dwUPKey = EncryptMXTCP((BYTE*)buffer, nLen, pClient->m_dwUPKey);

	if(pClient->m_mSocket.Send(buffer, nLen, 5) != nLen){

		CString strError;
		strError.Format("Robo-Panic: Sending login data failed: %s", pClient->m_mSocket.GetLastErrorStr());
		pClient->WriteMessage(strError, g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}

	ZeroMemory(buffer, sizeof(buffer));
	if(pClient->m_mSocket.Recv(buffer, 5, 0) != 5){

		CString strError;
		strError.Format("Robo-Panic: Handshake failed. %s", pClient->m_mSocket.GetLastErrorStr());
		pClient->WriteMessage(strError, g_sSettings.GetRGBErr());
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}
	
	pClient->m_dwDWKey = DecryptMXTCP((BYTE *)buffer, 5, pClient->m_dwDWKey);

	if((*(WORD*)buffer) != MXCHAT_LOGINGRANTED){

		pClient->WriteMessage("Login rejected.", RGB(150,0,0));
		pClient->m_bListen  = FALSE;
		pClient->m_eClose.SetEvent();
		return FALSE;
	}
	pClient->m_wNumUsers = *(WORD*)(buffer+2);
	pClient->WriteMessage("Login granted.", RGB(0, 120, 0));
	
	pClient->m_bListen = TRUE;
	

	// main recv loop
	while(pClient->m_bListen) {


		nRecv = pClient->m_mSocket.Recv(buffer, 4, 1);
		if(nRecv == SOCKET_ERROR){

			if((pClient->m_mSocket.GetLastError() != SOCK_TIMEOUT) && pClient->m_bListen){

				CString strError;
				strError.Format("Robo-Panic [a]: %s :'(", pClient->m_mSocket.GetLastErrorStr());
				pClient->WriteMessage(strError, g_sSettings.GetRGBErr());
				pClient->m_bListen  = FALSE;
				break;
			}
			else continue;
		}
		

		if(nRecv != 4){
			
			Sleep(100);
			CString strError;
			strError.Format("Stream Error? %s", buffer);
			pClient->WriteMessage(strError, RGB(150, 150, 150));
			continue;
		}
		
		pClient->m_dwDWKey = DecryptMXTCP((BYTE*)(buffer), 4, pClient->m_dwDWKey);

		memcpy(&wType, buffer, 2);
		if(wType == 0xFDE8){

			if(g_sSettings.GetPing())
				pClient->WriteMessage("PONG", RGB(150,150,150));
			continue;
		}

		nLen = *(WORD*)(buffer+2);
		
		if(nLen == 0) continue;

		if(!pClient->m_bListen) break;

		nRecv = pClient->m_mSocket.Recv(buffer+4, nLen, 0);

		if(nRecv == SOCKET_ERROR){

			if(pClient->m_mSocket.GetLastError() != SOCK_TIMEOUT){

				CString strError;
				strError.Format("Robo-Panic [b]: %s :'(", pClient->m_mSocket.GetLastErrorStr());
				pClient->WriteMessage(strError, g_sSettings.GetRGBErr());
				pClient->m_bListen  = FALSE;
				break;
			}
		}
		if(nRecv != nLen) continue;

		pClient->m_dwDWKey = DecryptMXTCP((BYTE*)(buffer+4), nLen, pClient->m_dwDWKey);

		pClient->DecodeCommand(*(WORD*)buffer, nLen, buffer+4);
		ZeroMemory(buffer,1024);

	}
	
	TRACE("Setting Event and exiting thread\n");
	pClient->m_eClose.SetEvent();
	TRACE("Bye\n");
	return 0;
}

BOOL CChatClient::Disconnect()
{

	if(m_bListen){

		m_bListen = FALSE;
		TRACE("Wating for shutdown\n");
		WaitForSingleObject(m_eClose.m_hObject, INFINITE);
		Sleep(100);
		TRACE("Closing socket\n");
		m_mSocket.Close();
		//delete m_pThread;
		//m_pThread = NULL;
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
		
		/*int nCmdLen = 0;
		memcpy(buffer+4, &m_wLineType, 2);
		nCmdLen+=2;
		memcpy(buffer+6, &m_dwClientIP, 4);
		nCmdLen+=4;
		memcpy(buffer+10, &m_wClientUDPPort, 2);
		nCmdLen+=2;
		memcpy(buffer+12, &m_dwFiles, 4);
		nCmdLen+=4;
		strcpy(buffer+16, m_strUser);
		nCmdLen+=m_strUser.GetLength()+1;
		
		*(WORD*)buffer = 0x0065;
		*(WORD*)(buffer+2) = nCmdLen;
		nCmdLen+=4;*/

		WORD wLen = Util::FormatMXMessage(0x0065, (char*)&buffer, "WDWDS", 
									  m_wLineType, m_dwClientIP,
									  m_wClientUDPPort, m_dwFiles,
									  (LPCTSTR)m_strUser);

		m_dwUPKey = EncryptMXTCP((BYTE*)buffer, wLen, m_dwUPKey);

		int nSend = m_mSocket.Send(buffer, wLen,0);

		if(nSend == SOCKET_ERROR){

			CString strError;
			strError.Format("Robo-Panic [i]: %s :'(", m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
			m_bListen  = FALSE;
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

			CString strError;
			strError.Format("Robo-Panic[j]: %s :'(", m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
			m_bListen  = FALSE;
		}
	}
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

			CString strError;
			strError.Format("Robo-Panic [k]: %s :'(", m_mSocket.GetLastErrorStr());
			WriteMessage(strError, g_sSettings.GetRGBErr());
			m_bListen  = FALSE;
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
	
	//m_dwClientIP = axtoi((LPSTR)(LPCSTR)strTmp, 8);

	int nA = 0, nB = 0, nC = 0, nD = 0;
	
	nA = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
	nB = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
	nC = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
	nD = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);

	m_strRoomIP.Format("%d.%d.%d.%d", nD, nC, nB, nA);

	// 7F 00 00 01
	m_dwClientIP = g_sSettings.GetServerIP();
	m_wClientUDPPort = (WORD)g_sSettings.GetServerPort();
    strTmp.Format("%X", m_dwClientIP);
	nA = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
	nB = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
	nC = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
	nD = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);
	strTmp.Format("%02X%02X%02X%02X", nD, nC, nB, nA);
	m_dwClientIP = Util::axtoi((LPSTR)(LPCSTR)strTmp, 8);

	m_wRoomTCPPort = Util::axtoi((LPSTR)(LPCSTR)strRoom.Mid(m_strRoom.GetLength()-4), 4);
	
	if(m_wRoomTCPPort < 1024) return FALSE;
	return TRUE;
}


void CChatClient::DecodeCommand(WORD wType, WORD wLen, char *cmd)
{


	// TODO
	// Change to sendmessage
	switch(wType){

	case 0x12C: // topic
		::SendMessage(m_pView->m_hWnd, UWM_TOPIC, wLen, (LPARAM)cmd);
		break;
	case 0x078: // motd
		::SendMessage(m_pView->m_hWnd, UWM_MOTD, wLen, (LPARAM)cmd);
		break;
	case 0x06F: // userlist
		::SendMessage(m_pView->m_hWnd, UWM_ADDUSER, wLen, (LPARAM)cmd);
		break;
	case 0x06E: // user join
		::SendMessage(m_pView->m_hWnd, UWM_JOIN, wLen, (LPARAM)cmd);
		break;
	case 0x0073: // user part
		::SendMessage(m_pView->m_hWnd, UWM_PART, wLen, (LPARAM)cmd);
		break;
	case 0x0070: // user rename
		::SendMessage(m_pView->m_hWnd, UWM_RENAME, wLen, (LPARAM)cmd);
		break;
	case 0x00C9: // normal message
		::SendMessage(m_pView->m_hWnd, UWM_MESSAGE, wLen, (LPARAM)cmd);
		break;
	case 0x00CB: // Action
		::SendMessage(m_pView->m_hWnd, UWM_ACTION, wLen, (LPARAM)cmd);
		break;
	case 0x00C8: // rcms garbage
		break;
	case 0x0190: // redirect
		::PostMessage(m_pView->m_hWnd, UWM_REDIRECT, wLen, (LPARAM)cmd);
		break;
	default: // unknown
		::SendMessage(m_pView->m_hWnd, UWM_UNKNOWN, MAKEWPARAM(wType, wLen), (LPARAM)cmd);
	}

}

void CChatClient::WriteMessage(LPCTSTR lpszMsg, COLORREF rColor)
{

	if(!m_pView) return;
	if(!::IsWindow(m_pView->m_hWnd)) return;

	::SendMessage(m_pView->m_hWnd, UWM_SYSTEM, (WPARAM)rColor, (LPARAM)lpszMsg);
}
