#pragma once

class CRichElement;
class CRichEditExCtrl;

class CRichFragment
{
public:
	CRichFragment(CRichElement* pElement, int nOffset, int nLength, CPoint* pPoint, CSize* pSize);
	CRichFragment(CRichElement* pElement, CPoint* pPoint);
	virtual ~CRichFragment(void);

	// Attributes
public:
	CRichElement*	m_pElement;
	CPoint			m_pt;
	CSize			m_sz;
	WORD			m_nOffset;
	WORD			m_nLength;

// Operations
public:
	void	Add(int nLength, CSize* pSize);
	void	Paint(CDC* pDC, CRichEditExCtrl* pCtrl, int nFragment);
};
