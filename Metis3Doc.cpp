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

#include "Metis3Doc.h"
#include "ConnectionDlg.h"
#include "MainFrm.h"
#include "Settings.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;

/////////////////////////////////////////////////////////////////////////////
// CMetis3Doc

IMPLEMENT_DYNCREATE(CMetis3Doc, CDocument)

BEGIN_MESSAGE_MAP(CMetis3Doc, CDocument)
	//{{AFX_MSG_MAP(CMetis3Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetis3Doc construction/destruction

CMetis3Doc::CMetis3Doc()
{

	m_dwFiles    = 0;
	m_wLine      = 0x00;
	m_wUserMode  = 0;
	m_bOldJoin   = FALSE;
}

CMetis3Doc::~CMetis3Doc()
{
}

BOOL CMetis3Doc::OnNewDocument()
{

	if(((CMainFrame*)GetApp()->m_pMainWnd)->m_bQuickJoin){

		m_dwFiles = g_sSettings.GetFiles();
		m_wLine   = g_sSettings.GetLine();
		m_strRoom = ((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom;
		m_strName = g_sSettings.GetNickname();
		Util::MakeValidUserName(m_strName);
	}
	else{

		CConnectionDlg dlg;

		if(dlg.DoModal() == IDCANCEL){

			return FALSE;
		}
		m_dwFiles = dlg.m_dwFiles;
		m_wLine   = (WORD)dlg.m_nLine;
		m_strRoom = dlg.m_strRoom;
		m_strName = dlg.m_strName;
		m_bOldJoin = dlg.m_bOldJoin;
	}

	if (!CDocument::OnNewDocument()){

		return FALSE;
	}
	
	((CMainFrame*)GetApp()->m_pMainWnd)->m_bQuickJoin = FALSE;
	((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom.Empty();

	int n = m_strRoom.ReverseFind('_');
	if(n > 0){

		m_strRoomShort = m_strRoom.Left(n);
	}
	
	CDocument::SetTitle(m_strRoom + " (" + m_strName + ")");
	return TRUE;
}


void CMetis3Doc::SetTitle(LPCTSTR lpszTitle, BOOL bShort)
{

	if(bShort){ // the roomname doesnt have the numbers at the end

		CString strAppendix;
		
		int n = m_strRoom.ReverseFind('_');

		m_strRoomShort = lpszTitle;
		
		if(n > 0){

			strAppendix = m_strRoom.Mid(n);
		}

		m_strRoom = m_strRoomShort + strAppendix;
	}
	else{ // the roomname has numbers at the end


		m_strRoom = lpszTitle;
		int n = m_strRoom.ReverseFind('_');
		if(n > 0){

			m_strRoomShort = m_strRoom.Left(n);
		}
	}
	CDocument::SetTitle(m_strRoom + " (" + m_strName + ")");
}
/////////////////////////////////////////////////////////////////////////////
// CMetis3Doc serialization

void CMetis3Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMetis3Doc diagnostics

#ifdef _DEBUG
void CMetis3Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMetis3Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMetis3Doc commands
