/*
** Copyright (C) 2004 Thees Winkler
**  
4** This program is free software; you can redistribute it and/or modify
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

#if !defined(AFX_CHANNELVIEW_H__F523F13F_A0C7_4D12_B86F_D4475D140E65__INCLUDED_)

#include "stdafx.h"
#include "Metis3.h"
#include "ChannelDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChannelDoc

IMPLEMENT_DYNCREATE(CChannelDoc, CDocument)

CChannelDoc::CChannelDoc()
{
}

BOOL CChannelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle("Channellist");
	return TRUE;
}

CChannelDoc::~CChannelDoc()
{
}


BEGIN_MESSAGE_MAP(CChannelDoc, CDocument)
	//{{AFX_MSG_MAP(CChannelDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelDoc diagnostics

#ifdef _DEBUG
void CChannelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CChannelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChannelDoc serialization

void CChannelDoc::Serialize(CArchive& ar)
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
// CChannelDoc commands
