#pragma once
#include "mysocket.h"


#define MAX_BUFFER_SIZE		10000
#define MAX_SEND_SIZE		1024

#define CLIENT_WINMX331		0x001
#define CLIENT_WINMX353		0x002

#define UM_NORMAL	        0x0000L  // normal state, logged in no voice
#define UM_PENDING          0x1000L	 // logging in, no voice, not in channel
#define UM_OPERATOR         0x0100L  // operator 
#define UM_SPEAKPERMIT	    0x0010L  // voice
#define UM_BANNED			0x0001L  // banned

typedef BOOL (RECVMESSAGEPROC)(DWORD dwParam, WPARAM wParam, LPARAM lParam);

#define CC_ERROR  0x000
#define CC_RENAME 0x001
#define CC_MSG    0x002
#define CC_ACTION 0x003
#define CC_PING   0x004
#define CC_OPMSG  0x005

class ClientNotify
{
public:
	ClientNotify(){ wType = 0; dwIP = 0; wPort = 0; uMode = UM_NORMAL;};
	~ClientNotify(){};

	WORD  wType;
	DWORD dwIP;
	WORD  wPort;
	UINT  uMode;
	GUID  guid;
};

class ClientMessage : public ClientNotify
{
public:
	ClientMessage(){ wType = CC_MSG; dwIP = 0; wPort = 0; };
	CString strName;
	CString strText;
};

class ClientAction : public ClientNotify
{
public:
	ClientAction(){ wType = CC_ACTION; dwIP = 0; wPort = 0; };
	CString strName;
	CString strText;
};

class ClientOpMsg : public ClientNotify
{
public:
	ClientOpMsg(){ wType = CC_OPMSG; dwIP = 0; wPort = 0; };
	CString strName;
	CString strText;
};

class ClientError : public ClientNotify
{
public:
	ClientError(){ wType = CC_ERROR; dwIP = 0; wPort = 0; }
	GUID    guID;
	CString strCause;
};

class ClientRename : public ClientNotify
{

public:
	ClientRename(){ wType = CC_RENAME; dwIP = 0; wPort = 0; };
	CString  strOldName;
	DWORD    dwOldIP;
	WORD     wOldPort;
	WORD     wOldLine;
	DWORD    dwOldFiles;
	CString  strNewName;
	DWORD    dwNewIP;
	WORD	 wNewPort;
	WORD     wNewLine;
	DWORD    dwNewFiles;
	WORD	 wUserLevel;
};

class CClientSocket :
	public CMySocket
{
public:
	CClientSocket(SOCKET socket);
	~CClientSocket(void);

public: // attributes

	CString m_strName;
	CString m_strSrcHost;
	DWORD   m_dwSrcIP;
	WORD    m_wSrcPort;
	DWORD   m_dwIP;
	WORD    m_wPort;
	WORD    m_wLineType;
	DWORD   m_dwFiles;
	UINT    m_uMode;
	DWORD   m_dwUPKey;
	DWORD   m_dwDWKey;
	//WORD    m_wClientID;
	GUID	m_guID;
	HWND    m_hMsgTarget;
	BOOL	m_bListen;
	CEvent  m_eDone;
	WORD    m_wClientType;

	char    m_cBuffer[MAX_BUFFER_SIZE];
	CWinThread *m_pThread;

public: // Interface
	BOOL HandShake(const CString strRoom);
	BOOL HighlevelHandshake(const CString strKey);
	void StartUp(void);
	void LogOut(void);
	BOOL SendNotify(WPARAM wParam, LPARAM lParam);
	
	void HandlePacket(WORD wType, WORD wLen, char* buffer);

	int SendCrypted(char *pBuff, int nLen, int nWait);

	BOOL SendTopic(const CString strTopic);
	BOOL SendMotd(const CString strMotd);
	BOOL SendMsg(const CString strUser, const CString strMsg);
	BOOL SendOperator(const CString strUser, const CString strMsg, BOOL bEcho);
	BOOL SendAction(const CString strUser, const CString strMsg);
	BOOL SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel = 0, DWORD dwRealIP = 0);
	BOOL SendRename(const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel = 0);
	BOOL SendPart(const CString strUser, DWORD dwIP, WORD wPort);
	BOOL SendUserlist(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel = 0);
	static UINT RecvProc(LPVOID pParam);
	
	// Note all users start as UM_PENDING and have no rights
	UINT GetUserMode(){ return m_uMode; }
	void SetUserMode(UINT uMode){ m_uMode = uMode; }
	void AddMode(UINT uMode);
	void RemoveMode(UINT uMode);

	BOOL SendChannelRename(CString strRoom);
};
