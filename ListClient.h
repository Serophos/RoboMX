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

#if !defined(_WPNCLIENT_H_)
#define _WPNCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MySocket.h"
#include "MXSock.h"

// Define
#define MAXTEXTBUFFSIZE		1024
#define QUESTATUSBASE		9000

#define MAXRECVMESSAGESIZE	3072	// Max Recv Message Size
#define MAXSENDCACHENUM		32		// Max Send Message Chace Num
#define MAXSENDMESSAGESIZE	3072	// Max Send Message Size


// Thread Synchronize Message
#define WM_RECVPROCSTART		(WM_USER + 0x100)	// Recv Thread Start
#define WM_RECVPROCEND			(WM_USER + 0x101)	// Recv Thread End


#define FRONTCODEHOSTNAME	"216.127.74.62"
#define FRONTCODESTARTHOST	0
#define FRONTCODEENDHOST	0
#define FRONTCODESTARTPORT	7952
#define FRONTCODEENDPORT	7950



// WPNP Messages
#define WPN_LOGINREQUEST	0x03E9	// Login Request (Send)
#define WPN_LOGINACCEPT		0x0460	// Login Notify (Recv)
#define WPN_QUESTATUS		0x0384	// Que Status (Send)
#define WPN_CHATROOMINFO	0x238E	// Chat List Notify (Recv)
#define WPN_ENUMCHATROOMS	0x238D	// Chat List Request (Send)

// Event Notifaction-Messages
#define CLN_ERROR				0x0000	// Error

#define CLN_WPNCONNECTSTART		0x0001	// Login Start
#define CLN_WPNCONNECTED		0x0002	// Login Complete
#define CLN_WPNDISCONNECTED		0x0003	// Disconnect from Parent WPN
#define CLN_CHANGEPARENTSTART	0x0004	// Change Parent Start
#define CLN_CHANGEPARENTCOMP	0x0005	// Change Parent Complete
#define CLN_PARENTDISCONNECTED	0x0006	// Disconnect from Parent
#define CLN_CHATROOMINFO		0x0007	// Chat List Notifation


// Error codes
#define CLNC_WPNCONNECTFAILED	0x0001	// Login Failed
#define CLNC_WPNGETPARENTFAILED	0x0002	// Get Parent Node Failed


typedef BOOL (RECVMESSAGEPROC)(DWORD dwParam, WPARAM wParam, LPARAM lParam);


typedef struct TAG_PARENTNODEINFO {

	DWORD dwNodeIP;		
	WORD wUDPPort;		
	WORD wTCPPort;		
	BYTE bFreePri;		
	BYTE bFreeSec;		
	WORD wReserved;
} PARENTNODEINFO, *PPARENTNODEINFO;

typedef struct TAG_WPNPMESSAGE {

	WORD wType;			
	WORD wLen;			
	char Data[MAXSENDMESSAGESIZE];
} WPNPMESSAGE, *PWPNPMESSAGE;

typedef struct TAG_CHATROOMINFO {

	DWORD dwUsers;
	DWORD dwLimit;
	LPSTR lpszRoomName;
	LPSTR lpszTopic;
} CHATROOMINFO, *PCHATROOMINFO;


// WPNP Secondary Client Class
class CListClient  
{
public:

	CListClient();
	virtual ~CListClient();
	void SetCallBackProck(RECVMESSAGEPROC *pProc, DWORD dwParam);
	int Connect(PARENTNODEINFO *pNodeInfo, LPCTSTR lpszUserName, WORD wLineType, WORD wTransferPort);
	BOOL Disconnect(void);
	BOOL SendMessage(WORD wType, WORD wLen, char *pData);
	BOOL SendFormatMessage(WORD wType, char *lpszFormat, ...);
	static int ScanMessage(char *pBuff, WORD wMaxSize, char *lpszFormat, ...);
	BOOL SendQueStatus(WORD wULMaxNum, WORD wULFreeNum, WORD wULQueNum);
	BOOL RefreshWPN(void);

	// Dummy user info
	CString m_strBaseUserName;	// Base User Name XXXXXNNN
	CString m_strWPNUserName;	// Full User Name XXXXXNNN_MMMMM
	WORD m_wTransferPort;		// TCP Port for File Transfer
	WORD m_wLineType;			// Connection Line Type

	// Parent Node
	DWORD m_dwParentIP;			// Parent Node IP IP Addresses
	WORD m_wParentTCPPort;		// Parent Node TCP Port
	WORD m_wParentUDPPort;		// Parent Node UDP Port
	BOOL m_bAutoParent;			// Get Parent Node Automatically
	BOOL m_bIsLoggedIn;			// WPN Login Status

private:
	BOOL GetParentNode(PARENTNODEINFO *pNodeInfo);
	BOOL LoginParent(PARENTNODEINFO *pNodeInfo, LPCTSTR lpszBaseName, CString &strWPNUserName,  WORD wLineType, WORD wTransferPort);
	static UINT WPNClientProc(LPVOID lParam);
	static UINT WPNSendProc(LPVOID lParam);
	BOOL ProcWPNPMessage(WORD wType, WORD wLen, char *Buff);
	void ClearSendArray(void);
	BOOL SendNotify(WPARAM wParam, LPARAM lParam);

	// Buffer and stuff
	HANDLE m_hSendEvent;
	CRITICAL_SECTION m_ClientLock;
	DWORD m_dwSendType;
	DWORD m_dwSendSize;			
	WPNPMESSAGE m_SendArray[MAXSENDCACHENUM];	

	// Socket
	CMySocket m_mParentSock;		// Socket for Parent
	CMySocket m_mPeerCacheSock;	// Socket for Peer Cache Server
	DWORD m_dwUPKey;			// Send Crypt Key
	DWORD m_dwDWKey;			// Recv Crypt Key

	// Callback Function
	RECVMESSAGEPROC *m_pCallBackProc;	// Callback Function
	DWORD m_dwParam;					// Callback Function Option

	BOOL m_bKeepLogin;			// Keep Login Flag
	BOOL m_bIsWPNSendProc;		// Send Thread is Active
	BOOL m_bIsWPNClientProc;	// Recv Thread is Active

};

void PumpMessage(void);

#endif // !defined(_WPNCLIENT_H_)
