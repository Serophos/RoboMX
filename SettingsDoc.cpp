// SettingsDoc.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "SettingsDoc.h"


// CSettingsDoc

IMPLEMENT_DYNCREATE(CSettingsDoc, CDocument)

CSettingsDoc::CSettingsDoc()
{
}

BOOL CSettingsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle("Settings");
	return TRUE;
}

CSettingsDoc::~CSettingsDoc()
{
}


BEGIN_MESSAGE_MAP(CSettingsDoc, CDocument)
END_MESSAGE_MAP()


// CSettingsDoc diagnostics

#ifdef _DEBUG
void CSettingsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSettingsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSettingsDoc serialization

void CSettingsDoc::Serialize(CArchive& ar)
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


// CSettingsDoc commands
