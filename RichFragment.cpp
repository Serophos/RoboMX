#include "stdafx.h"
#include "RichEditExCtrl.h"
#include "RichFragment.h"
#include "RichElement.h"
#include ".\richfragment.h"

CRichFragment::CRichFragment(CRichElement* pElement, int nOffset, int nLength, CPoint* pPoint, CSize* pSize)
{

	m_pElement	= pElement;
	m_pt		= *pPoint;
	m_sz		= *pSize;
	m_nOffset	= (WORD)nOffset;
	m_nLength	= (WORD)nLength;
}

CRichFragment::CRichFragment(CRichElement* pElement, CPoint* pPoint)
{

	m_pElement	= pElement;
	m_pt		= *pPoint;
	m_sz		= pElement->GetSize();
	m_nOffset	= 0;
	m_nLength	= 0;
}

CRichFragment::~CRichFragment()
{ 
}

void CRichFragment::Add(int nLength, CSize* pSize)
{

	ASSERT(m_pElement->m_nType >= retText);
	
	m_nLength	= nLength;
	m_sz		= *pSize;
}

void CRichFragment::Paint(CDC* pDC, CRichEditExCtrl* pCtrl, int nFragment)
{

	BOOL bClean  =	pCtrl->m_pSelAbsEnd.nFragment < nFragment ||
					pCtrl->m_pSelAbsStart.nFragment > nFragment;

	BOOL bSelect =	FALSE;
	
	if(!bClean){

		if((pCtrl->m_pSelAbsStart.nFragment == pCtrl->m_pSelAbsEnd.nFragment) &&
			 (pCtrl->m_pSelAbsStart.nOffset == pCtrl->m_pSelAbsEnd.nOffset)){

			bClean = TRUE;
		}
		else if((nFragment < pCtrl->m_pSelAbsEnd.nFragment) &&
				((nFragment > pCtrl->m_pSelAbsStart.nFragment) ||
					(nFragment == pCtrl->m_pSelAbsStart.nFragment) &&
					pCtrl->m_pSelAbsStart.nOffset == 0)){

			bSelect = TRUE;
		}
	}
	
	if(m_pElement->m_nType >= retText){

		LPCTSTR pszText = (LPCTSTR)m_pElement->m_sText + m_nOffset;
		
		if(bClean){

			pDC->ExtTextOut(m_pt.x, m_pt.y, ETO_OPAQUE, NULL, pszText, m_nLength, NULL);
		}
		else if(bSelect){

			pDC->SetBkColor(pDC->SetTextColor(pDC->GetBkColor()));
			pDC->ExtTextOut(m_pt.x, m_pt.y, ETO_OPAQUE, NULL, pszText, m_nLength, NULL);
			pDC->SetBkColor(pDC->SetTextColor(pDC->GetBkColor()));
		}
		else if(pCtrl->m_pSelAbsStart.nFragment == nFragment ||
					pCtrl->m_pSelAbsEnd.nFragment == nFragment){

			int nCharStart = 0, nCharEnd = m_nLength;
			int nX = m_pt.x;
			
			if(pCtrl->m_pSelAbsStart.nFragment == nFragment){

				nCharStart = pCtrl->m_pSelAbsStart.nOffset;
			}
			
			if(pCtrl->m_pSelAbsEnd.nFragment == nFragment){

				nCharEnd = pCtrl->m_pSelAbsEnd.nOffset;
			}
			
			if(nCharStart > 0){

				pDC->ExtTextOut(nX, m_pt.y, ETO_OPAQUE, NULL, pszText, nCharStart, NULL);
				nX += pDC->GetTextExtent(pszText, nCharStart).cx;
				pszText += nCharStart;
			}
			
			if(nCharStart < nCharEnd){

				pDC->SetBkColor(pDC->SetTextColor(pDC->GetBkColor()));
				pDC->ExtTextOut(nX, m_pt.y, ETO_OPAQUE, NULL, pszText, nCharEnd - nCharStart, NULL);
				pDC->SetBkColor(pDC->SetTextColor(pDC->GetBkColor()));
				nX += pDC->GetTextExtent(pszText, nCharEnd - nCharStart).cx;
				pszText += nCharEnd - nCharStart;
			}
			
			if(nCharEnd < m_nLength){

				pDC->ExtTextOut(nX, m_pt.y, ETO_OPAQUE, NULL, pszText, m_nLength - nCharEnd, NULL);
			}
		}
		else{

			pDC->ExtTextOut(m_pt.x, m_pt.y, ETO_OPAQUE, NULL, pszText, m_nLength, NULL);
		}
		
		pDC->ExcludeClipRect(m_pt.x, m_pt.y, m_pt.x + m_sz.cx, m_pt.y + m_sz.cy);
	}
	else if((m_pElement->m_nType == retBitmap) && m_pElement->m_hImage){

		CDC mdc;
		mdc.CreateCompatibleDC(pDC);
		
		HBITMAP hOld = (HBITMAP)SelectObject(mdc.GetSafeHdc(),
			(HBITMAP)m_pElement->m_hImage);
		
		pDC->BitBlt(m_pt.x, m_pt.y, m_sz.cx, m_sz.cy, &mdc, 0, 0, SRCCOPY);
		
		SelectObject(mdc.GetSafeHdc(), hOld);
		mdc.DeleteDC();
		
		if(bSelect){

			CRect rc(m_pt.x, m_pt.y, m_pt.x + m_sz.cx, m_pt.y + m_sz.cy);
			pDC->InvertRect(&rc);
		}
		
		pDC->ExcludeClipRect(m_pt.x, m_pt.y, m_pt.x + m_sz.cx, m_pt.y + m_sz.cy);
	}
}

