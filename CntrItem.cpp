// CntrItem.cpp : implementation of the CRichEditTestCntrItem class
//

#include "stdafx.h"
#include "Metis3.h"

#include "ServerDoc.h"
#include "ServerView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRichEditTestCntrItem implementation

IMPLEMENT_SERIAL(CServerCntrItem, CRichEditCntrItem, 0)

CServerCntrItem::CServerCntrItem(REOBJECT* preo, CServerDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
}

CServerCntrItem::~CServerCntrItem()
{
	// TODO: add cleanup code here
}


// CRichEditTestCntrItem diagnostics

#ifdef _DEBUG
void CServerCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CServerCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

