#pragma once
#include "mysocket.h"


#define MAX_BUFFER_SIZE 1024

#define CLIENT_WINMX331    0x001
#define CLIENT_WINMX353	   0x002

#define UM_NORMAL	        0x00000000L
#define UM_PENDING          0x40000000L
#define UM_OPERATOR         0x00400000L
#define UM_SPEAKPERMIT	    0x00000400L

typedef BOOL (RECVMESSAGEPROC)(DWORD dwParam, WPARAM wParam, LPARAM lParam);


#define CC_ERROR  0x000
#define CC_RENAME 0x001
#define CC_MSG    0x002
#define CC_ACTION 0x003
#define CC_PING   0x004

class ClientNotify
{
public:
	ClientNotify(){ wType = 0; dwIP = 0; wPort = 0; };
	~ClientNotify(){};

	WORD  wType;
	DWORD dwIP;
	WORD  wPort;
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

class ClientError : public ClientNotify
{
public:
	ClientError(){ wType = CC_ERROR; dwIP = 0; wPort = 0; }
	WORD wIndex;
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
};

class CClientSocket :
	public CMySocket
{
public:
	CClientSocket(SOCKET socket);
	~CClientSocket(void);

public: // attributes

	CString m_strName;
	DWORD   m_dwSrcIP;
	WORD    m_wSrcPort;
	DWORD   m_dwIP;
	WORD    m_wPort;
	WORD    m_wLineType;
	DWORD   m_dwFiles;
	UINT    m_uMode;
	DWORD   m_dwUPKey;
	DWORD   m_dwDWKey;
	WORD    m_wClientID;
	HWND    m_hMsgTarget;
	BOOL	m_bListen;
	CEvent  m_eDone;
	WORD    m_wClientType;

	char    m_cBuffer[MAX_BUFFER_SIZE];

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
	BOOL SendAction(const CString strUser, const CString strMsg);
	BOOL SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel = 0, DWORD dwRealIP = 0);
	BOOL SendRename(const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles);
	BOOL SendPart(const CString strUser, DWORD dwIP, WORD wPort);
	BOOL SendUserlist(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel = 0);
	static UINT RecvProc(LPVOID pParam);
	
	// Note all users start as UM_PENDING and have no rights
	UINT GetUserMode(){ return m_uMode; }
	void SetUserMode(UINT uMode){ m_uMode = uMode; }
	void AddMode(UINT uMode){ m_uMode |= uMode; }
	void RemoveMode(UINT uMode){ m_uMode &= ~uMode; }

};
