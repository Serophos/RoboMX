#include "stdafx.h"
#include ".\richelement.h"
#include "RichDocument.h"
#include "RichFragment.h"
#include "Settings.h"
#include "EmoticonManager.h"

extern CSettings g_sSettings;
extern CEmoticonManager g_eEmoticons;

CRichElement::CRichElement(int nType, LPCTSTR pszText, LPCTSTR pszLink, DWORD nFlags, int nGroup)
{

	m_pDocument	= NULL;
	m_nType		= nType;
	m_nFlags	= nFlags;
	m_nGroup	= nGroup;
	m_hImage	= NULL;

	if(m_nType == retHeading){

		m_nType = retText;
		m_nFlags |= retfHeading;
	}
	if(pszText != NULL){

		if((m_nType == retBitmap) && HIWORD(pszText) == 0){

			m_sText.Format(_T("%lu"), (DWORD)pszText);
		}
		else{

			m_sText = pszText;
		}
	}

	if(pszLink != NULL) m_sLink = pszLink;
}

CRichElement::~CRichElement()
{

	if(m_hImage != NULL){

		//if(m_nType == retBitmap){

			//DeleteObject((HBITMAP)m_hImage);
			//m_hImage = NULL;
		//}
	}
}

//////////////////////////////////////////////////////////////////////
// CRichElement editing

void CRichElement::Show(BOOL bShow)
{

	if(bShow == ((m_nFlags & retfHidden) > 0)){

		m_nFlags |= retfHidden;
		if(bShow) m_nFlags &= ~retfHidden;
		m_pDocument->m_nCookie++;
	}
}

void CRichElement::SetText(LPCTSTR pszText)
{

	if(m_sText != pszText){

		m_sText = pszText;
		m_pDocument->m_nCookie++;
	}
}

void CRichElement::SetFlags(DWORD nFlags, DWORD nMask)
{

	DWORD nNew = (m_nFlags & ~nMask) | (nFlags & nMask);
	
	if(nNew != m_nFlags){

		m_nFlags = nNew;
		m_pDocument->m_nCookie++;
	}
}

//////////////////////////////////////////////////////////////////////
// CRichElement delete

void CRichElement::Delete()
{
	if(m_pDocument) m_pDocument->Remove(this);
	delete this;
}

//////////////////////////////////////////////////////////////////////
// CRichElement setup for paint

void CRichElement::PrePaint(CDC* pDC, BOOL bHover)
{
	
	if(m_pDocument->m_fntNormal.m_hObject == NULL) m_pDocument->CreateFonts();
	
	CFont* pFont = &m_pDocument->m_fntNormal;
	
	switch (m_nType){

	case retText:
	case retName:
		if(m_nFlags & retfColor){

			pDC->SetTextColor(m_cColor);
			pDC->SetBkColor(m_cBgColor);
		}
		else
			pDC->SetTextColor(m_pDocument->m_crText);
		pFont = &m_pDocument->m_fntNormal;
		break;
	case retLink:
		if(m_nFlags & retfColor)
			pDC->SetTextColor(m_cColor);
		else
			pDC->SetTextColor(bHover ? m_pDocument->m_crHover : m_pDocument->m_crLink);
		pFont = &m_pDocument->m_fntUnder;
		break;
	case retBitmap:
		PrePaintBitmap(pDC);
		pFont = NULL;
		break;
	default:
		pFont = NULL;
		break;
	}
	
	if(m_nFlags & retfBold){

		if(m_nFlags & retfUnderline) pFont = &m_pDocument->m_fntBoldUnder;
		else pFont = &m_pDocument->m_fntBold;
	}
	else if(m_nFlags & retfItalic){

		pFont = &m_pDocument->m_fntItalic;
	}
	else if(m_nFlags & retfUnderline){

		pFont = &m_pDocument->m_fntUnder;
	}
	else if(m_nFlags & retfHeading){

		pFont = &m_pDocument->m_fntHeading;
		pDC->SetTextColor(m_pDocument->m_crHeading);
	}
	
	if(pFont) pDC->SelectObject(pFont);
}

void CRichElement::PrePaintBitmap(CDC* pDC)
{
	
	if(m_hImage != NULL) return;
	
	int n = g_eEmoticons.Lookup(m_sText, -1);
	if(n >= 0){

		m_hImage = (DWORD)g_eEmoticons.m_aEmoticons.GetAt(n)->hBitmap;
	}
}

//////////////////////////////////////////////////////////////////////
// CRichElement dimensions

CSize CRichElement::GetSize()
{

	CSize sz(0, 0);
	
	if(m_nType == retGap){

		_stscanf(m_sText, _T("%lu"), &sz.cx);
	}
	else if(m_nType == retBitmap && m_hImage != NULL){

		BITMAP pInfo;
		GetObject((HBITMAP)m_hImage, sizeof(pInfo), &pInfo);

		sz.cx = pInfo.bmWidth;
		sz.cy = pInfo.bmHeight;
	}
	else if(m_nType == retAnchor){

		sz.cx = sz.cy = 16;
		_stscanf(m_sText, _T("%i.%i"), &sz.cx, &sz.cy);
	}
	
	return sz;
}
