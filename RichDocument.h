#pragma once

class CRichElement;

class CRichDocument  
{
// Construction
public:
	CRichDocument();
	virtual ~CRichDocument();
	
// Attributes
public:
	CCriticalSection	m_pSection;
	CPtrList			m_pElements;
	DWORD				m_nCookie;

public:
	CSize			m_szMargin;
	COLORREF		m_crBackground;
	COLORREF		m_crText;
	COLORREF		m_crLink;
	COLORREF		m_crHover;
	COLORREF		m_crHeading;

public:
	CFont			m_fntNormal;
	CFont			m_fntBold;
	CFont			m_fntItalic;
	CFont			m_fntUnder;
	CFont			m_fntBoldUnder;
	CFont			m_fntHeading;
	
// Operations
public:
	POSITION		GetIterator() const;
	CRichElement*	GetNext(POSITION& pos) const;
	CRichElement*	GetPrev(POSITION& pos) const;
	int				GetCount() const;
	POSITION		Find(CRichElement* pElement) const;

public:
	CRichElement*	Add(CRichElement* pElement, POSITION posBefore = NULL);
	CRichElement*	Add(int nType, LPCTSTR pszText, LPCTSTR pszLink = NULL, DWORD nFlags = 0, int nGroup = 0, POSITION posBefore = NULL);
	void			Remove(CRichElement* pElement);
	void			ShowGroup(int nGroup, BOOL bShow = TRUE);
	void			ShowGroupRange(int nMin, int nMax, BOOL bShow = TRUE);
	void			SetModified();
	void			Clear();

public:
	//BOOL			LoadXML(CXMLElement* pBase, CMapStringToPtr* pMap = NULL, int nGroup = 0);
	void			CreateFonts(LPCTSTR pszFaceName = _T("Arial"), int nSize = 12);

//protected:
	//BOOL			LoadXMLStyles(CXMLElement* pParent);
	//BOOL			LoadXMLColour(CXMLElement* pXML, LPCTSTR pszName, COLORREF* pColour);


	// Inline
public:
	inline void Lock()
	{
		m_pSection.Lock();
	}

	inline void Unlock()
	{
		m_pSection.Unlock();
	}
	
};
