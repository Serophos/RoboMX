#pragma once

#include "MySocket.h"
#include "MXSock.h"
#include "Util.h"

#define MAXTEXTBUFFSIZE		1024
#define QUESTATUSBASE		9000

#define MAXRECVMESSAGESIZE	3072	// Max Recv Message Size
#define MAXSENDCACHENUM		32		// Max Send Message Cache Num
#define MAXSENDMESSAGESIZE	3072	// Max Send Message Size


// Thread Synchronize Message
#define WM_RECVPROCSTART		(WM_USER + 0x100)	// Recv Thread Start
#define WM_RECVPROCEND			(WM_USER + 0x101)	// Recv Thread End


// Event Notifaction-Messages
#define WPN_ERROR				0x0000	// Error

#define WPN_WPNCONNECTSTART		0x0001	// Login Start
#define WPN_WPNCONNECTED		0x0002	// Login Complete
#define WPN_WPNDISCONNECTED		0x0003	// Disconnect from Parent WPN
#define WPN_CHANGEPARENTSTART	0x0004	// Change Parent Start
#define WPN_CHANGEPARENTCOMP	0x0005	// Change Parent Complete
#define WPN_PARENTDISCONNECTED	0x0006	// Disconnect from Parent
#define WPN_CHATROOMINFO		0x0007	// Chat List Notifation

// Error codes
#define WPN_ERROR_LOGIN			0x0001	// Login Failed
#define WPN_ERROR_GETNODE		0x0002	// Get Parent Node Failed


// Frontcode
#define FRONTCODEHOSTNAME	"216.127.74.62"
#define FRONTCODESTARTHOST	0
#define FRONTCODEENDHOST	0
#define FRONTCODESTARTPORT	7952
#define FRONTCODEENDPORT	7950


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


class CWPNPClient
{
public:

	CWPNPClient(void);
	~CWPNPClient(void);

	// Public Interface
	void SetCallBackProck(RECVMESSAGEPROC *pProc, DWORD dwParam);
	
	// Connect to parent node
	int	 Connect(PARENTNODEINFO *pNodeInfo, LPCTSTR lpszUserName, WORD wLineType, WORD wTransferPort);

	// Disconnect from parent node
	BOOL Disconnect(void);

	// send a message
	BOOL SendMessage(WORD wType, WORD wLen, char *pData);

	// sends queue status to parent
	BOOL SendQueStatus(WORD wULMaxNum, WORD wULFreeNum, WORD wULQueNum);

	// get a new parent node
	BOOL RefreshWPN(void);

	BOOL SendFormatMessage(WORD wType, char *lpszFormat, ...);

	// Deprecated
	//static int ScanMessage(char *pBuff, WORD wMaxSize, char *lpszFormat, ...);

	// Public Attributes

	// Userinfo
	CString m_strBaseUserName;	// Base User Name XXXXXNNN
	CString m_strWPNUserName;	// Full User Name XXXXXNNN_MMMMM
	WORD	m_wTransferPort;	// TCP Port for File Transfer
	WORD	m_wLineType;		// Connection Line Type

	// Parent Node info
	DWORD m_dwParentIP;			// Parent Node IP IP Addresses
	WORD  m_wParentTCPPort;		// Parent Node TCP Port
	WORD  m_wParentUDPPort;		// Parent Node UDP Port
	BOOL  m_bAutoParent;		// Get Parent Node Automatically
	BOOL  m_bIsLoggedIn;		// WPN Login Status

private:

	static UINT WPNClientProc(LPVOID lParam);
	static UINT WPNSendProc(LPVOID lParam);

	BOOL GetParentNode(PARENTNODEINFO *pNodeInfo);
	BOOL LoginParent(PARENTNODEINFO *pNodeInfo, LPCTSTR lpszBaseName, CString &strWPNUserName,  WORD wLineType, WORD wTransferPort);
	BOOL ProcWPNPMessage(WORD wType, WORD wLen, char *Buff);
	void ClearSendArray(void);
	BOOL SendNotify(WPARAM wParam, LPARAM lParam);

	// Thread synchronisation
	HANDLE				m_hSendEvent;
	CRITICAL_SECTION	m_ClientLock;

	// bufers
	DWORD				m_dwSendType;
	DWORD				m_dwSendSize;			
	WPNPMESSAGE			m_SendArray[MAXSENDCACHENUM];	

	// Socket
	CMySocket m_mParentSock;		// Socket for Parent
	CMySocket m_mPeerCacheSock;		// Socket for Peer Cache Server
	DWORD m_dwUPKey;				// Send Crypt Key
	DWORD m_dwDWKey;				// Recv Crypt Key

	// Callback Function
	RECVMESSAGEPROC *m_pCallBackProc;	// Callback Function
	DWORD			m_dwParam;			// Callback parameter

	BOOL m_bKeepLogin;					// Keep Login Flag
	BOOL m_bIsWPNSendProc;				// Send Thread is Active
	BOOL m_bIsWPNClientProc;			// Recv Thread is Active
};

void PumpMessage(void);