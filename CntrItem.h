// CntrItem.h : interface of the CRichEditTestCntrItem class
//

#pragma once

class CServerDoc;
class CServerView;

class CServerCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CServerCntrItem)

// Constructors
public:
	CServerCntrItem(REOBJECT* preo = NULL, CServerDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CServerDoc* GetDocument()
		{ return reinterpret_cast<CServerDoc*>(CRichEditCntrItem::GetDocument()); }
	CServerView* GetActiveView()
		{ return reinterpret_cast<CServerView*>(CRichEditCntrItem::GetActiveView()); }

	public:
	protected:

// Implementation
public:
	~CServerCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

