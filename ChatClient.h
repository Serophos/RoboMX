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

#if !defined(AFX_CHATCLIENT_H__E77F22CE_3865_4876_944C_DDDE8A6555F0__INCLUDED_)
#define AFX_CHATCLIENT_H__E77F22CE_3865_4876_944C_DDDE8A6555F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MySocket.h"
#include "MXSock.h"
#pragma comment(lib, "MXSock")


#include "RoboEx.h"

#define MXCHAT_LOGIN		0x0064
#define MXCHAT_LOGINGRANTED 0x0066

#define SERVER_UNKNOWN      0x0000
#define SERVER_RCMS         0x0001
#define SERVER_WINMX353		0x0002
#define SERVER_ROBOMX		0x0003

#define USER_NORMAL			0x0000
#define USER_OPERATOR       0x0001
#define USER_VOICED         0x0002

class CMetis3View;

class ClientJoin
{
public:
	ClientJoin(){ dwNodeIP = 0; wNodePort = 0; wLine = 0; dwSrcIP = 0; dwFiles = 0; wLevel = 0;}

	CString strName;
	CString strHost;
	DWORD	dwFiles;
	DWORD	dwNodeIP;
	DWORD	dwSrcIP;
	WORD	wLevel;
	WORD	wLine;
	WORD	wNodePort;
};

class CChatClient  
{
public:
	CChatClient();
	virtual ~CChatClient();

public:
	void WriteMessage(LPCTSTR lpszMsg, COLORREF rColor);
	BOOL SendRename(CString strUser, DWORD dwFiles, WORD wLine);
	void SendMessage(LPCTSTR lpszMessage, int nLen, BOOL bAction = FALSE);
	int DecodeJoin(WORD wType, WORD wLen, char* buffer);
	void DecodeCommand(WORD wType, WORD wLen, char* buffer);
	static UINT RecvProc(PVOID pParam);
	BOOL SetRoom(CString strRoom);
	BOOL Disconnect();
	BOOL Connect();
	BOOL SendNew(LPCTSTR lpszKey);
	void Ping();
	void WriteMessage(UINT nID, COLORREF rColor);

	// Room  info
	CString m_strRoom;
	CString m_strRoomIP;
	DWORD   m_dwRoomIP;
	WORD    m_wRoomTCPPort;
	WORD	m_wNumUsers;
	CString m_strTopic;
	CString m_strMotd;
	int		m_nNumUsers;

	// User info
	CString m_strUser;
	WORD    m_wLineType;
	WORD    m_wClientUDPPort;
	DWORD   m_dwClientIP;
	DWORD   m_dwFiles;
	WORD	m_wServerType;

	BOOL    m_bOldJoin;

	// Crypt Shit
	DWORD m_dwUPKey;
	DWORD m_dwDWKey;

	// Network stuff
	CMySocket m_mSocket;
	volatile BOOL    m_bListen;
	volatile BOOL	 m_bAbort;

public:
	HWND		m_hView;
	CEvent		m_eClose;
	CWinThread *m_pThread;
	BOOL ConnectInternal(void);
	BOOL ListenInternal(void);
};

#endif // !defined(AFX_CHATCLIENT_H__E77F22CE_3865_4876_944C_DDDE8A6555F0__INCLUDED_)
