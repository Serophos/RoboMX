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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	// TODO: add one-time construction code here

}

CMetis3Doc::~CMetis3Doc()
{
}

BOOL CMetis3Doc::OnNewDocument()
{

	CConnectionDlg dlg;

	if(dlg.DoModal() == IDCANCEL){

		return FALSE;
	}

	if (!CDocument::OnNewDocument()){

		return FALSE;
	}
	
	m_dwFiles = dlg.m_dwFiles;
	m_wLine   = (WORD)dlg.m_nLine;
	m_strRoom = dlg.m_strRoom;
	m_strName = dlg.m_strName;

	SetTitle(m_strName + "@" + m_strRoom);
	return TRUE;
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
