// ServerView.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "ServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerView

IMPLEMENT_DYNCREATE(CServerView, CRichEditView)

CServerView::CServerView()
{
}

CServerView::~CServerView()
{
}


BEGIN_MESSAGE_MAP(CServerView, CRichEditView)
	//{{AFX_MSG_MAP(CServerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerView drawing

void CServerView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CServerView message handlers

void CServerView::OnInitialUpdate()
{

	CRichEditView::OnInitialUpdate();
	
	m_bListen = TRUE;
	AfxBeginThread(ServerThread, (PVOID)this, THREAD_PRIORITY_NORMAL);

}

UINT CServerView::ServerThread(PVOID pParam)
{

	CServerView* pServer = (CServerView*)pParam;

	CMySocket m_mServer(IPPROTO_UDP, 6257);

	SOCKET sClient = INVALID_SOCKET;
	char buffer[6000];
	int nLen = 0;
	do{

		int nLen = m_mServer.RecvFrom(buffer, 6000, 0);

		if(nLen){

			TRACE("RecvFrom %d\n", nLen);
			//DecryptMXUDP(BYTE*)buffer, nLen
		}
		else{

			TRACE("Z\n");
			Sleep(100);
		}

	}while(pServer->m_bListen);
	

	TRACE("Leaving Server thread\n");
	return 0;
}

UINT CServerView::ClientThread(PVOID pParam)
{

	TRACE("Incoming connection...\n");
	CMySocket m_mClient((SOCKET)pParam);
	
	char buffer[1024];
	m_mClient.Recv(buffer, 1024, 0);

	TRACE("Leaving Client thread\n");
	return 0;
}
