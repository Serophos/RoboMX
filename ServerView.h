#pragma once

#include "MXSock.h"
#include "MySocket.h"
// CServerView view
#include "ClientSocket.h"
#include "RichDocument.h"
#include "RichEditExCtrl.h"
#include "MyEdit.h"

#include <queue>
using std::queue;

// Channel modes:
#define CM_NORMAL	        0x00000000L
#define CM_OPTOPIC          0x40000000L
#define CM_KEYWORD          0x00400000L
#define CM_MODERATED        0x00004000L

typedef struct _TAG_BAN
{

	CString strName;
	CString strIP;
	CString strHost;
} BAN, *PBAN;

typedef  CArray<CClientSocket*, CClientSocket*> CClients;
typedef  CArray<BAN, BAN> CBans;

class CServerView : public CFormView
{
	DECLARE_DYNCREATE(CServerView)
public:
	enum { IDD = IDD_SERVER_VIEW };

protected:
	CServerView();           // protected constructor used by dynamic creation
	virtual ~CServerView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected: // overridables
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX); 

protected:
	afx_msg LRESULT ClientCallback(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT LoadSettings(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInput(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//afx_msg LRESULT OnNewClient(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public: // attributes
	CRichEditExCtrl		m_rSys;
	CRichDocument		m_rSysDoc;
	CMyEdit				m_eInput;
	CString				m_strInput;
	
	CFont	  m_fFont;
	CMySocket m_mServer;
	BOOL	  m_bHosted;
	CEvent	  m_eDone;
	CEvent    m_eHosted;
	CEvent    m_eNotifyDone;
	BOOL	  m_bShutdown;
	CString   m_strRoomBase;
	CString   m_strRoomFull;
	DWORD	  m_dwIP;
	WORD	  m_wPort;
	DWORD	  m_dwLimit;
	CString	  m_strTopic;
	CString   m_strMotd;
	CClients  m_aClients;
	CBans	  m_aBans;
	UINT      m_uMode;
	CString   m_strKeyword;
	SOCKET    m_sIn;
	CWinThread *m_pServerThread;
	CWinThread *m_pNotifyThread;
	
	queue<ClientNotify*> m_qNotifies;

public:
	CRITICAL_SECTION m_csLock;
	void WriteText(LPCTSTR lpszText);
	void WriteText(UINT nID, ...);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void HandleCommand(CString strCmd);
	void PrintCmdNotUnderstood(const CString strCmd = "");
	void PrintHelp(void);
	void OnCloseDocument();
	
	void SetTopic(const CString strTopic);
	void SetMotd(const CString strMotd);
	void CheckClients(void);

	void ReCalcLayout();

	// Broadcast Send
	void SendTopic();
	void SendMotd();
	void SendMsg(CString strUser, CString strMsg);
	void SendOpMsg(CString strUser, CString strMsg);
	void SendAction(CString strUser, CString strMsg);
	void SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP);
	void SendPart(const CString strUser, DWORD dwIP, WORD wPort);
	void SendRename(DWORD dwSrcIP, WORD wSrcPort, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel);
	void SendMode(const CString strSender, const CString strMode);
	void ServerAction(const CString strSender, const CString strMsg);

	void Stop(void);

	static UINT ServerThread(LPVOID pParam);
	static UINT LoginThread(LPVOID pParam);
	static UINT NotifyThread(LPVOID pParam);

	DWORD TranslateIP(CString strIP);

	BOOL CheckUserName(const CString strName, CString strHost, DWORD dwIP);
	BOOL CheckUserName(const CString strName, DWORD dwIP = 0, WORD wPort = 0);
	BOOL CheckCommands(const CString strUser, const CString strMsg);

	BOOL ExecuteChannelCommand(const CString strUser, const CString strMsg);
	void FixString(CString& strFix);
	int GetByID(GUID guid);
	void SendChannelRename(void);
	BOOL HasSpeakPermission(UINT uMode);
	void SendNoVoice(GUID guid);
	void EchoChat(CString strName, CString strMsg);
};


