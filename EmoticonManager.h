#pragma once

class CRichDocument;

class CEmoticon
{
public:
	CEmoticon(){ hBitmap = 0;}
	~CEmoticon(){ if(hBitmap) DeleteObject(hBitmap);}

	HBITMAP hBitmap;
	char szFileName[1024];
	char szActivationText[64];
};

class CEmoticonManager
{

public:
	CEmoticonManager(void);
	~CEmoticonManager(void);

public:
	BOOL	Load(void);
	BOOL	Free(void);
	LPCTSTR	FindNext(LPCTSTR pszText, int* pnIndex);
	int		Lookup(LPCTSTR pszText, int nLen = -1) const;
	LPCTSTR	GetText(int nIndex) const;
	void	BuildTokens();
	void	FormatText(CRichDocument* pDocument, LPCTSTR pszBody, BOOL bNewlines = FALSE, BOOL bBold = TRUE, BOOL bItalic = FALSE, BOOL bUnderLine = FALSE, COLORREF cr = 0, COLORREF crBg = 0xFFFFFF, BOOL bName = FALSE);
	static	HBITMAP ReplaceColor(HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC);

public:
	CArray<CEmoticon*, CEmoticon*> m_aEmoticons;
	LPTSTR		m_pTokens;

protected:
	void AddEmoticon(char* szFileName, char* szActivationText);
};
