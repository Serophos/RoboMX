/*
** Copyright (C) 2004 Thees Winkler
**    
** Based on code released into public domain by Nushi@Matari
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
#include "ListClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Instructor / Destructor
//////////////////////////////////////////////////////////////////////

CListClient::CListClient()
{

	m_dwUPKey			= 0;
	m_dwDWKey			= 0;
	m_wTransferPort		= 0;
	m_wLineType			= 0x00;  // Unknown
	m_wParentTCPPort	= 0;
	m_wParentUDPPort	= 0;
	m_dwParentIP		= 0;
	m_bAutoParent		= TRUE;
	m_strWPNUserName	= "";
	m_strBaseUserName	= "";
	m_bKeepLogin		= TRUE;
	m_bIsLoggedIn			= FALSE;
	m_bIsWPNClientProc	= FALSE;
	m_bIsWPNSendProc	= FALSE;
	m_pCallBackProc		= NULL;
	m_dwParam			= NULL;

	m_hSendEvent		= NULL;
	m_dwSendType		= 0;
	m_dwSendSize		= 0;

	ZeroMemory(m_SendArray, sizeof(m_SendArray));

	// Initialize Critical Section
	::InitializeCriticalSection(&m_ClientLock);
}

CListClient::~CListClient()
{

	Disconnect();
	
	// Delete Critical Section
	::DeleteCriticalSection(&m_ClientLock);
}

void CListClient::SetCallBackProck(RECVMESSAGEPROC *pProc, DWORD dwParam)
{

	m_pCallBackProc = pProc;
	m_dwParam		= dwParam;
}

int CListClient::Connect(PARENTNODEINFO *pNodeInfo, LPCTSTR lpszUserName, WORD wLineType, WORD wTransferPort)
{

	// Check Parameters
	if(!lpszUserName || m_bIsWPNClientProc || m_bIsWPNSendProc) return 0;
	if(!lstrlen(lpszUserName)) return 0;

	::EnterCriticalSection(&m_ClientLock);
	{

		// Save Parameter
		m_bKeepLogin = TRUE;
		m_bAutoParent = (pNodeInfo)? FALSE : TRUE;
		m_strBaseUserName = lpszUserName;
		m_strWPNUserName = "";
		m_wLineType = wLineType;
		m_wTransferPort = wTransferPort;
		
		if(pNodeInfo){

			m_dwParentIP = pNodeInfo->dwNodeIP;
			m_wParentTCPPort = pNodeInfo->wTCPPort;
			m_wParentUDPPort = pNodeInfo->wUDPPort;
		}

		m_bIsWPNClientProc = FALSE;
		CWinThread *pWPNClientThread = AfxBeginThread(CListClient::WPNClientProc, this);
		if(!pWPNClientThread){

			::LeaveCriticalSection(&m_ClientLock);
			return 0;
		}

		while(!m_bIsWPNClientProc){
			
			Sleep(1);
		}

	}
	::LeaveCriticalSection(&m_ClientLock);

	return 1;
}

BOOL CListClient::GetParentNode(PARENTNODEINFO *pNodeInfo)
{

	if(!pNodeInfo) return FALSE;

	char buffer[512] = {'\0'};
	BYTE *pNodeBuffer = (BYTE *)buffer + sizeof(buffer) / 2;
	PARENTNODEINFO pnFreeNode;
	int i = 0, j = 0, nFree = 0;
	TCHAR szHostName[256] = {'\0'};
	WORD wHostIndex = 0;
	WORD wPortIndex = 0;


	for(wPortIndex = FRONTCODESTARTPORT ; wPortIndex >= FRONTCODEENDPORT && 
		  m_bKeepLogin; wPortIndex--){
	
		for(wHostIndex = FRONTCODESTARTHOST ; wHostIndex <= FRONTCODEENDHOST && 
			  m_bKeepLogin ; wHostIndex++){

			wsprintf(szHostName, FRONTCODEHOSTNAME, wHostIndex);
		
			m_mPeerCacheSock.Close();
			if(!m_mPeerCacheSock.Connect(szHostName, wPortIndex, 5)){

				// We failed to connect. use alternate port
				continue;
			}

			if(m_mPeerCacheSock.Recv(buffer, 1, 5) != 1)continue;
			if(buffer[0] != 0x38)continue;

			if(m_mPeerCacheSock.Recv(buffer, 16, 5) != 16)continue;
			if(GetCryptKeyID((BYTE *)buffer) != 0x54)continue;

			// Get free nodes
			if(m_mPeerCacheSock.Recv(buffer, 132, 5) != 132)continue;

			m_mPeerCacheSock.Close();

			// Decrypt Node Information
			DecryptFrontCode((BYTE *)buffer, pNodeBuffer);
			memcpy(pNodeInfo, pNodeBuffer, 120);
	
			// Sort by Free Secondary Num
			for(j = 0 ; j < 9 ; j++){

				nFree = j;

				for(i = j + 1 ; i < 10 ; i++){

					if(pNodeInfo[i].bFreeSec > pNodeInfo[nFree].bFreeSec || 
						(pNodeInfo[i].bFreeSec == pNodeInfo[nFree].bFreeSec && 
						   pNodeInfo[i].bFreePri < pNodeInfo[nFree].bFreePri)){
						
						nFree = i;
					}
				}

				pnFreeNode = pNodeInfo[j];
				pNodeInfo[j] = pNodeInfo[nFree];
				pNodeInfo[nFree] = pnFreeNode;
			}

			// Retry if No Free Parent Node
			if(!pNodeInfo[0].bFreeSec){

				continue;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CListClient::LoginParent(PARENTNODEINFO *pNodeInfo, LPCTSTR lpszBaseName, CString &strWPNUserName, WORD wLineType, WORD wTransferPort)
{


	m_bIsLoggedIn = FALSE;

	if(!lpszBaseName || !pNodeInfo) return FALSE;
	if(!lstrlen(lpszBaseName)) return FALSE;

	int nNameLen = lstrlen(lpszBaseName);
	char buffer[1024] = {'\0'};

	if(!m_mParentSock.Connect(pNodeInfo->dwNodeIP, pNodeInfo->wTCPPort, 5)) return FALSE;

	if(m_mParentSock.Recv(buffer, 1, 5) != 1) return FALSE;

	if(buffer[0] != 0x31) return FALSE;

	// Send UP Key Block
	CreateCryptKeyID(0x52, (BYTE *)buffer);
	if(m_mParentSock.Send(buffer, 16, 5) != 16) return FALSE;

	// Recv DW Key Block
	if(m_mParentSock.Recv(buffer, 16, 5) != 16) return FALSE;
	if(GetCryptKeyID((BYTE *)buffer) != 0x53) return FALSE;

	GetCryptKey((BYTE *)buffer, &m_dwUPKey, &m_dwDWKey);

	// Send Login Request
	lstrcpy(buffer + 4, lpszBaseName);
	memcpy(buffer + nNameLen + 5, &wLineType, 2);	
	memcpy(buffer + nNameLen + 7, &wTransferPort, 2);	
	*(WORD *)buffer = WPN_LOGINREQUEST;
	*(WORD *)(buffer + 2) = (WORD)nNameLen + 5;
	nNameLen += 9;
	m_dwUPKey = EncryptMXTCP((BYTE *)buffer, nNameLen, m_dwUPKey);

	if(m_mParentSock.Send(buffer, nNameLen, 5) != nNameLen){

		return FALSE;
	}

	// Recv User ID
	if(m_mParentSock.Recv(buffer, 6, 5) != 6) return FALSE;

	m_dwDWKey = DecryptMXTCP((BYTE *)buffer, 6, m_dwDWKey);

	if((*(WORD*)buffer != WPN_LOGINACCEPT) || (*(WORD*)(buffer + 2) != 2)) return FALSE;

	// Make User Name
	strWPNUserName.Format("%s_%05u", lpszBaseName, *(WORD*)(buffer + 4));

	// Save Parent Node Information
	m_wTransferPort  = wTransferPort;
	m_wLineType      = wLineType;
	m_wParentTCPPort = pNodeInfo->wTCPPort;
	m_wParentUDPPort = pNodeInfo->wUDPPort;
	m_dwParentIP	 = pNodeInfo->dwNodeIP;

	return TRUE;
}

UINT CListClient::WPNClientProc(LPVOID lParam)
{

	CListClient *pClient = (CListClient *)lParam;

	int i = 0, nRef = 0;
	BOOL bIsFirst = TRUE;
	char buffer[MAXRECVMESSAGESIZE] = {'\0'};
	WORD wMsgLen = 0;
	WORD wMsgType = 0;

	PARENTNODEINFO NodeInfo[10];
	ZeroMemory(NodeInfo, sizeof(NodeInfo));


	srand(GetTickCount());

	// Begin Send Thread
	CWinThread *pWPNSendThread = AfxBeginThread(CListClient::WPNSendProc, pClient);
	if(!pWPNSendThread){

		// End Recv Thread
		pClient->m_bKeepLogin = FALSE;
		pClient->m_bIsWPNClientProc = TRUE;
		return 0;
	}

	// Wait for Send Thread Start
	while(!pClient->m_bIsWPNSendProc)Sleep(1);
	pClient->m_bIsWPNClientProc = TRUE;

	pClient->SendNotify(CLN_WPNCONNECTSTART, 0);

	while(pClient->m_bKeepLogin){
		if(pClient->m_bAutoParent){

			if(!bIsFirst)pClient->SendNotify(CLN_CHANGEPARENTSTART, 0);

			while(pClient->m_bKeepLogin){

				if(!pClient->GetParentNode(NodeInfo)){

					if(pClient->m_bKeepLogin)pClient->SendNotify(CLN_ERROR, CLNC_WPNGETPARENTFAILED);
					Sleep(5000);
					continue;
				}

				// Try Login Parent
				for(i = 0 ; i < 10 && pClient->m_bKeepLogin ; i++){

					if(pClient->LoginParent(&NodeInfo[i], pClient->m_strBaseUserName, pClient->m_strWPNUserName, pClient->m_wLineType, pClient->m_wTransferPort))break;
					pClient->m_mParentSock.Close();
				}
	
				// Judge Login Success
				if(i < 10 && pClient->m_bKeepLogin){

					pClient->m_bIsLoggedIn = TRUE;
					break;
				}
				else{

					if(pClient->m_bKeepLogin){

						pClient->SendNotify(CLN_ERROR, CLNC_WPNCONNECTFAILED);
						continue;
					}
					else break;
				}
			}
			if(!pClient->m_bKeepLogin) break;
		}
		else{

			// Manual Parent Mode
			NodeInfo[0].dwNodeIP = pClient->m_dwParentIP;
			NodeInfo[0].wTCPPort = pClient->m_wParentTCPPort;
			NodeInfo[0].wUDPPort = pClient->m_wParentUDPPort;

			// Notify (Login Start)
			if(!bIsFirst)pClient->SendNotify(CLN_WPNCONNECTSTART, 0);

			while(pClient->m_bKeepLogin){

				if(!pClient->LoginParent(&NodeInfo[0], pClient->m_strBaseUserName, pClient->m_strWPNUserName, pClient->m_wLineType, pClient->m_wTransferPort)){

					pClient->m_mParentSock.Close();

					if(pClient->m_bKeepLogin){

						pClient->SendNotify(CLN_ERROR, CLNC_WPNCONNECTFAILED);
						Sleep(5000);
						continue;
					}
					else{
						
						break;
					}
				}
				else{

					pClient->m_bIsLoggedIn = TRUE;
					break;
				}
			}
			
			if(!pClient->m_bIsLoggedIn){
				
				continue;
			}
		}

		// Clear Ringbuffer
		pClient->ClearSendArray();
		
		// Notify (Login Complete)
		if(pClient->m_bAutoParent){

			pClient->SendNotify((bIsFirst)? CLN_WPNCONNECTED : CLN_CHANGEPARENTCOMP, 0);
		}
		else{
			
			pClient->SendNotify(CLN_WPNCONNECTED, 0);
		}
		bIsFirst = FALSE;

		// WPNP Message Recv Loop
		while(pClient->m_bKeepLogin){

			// Recv Header
			nRef = pClient->m_mParentSock.Recv(buffer, 4);
			if(nRef != 4){

				if(pClient->m_mParentSock.GetLastError() == SOCK_TIMEOUT){

					continue;
				}
				else break;
			}

			// Decrypt Header
			pClient->m_dwDWKey = DecryptMXTCP((BYTE *)buffer, 4, pClient->m_dwDWKey);
			wMsgType = *(WORD *)buffer;
			wMsgLen = *(WORD *)(buffer + 2);
			if(!wMsgLen){
				
				continue;
			}
			else if(wMsgLen >= sizeof(buffer)){
				
				break;
			}
			
			// Recv Data
			if(pClient->m_mParentSock.Recv(buffer, wMsgLen, 5) == wMsgLen){

				// Decrypt Data
				pClient->m_dwDWKey = DecryptMXTCP((BYTE *)buffer, wMsgLen, pClient->m_dwDWKey);
			
				// Analysis Message
				pClient->ProcWPNPMessage(wMsgType, wMsgLen, buffer);
				
				// Clear Buffer
				ZeroMemory(buffer, sizeof(buffer));
			}
			else break;
		}

		// Disconnect
		pClient->m_mParentSock.Close();
		pClient->m_bIsLoggedIn = FALSE;
		::SetEvent(pClient->m_hSendEvent);
		pClient->SendNotify(CLN_PARENTDISCONNECTED, 0);
	}
	
	// End Send Thread
	::SetEvent(pClient->m_hSendEvent);
	while(pClient->m_bIsWPNSendProc)Sleep(1);

	pClient->m_strWPNUserName = "";

	// Thread End
	pClient->SendNotify(CLN_WPNDISCONNECTED, 0);
	pClient->m_bIsWPNClientProc = FALSE;

	return 0;
}

UINT CListClient::WPNSendProc(LPVOID lParam)
{

	CListClient *pClient = (CListClient *)lParam;
	WORD wType = 0, wLen = 0;	
	char buffer[MAXSENDMESSAGESIZE] = {'\0'};
	srand(GetTickCount());

	// Initialize Event
	pClient->m_hSendEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send Thread is Active
	pClient->m_bIsWPNSendProc = TRUE;

	// Initialize Event Failed
	if(!pClient->m_hSendEvent){

		// End Recv Thread
		pClient->m_bKeepLogin = FALSE;
		pClient->m_mParentSock.Close();
	}

	// Send Loop
	while(pClient->m_bKeepLogin){

		if(::WaitForSingleObject(pClient->m_hSendEvent, INFINITE) != WAIT_OBJECT_0){

			// End Recv Thread
			pClient->m_bKeepLogin = FALSE;
			pClient->m_mParentSock.Close();
			}

		// Signal Status
		while(pClient->m_bKeepLogin && pClient->m_bIsLoggedIn){

			::EnterCriticalSection(&pClient->m_ClientLock);
			if(pClient->m_dwSendSize){

				// Get Send Data
				wType = pClient->m_SendArray[pClient->m_dwSendType].wType;
				wLen = pClient->m_SendArray[pClient->m_dwSendType].wLen;
				memcpy(buffer + sizeof(WORD) * 2, pClient->m_SendArray[pClient->m_dwSendType].Data, wLen);

				// Upadate Ring Buffer
				pClient->m_dwSendType++;
				pClient->m_dwSendSize--;
				if(pClient->m_dwSendType >= MAXSENDCACHENUM)pClient->m_dwSendType = 0;
				::LeaveCriticalSection(&pClient->m_ClientLock);
			}
			else{

				::LeaveCriticalSection(&pClient->m_ClientLock);
				break;
			}
			
			// Encrypt
			*(WORD*)(buffer) = wType;
			*(WORD*)(buffer + sizeof(WORD)) = wLen;
			wLen += (sizeof(WORD) * 2);
			pClient->m_dwUPKey = EncryptMXTCP((BYTE *)buffer, wLen, pClient->m_dwUPKey);
			
			// Send
			if(pClient->m_mParentSock.Send(buffer, wLen, 5) != wLen){

				// Send Failed
				pClient->m_bKeepLogin = FALSE;
				pClient->m_mParentSock.Close();
				break;
			}
			
		}		
	}
	
	// Thread End
	pClient->ClearSendArray();
	if(pClient->m_hSendEvent)::CloseHandle(pClient->m_hSendEvent);
	pClient->m_bIsWPNSendProc = FALSE;

	return 0;
}

BOOL CListClient::Disconnect()
{

	if(!m_bIsWPNClientProc) return FALSE;

	::EnterCriticalSection(&m_ClientLock);
	m_bKeepLogin = FALSE;
	m_mPeerCacheSock.Close();
	m_mParentSock.Close();
	while(m_bIsWPNClientProc) PumpMessage();
	::LeaveCriticalSection(&m_ClientLock);

	return TRUE;
}

BOOL CListClient::ProcWPNPMessage(WORD wType, WORD wLen, char *buffer)
{

	if(wType == WPN_CHATROOMINFO){

		CHATROOMINFO RoomInfo = {0, 0, NULL, NULL};
		if(ScanMessage(buffer, wLen, "DDSS", &RoomInfo.dwUsers, &RoomInfo.dwLimit, &RoomInfo.lpszRoomName, &RoomInfo.lpszTopic) == 4){

			SendNotify(CLN_CHATROOMINFO, (LPARAM)&RoomInfo);
		}
	}		
	return TRUE;
}

void CListClient::ClearSendArray()
{

	// Clear Send Ring Buffer
	m_dwSendType = 0;
	m_dwSendSize = 0;
	ZeroMemory(m_SendArray, sizeof(m_SendArray));
}

BOOL CListClient::SendMessage(WORD wType, WORD wLen, char *pData)
{

	if(!m_bIsLoggedIn) return FALSE;
	if(m_dwSendSize >= MAXSENDCACHENUM) return FALSE;

	::EnterCriticalSection(&m_ClientLock);
	{

		// Add to Send Ring Buffer
		DWORD dwPos = m_dwSendType + m_dwSendSize;
		if(dwPos >= MAXSENDCACHENUM)dwPos -= MAXSENDCACHENUM;
		m_dwSendSize++;
		
		// Set Data
		m_SendArray[dwPos].wType = wType;
		m_SendArray[dwPos].wLen  = wLen;
		if(wLen)memcpy(m_SendArray[dwPos].Data, pData, wLen);

		::LeaveCriticalSection(&m_ClientLock);

		// Send Start
		::SetEvent(m_hSendEvent);
	}
	return TRUE;
}

BOOL CListClient::SendFormatMessage(WORD wType, char *lpszFormat, ...)
{

	if(!m_bIsLoggedIn) return FALSE;
	if(m_dwSendSize >= MAXSENDCACHENUM) return FALSE;

	::EnterCriticalSection(&m_ClientLock);
	{

		// Add to Send Ring Buffer
		DWORD dwPos = m_dwSendType + m_dwSendSize;
		if(dwPos >= MAXSENDCACHENUM){
			
			dwPos -= MAXSENDCACHENUM;
		}
		m_dwSendSize++;
		
		// Data Format
		va_list Args;
		va_start(Args, lpszFormat);
		WORD wLen = 0;
		char *lpText = NULL;
		char *pBuff = m_SendArray[dwPos].Data;

		ZeroMemory(m_SendArray[dwPos].Data, sizeof(m_SendArray[dwPos].Data));

		while(*lpszFormat){

			switch(*lpszFormat){

				case 'S' :	// NULL Terminated String
					lpText = va_arg(Args, char *);
					lstrcpy(pBuff + wLen, lpText);
					wLen += (lstrlen(lpText) + 1);
					break;
				case 's' :	// Not NULL Terminated String
					lpText = va_arg(Args, char *);
					lstrcpy(pBuff + wLen, lpText);
					wLen += (lstrlen(lpText));
					break;
				case 'B' :	// BYTE
					pBuff[wLen] = va_arg(Args, BYTE);
					wLen += 1;
					break;
				case 'W' :	// WORD
					*((WORD *)(pBuff + wLen)) = va_arg(Args, WORD);
					wLen += 2;
					break;
				case 'D' :	// DWORD
					*((DWORD *)(pBuff + wLen)) = va_arg(Args, DWORD);
					wLen += 4;
					break;
				default :
					ASSERT(0);
					break;
			}
		
			lpszFormat++;
		}

		pBuff[wLen] = 0;
		va_end(Args);

		// Set Data
		m_SendArray[dwPos].wType = wType;
		m_SendArray[dwPos].wLen  = wLen;

	}
	::LeaveCriticalSection(&m_ClientLock);

	// Send Start
	::SetEvent(m_hSendEvent);

	return TRUE;
}

int CListClient::ScanMessage(char *pBuff, WORD wMaxSize, char *lpszFormat, ...)
{

	ASSERT(pBuff && wMaxSize && lpszFormat);

	va_list Args;
	va_start(Args, lpszFormat);

	WORD wPos = 0;
	int iCount = 0;
	char *lpText = NULL;

	while(*lpszFormat && wPos < wMaxSize){

		switch(*lpszFormat){

			case 'S' :
				lpText = pBuff + wPos;
				*va_arg(Args, char **) = lpText;
				wPos += (lstrlen(lpText) + 1);
				iCount++;
				break;
			case 'B' :
				*va_arg(Args, BYTE *) = *((BYTE *)(pBuff + wPos));
				wPos += 1;
				iCount++;
				break;
			case 'W' :
				*va_arg(Args, WORD *) = *((WORD *)(pBuff + wPos));
				wPos += 2;
				iCount++;
				break;
			case 'D' :
				*va_arg(Args, DWORD *) = *((DWORD *)(pBuff + wPos));
				wPos += 4;
				iCount++;
				break;
			default :
				ASSERT(0);
				break;
		}
	
		lpszFormat++;
	}

	va_end(Args);

	return iCount;
}

BOOL CListClient::SendQueStatus(WORD wULMaxNum, WORD wULFreeNum, WORD wULQueNum)
{

	// Send Que Status
	if(!m_bIsLoggedIn) return FALSE;

	char szQueStatus[MAXTEXTBUFFSIZE] = "";
	
	if(wULFreeNum){
		
		sprintf(szQueStatus, "%u %u of 1 available", QUESTATUSBASE + wULFreeNum - wULQueNum, wULFreeNum, wULMaxNum);
	}
	else{
		
		sprintf(szQueStatus, "%u %u in queue (%u of %u available)", QUESTATUSBASE + wULFreeNum - wULQueNum, wULQueNum, wULFreeNum, wULMaxNum);
	}

	return SendFormatMessage(WPN_QUESTATUS, "S", szQueStatus);
}

BOOL CListClient::SendNotify(WPARAM wParam, LPARAM lParam)
{

	if(!m_pCallBackProc) return FALSE;
	return m_pCallBackProc((DWORD)this, wParam, lParam);
}

BOOL CListClient::RefreshWPN()
{

	if(!m_bIsWPNClientProc) return FALSE;

	if(m_bIsLoggedIn){
		
		m_mParentSock.Close();
	}
	else{
		
		m_mPeerCacheSock.Close();
	}

	return TRUE;
}

void PumpMessage(void)
{

	MSG Msg;

	while(::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)){

		if(!AfxGetThread()->PumpMessage()){

			AfxPostQuitMessage(Msg.wParam);
			return;
		}
	}
}

void WaitThreadMessage(UINT nMessage, WPARAM *pwParam, LPARAM *plParam)
{

	MSG Msg;

	do{

		while(!PeekMessage(&Msg, (HWND)-1, 0, 0, PM_NOREMOVE)) Sleep(1);
	}
	while(Msg.message != nMessage);

	if(pwParam && plParam){

		*pwParam = Msg.wParam;
		*plParam = Msg.lParam;
	}
}

