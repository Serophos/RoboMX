#pragma once


// CSettingsDoc document

class CSettingsDoc : public CDocument
{
	DECLARE_DYNCREATE(CSettingsDoc)

public:
	CSettingsDoc();
	virtual ~CSettingsDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
