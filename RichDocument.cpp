#include "stdafx.h"
#include ".\richdocument.h"
#include "RichElement.h"
#include "RichFragment.h"
#include "Settings.h"

extern CSettings g_sSettings;

CRichDocument::CRichDocument() : m_szMargin(8, 8)
{
	m_nCookie		= 0;
	m_crBackground	= RGB(255, 255, 255);
	m_crText		= RGB(0, 0, 0);
	m_crLink		= RGB(0, 0, 255);
	m_crHover		= RGB(255, 0, 0);
	m_crHeading		= RGB(0x80, 0, 0);
}

CRichDocument::~CRichDocument()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
// CRichDocument element access

POSITION CRichDocument::GetIterator() const
{

	return m_pElements.GetHeadPosition();
}

CRichElement* CRichDocument::GetNext(POSITION& pos) const
{

	return (CRichElement*)m_pElements.GetNext(pos);
}

CRichElement* CRichDocument::GetPrev(POSITION& pos) const
{

	return (CRichElement*)m_pElements.GetPrev(pos);
}

int CRichDocument::GetCount() const
{

	return m_pElements.GetCount();
}

POSITION CRichDocument::Find(CRichElement* pElement) const
{

	return m_pElements.Find(pElement);
}

//////////////////////////////////////////////////////////////////////
// CRichDocument element modification

CRichElement* CRichDocument::Add(CRichElement* pElement, POSITION posBefore)
{

	CSingleLock pLock(&m_pSection, TRUE);
	
	if(posBefore)
		m_pElements.InsertBefore(posBefore, pElement);
	else
		m_pElements.AddTail(pElement);

	pElement->m_pDocument = this;
	m_nCookie++;

	return pElement;
}

CRichElement* CRichDocument::Add(int nType, LPCTSTR pszText, LPCTSTR pszLink, DWORD nFlags, int nGroup, POSITION posBefore)
{

	CRichElement* pElement = new CRichElement(nType, pszText, pszLink, nFlags, nGroup);
	return Add(pElement, posBefore);
}

void CRichDocument::Remove(CRichElement* pElement)
{

	CSingleLock pLock(&m_pSection, TRUE);
	
	if(POSITION pos = m_pElements.Find(pElement)){

		m_pElements.RemoveAt(pos);
		pElement->m_pDocument = NULL;
		m_nCookie++;
	}
}

void CRichDocument::ShowGroup(int nGroup, BOOL bShow)
{

	CSingleLock pLock(&m_pSection, TRUE);
	
	for(POSITION pos = GetIterator(); pos;){

		CRichElement* pElement = GetNext(pos);
		if(pElement->m_nGroup == nGroup) pElement->Show(bShow);
	}
}

void CRichDocument::ShowGroupRange(int nMin, int nMax, BOOL bShow)
{

	CSingleLock pLock(&m_pSection, TRUE);
	
	for(POSITION pos = GetIterator() ; pos ;){

		CRichElement* pElement = GetNext(pos);
		if(pElement->m_nGroup >= nMin && pElement->m_nGroup <= nMax)
			pElement->Show(bShow);
	}
}

void CRichDocument::SetModified()
{

	m_nCookie++;
}

void CRichDocument::Clear()
{

	CSingleLock pLock(&m_pSection, TRUE);
	
	for(POSITION pos = GetIterator(); pos;){

		delete GetNext(pos);
	}

	m_pElements.RemoveAll();
	m_nCookie++;
}

//////////////////////////////////////////////////////////////////////
// CRichDocument font construction

void CRichDocument::CreateFonts(LPCTSTR pszFaceName, int nSize)
{

	CSingleLock pLock(&m_pSection, TRUE);
	
	if(m_fntNormal.m_hObject) m_fntNormal.DeleteObject();

	BYTE nCharSet = g_sSettings.GetCharSet();

	m_fntNormal.CreateFont(-nSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFaceName);
	
	if(m_fntBold.m_hObject) m_fntBold.DeleteObject();
	
	m_fntBold.CreateFont(-nSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFaceName);
	
	if(m_fntItalic.m_hObject) m_fntItalic.DeleteObject();
	
	m_fntItalic.CreateFont(-nSize, 0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFaceName);
	
	if(m_fntUnder.m_hObject) m_fntUnder.DeleteObject();
	
	m_fntUnder.CreateFont(-nSize, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFaceName);
	
	if(m_fntBoldUnder.m_hObject) m_fntBoldUnder.DeleteObject();
	
	m_fntBoldUnder.CreateFont(-nSize, 0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFaceName);
	
	if(m_fntHeading.m_hObject) m_fntHeading.DeleteObject();
	
	m_fntHeading.CreateFont(-(nSize + 6), 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, pszFaceName);
}
