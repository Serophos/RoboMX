// RichEditTestDoc.cpp : implementation of the CRichEditTestDoc class
//

#include "stdafx.h"
#include "Metis3.h"

#include "ServerDoc.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRichEditTestDoc

IMPLEMENT_DYNCREATE(CServerDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CServerDoc, CRichEditDoc)
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, CRichEditDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()


// CRichEditTestDoc construction/destruction

CServerDoc::CServerDoc()
{
	// TODO: add one-time construction code here

}

CServerDoc::~CServerDoc()
{
}

BOOL CServerDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	SetTitle("Unnamed Roomserver");
	return TRUE;
}
CRichEditCntrItem* CServerDoc::CreateClientItem(REOBJECT* preo) const
{
	return new CServerCntrItem(preo, const_cast<CServerDoc*>(this));
}




// CRichEditTestDoc serialization

void CServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CRichEditDoc enables serialization
	//  of the container document's COleClientItem objects.
	// TODO: set CRichEditDoc::m_bRTF = FALSE if you are serializing as text
	CRichEditDoc::Serialize(ar);
}


// CRichEditTestDoc diagnostics

#ifdef _DEBUG
void CServerDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CServerDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


// CRichEditTestDoc commands
