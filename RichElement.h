#pragma once

class CRichDocument;
class CRichFragment;
class CRichEditExCtrl;

class CRichElement
{
public:
	CRichElement(int nType = 0, LPCTSTR pszText = NULL, LPCTSTR pszLink = NULL, DWORD nFlags = 0, int nGroup = 0);
	virtual ~CRichElement();
	
// Attributes
public:
	CRichDocument*	m_pDocument;
	int				m_nType;
	int				m_nGroup;
	DWORD			m_nFlags;
	CString			m_sText;
	CString			m_sLink;
	DWORD			m_hImage;
	COLORREF		m_cColor;
	COLORREF		m_cBgColor;

// Operations
public:
	void	Show(BOOL bShow = TRUE);
	void	SetText(LPCTSTR pszText);
	void	SetFlags(DWORD nFlags, DWORD nMask = 0xFFFFFFFF);
	void	Delete();
protected:
	void	PrePaint(CDC* pDC, BOOL bHover);
	void	PrePaintBitmap(CDC* pDC);
	CSize	GetSize();
	
	friend class CRichFragment;
	friend class CRichEditExCtrl;
};

enum
{
	retNull, retNewline, retGap, retAlign,
	retBitmap, retAnchor, retCmdIcon,
	retText, retLink, retName, retHeading
};

enum
{
	retfNull		= 0x00,
	retfBold		= 0x01,
	retfItalic		= 0x02,
	retfUnderline	= 0x04,
	retfHeading		= 0x08,
	retfMiddle		= 0x10,
	retfColor		= 0x20,
	retfHidden		= 0x80,
};

enum
{
	reaLeft, reaCenter, reaRight
};
