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
#include "MySocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySocket::CMySocket()
{

	m_sSocket	  = INVALID_SOCKET;
	m_dwLastError = SOCK_NOERROR;
}

CMySocket::~CMySocket()
{

	Close();
}

BOOL CMySocket::Connect(LPCTSTR lpszIP, WORD wPort, int nWait)
{

	DWORD dwIP = inet_addr(lpszIP);

	if(dwIP == INADDR_NONE){

		struct hostent *pHost = gethostbyname(lpszIP);
		if(!pHost){		

			m_dwLastError = SOCK_SOCKERROR;
			return FALSE;
		}
		else{
			
			return Connect(*((DWORD *)pHost->h_addr_list[0]), wPort, nWait);
		}
	}
	else{
		
		return Connect(dwIP, wPort);
	}
}

BOOL CMySocket::Connect(DWORD dwIP, WORD wPort, int nWait)
{

	// already connected?
	if(m_sSocket != INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;
	}

	m_sSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;		
	}

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(wPort);
	SockAddr.sin_addr.S_un.S_addr = dwIP;

	if(!nWait){

		if(connect(m_sSocket, (PSOCKADDR)&SockAddr, sizeof(sockaddr))){

			Close();
			m_dwLastError = SOCK_SOCKERROR;
			return FALSE;		
		}
	
		m_dwLastError = SOCK_NOERROR;
		return TRUE;
	}

	unsigned long nArg = 1;
	fd_set FdSet;
	struct timeval TimeVal;
	TimeVal.tv_sec = nWait;
	TimeVal.tv_usec = 0;
	FD_ZERO(&FdSet);
	FD_SET(m_sSocket, &FdSet);

	if(ioctlsocket(m_sSocket, FIONBIO, &nArg) == SOCKET_ERROR){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;		
	}

	if(connect(m_sSocket, (PSOCKADDR)&SockAddr, sizeof(sockaddr)) != SOCKET_ERROR){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;		
	}
	if(WSAGetLastError() != WSAEWOULDBLOCK){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;		
	}

	if(select(32, NULL, &FdSet, NULL, &TimeVal) == SOCKET_ERROR){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;		
	}

	if(!FD_ISSET(m_sSocket, &FdSet)){

		Close();
		m_dwLastError = SOCK_TIMEOUT;
		return FALSE;
	}

	nArg = 0;
	if(ioctlsocket(m_sSocket, FIONBIO, &nArg) == SOCKET_ERROR){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;		
	}

	m_dwLastError = SOCK_NOERROR;
	return TRUE;
}

int CMySocket::Recv(char *pBuff, int nLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return SOCKET_ERROR;
	}

	int nRef = 0, nRecv = 0;

	do{

		nRef = RecvOnce(pBuff + nRecv, nLen - nRecv, nWait);
		if(nRef == SOCKET_ERROR) return SOCKET_ERROR;
		nRecv += nRef;
	}
	while(nRecv < nLen);

	return nRecv;
}

int CMySocket::RecvOnce(char *pBuff, int nMaxLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return SOCKET_ERROR;
	}
	
	// Set Socket-Timeout
	int nValue  = 0, nSize = sizeof(int);

	if(getsockopt(m_sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nValue, &nSize)){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return SOCKET_ERROR;
	}

	if(nValue != (nWait * 1000)){

		nValue = nWait * 1000;
		if(setsockopt(m_sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nValue, sizeof(int))){
			Close();
			m_dwLastError = SOCK_SOCKERROR;
			return SOCKET_ERROR;
		}
	}
	
	// Receive data :-)
	int nRef = recv(m_sSocket, pBuff, nMaxLen, 0);
	if(nRef == SOCKET_ERROR || !nRef){

		DWORD dwLastError = (WSAGetLastError() == WSAETIMEDOUT) ? SOCK_TIMEOUT : SOCK_SOCKERROR;
		if(dwLastError != SOCK_TIMEOUT){

			Close();
		}
		m_dwLastError = dwLastError;
		return SOCKET_ERROR;
	}
		
	return nRef;
}


int CMySocket::Send(char *pBuff, int nLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return SOCKET_ERROR;
	}

	int nRef = 0, nSend = 0;

	do{

		nRef = SendOnce(pBuff + nSend, nLen - nSend, nWait);
		if(nRef == SOCKET_ERROR){

			return SOCKET_ERROR;
		}
		nSend += nRef;
	}
	while(nSend < nLen);

	return nSend;
}

int CMySocket::SendOnce(char *pBuff, int nMaxLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return SOCKET_ERROR;
	}
	
	// Set timeout
	int nValue  = 0, nSize = sizeof(int);
	if(getsockopt(m_sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nValue, &nSize)){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return SOCKET_ERROR;
	}
	if(nValue != (nWait * 1000)){

		nValue = nWait * 1000;
		if(setsockopt(m_sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nValue, sizeof(int))){

			Close();
			m_dwLastError = SOCK_SOCKERROR;
			return SOCKET_ERROR;
		}
	}
	
	// send...
	int nRef = send(m_sSocket, pBuff, nMaxLen, 0);
	if(nRef == SOCKET_ERROR || !nRef){

		DWORD dwLastError = (WSAGetLastError() == WSAETIMEDOUT) ? SOCK_TIMEOUT : SOCK_SOCKERROR;
		if(dwLastError != SOCK_TIMEOUT){

			Close();
		}
		m_dwLastError = dwLastError;
		return SOCKET_ERROR;
	}
		
	return nRef;
}


BOOL CMySocket::Close(void)
{

	m_dwLastError = SOCK_NOERROR;

	// check if we are actually connected ;)
	if(m_sSocket != INVALID_SOCKET){

		char Char;

		shutdown(m_sSocket, SD_BOTH);
		while(recv(m_sSocket, &Char, 1, 0) == 1); // recv left data...
		closesocket(m_sSocket);
		m_sSocket = INVALID_SOCKET;
	}
	else{
		
		m_dwLastError = SOCK_SOCKINVALID;
	}

	return (m_dwLastError == SOCK_NOERROR) ? TRUE : FALSE;
}

DWORD CMySocket::GetLastError(void)
{

	return m_dwLastError;
}

CString CMySocket::GetLastErrorStr(void)
{

	CString strError;
	switch(GetLastError()){

	case SOCK_NOERROR:
		strError = "No error occured";
		break;
	case SOCK_SOCKERROR:
		strError = "Socket error";
		break;
	case SOCK_TIMEOUT:
		strError = "Connection timeout";
		break;
	case SOCK_SOCKINVALID:
		strError = "Invalid Socket";
		break;
	default:
		strError = "Unknown Error";
	}
	return strError;
}


BOOL CMySocket::GetLocalAddr(DWORD *pIP, WORD *pPort)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return FALSE;
	}

	struct sockaddr_in LocalAddr;
	int AddrLen = sizeof(LocalAddr);
	
	if(getsockname(m_sSocket, (struct sockaddr *)&LocalAddr, &AddrLen) == SOCKET_ERROR){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;
	}

	*pIP = LocalAddr.sin_addr.S_un.S_addr;
	*pPort = ntohs(LocalAddr.sin_port);

	return TRUE;
}

BOOL CMySocket::GetPeerAddr(DWORD *pIP, WORD *pPort)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return FALSE;
	}

	struct sockaddr_in PeerAddr;
	int AddrLen = sizeof(PeerAddr);
	
	if(getpeername(m_sSocket, (struct sockaddr *)&PeerAddr, &AddrLen) == SOCKET_ERROR){

		Close();
		m_dwLastError = SOCK_SOCKERROR;
		return FALSE;
	}

	*pIP = PeerAddr.sin_addr.S_un.S_addr;
	*pPort = ntohs(PeerAddr.sin_port);

	return TRUE;
}

