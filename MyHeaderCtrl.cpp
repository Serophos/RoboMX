// MyHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "MyHeaderCtrl.h"


// CMyHeaderCtrl

IMPLEMENT_DYNAMIC(CMyHeaderCtrl, CHeaderCtrl)

CMyHeaderCtrl::CMyHeaderCtrl()	
	: m_nSortColumn(-1),
	  m_bSortAscending(TRUE)
{
}

CMyHeaderCtrl::~CMyHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyHeaderCtrl, CHeaderCtrl)
END_MESSAGE_MAP()



// CMyHeaderCtrl message handlers

void CMyHeaderCtrl::SetSortArrow( const int nSortColumn, const BOOL bSortAscending )
{

	m_nSortColumn    = nSortColumn;
	m_bSortAscending = bSortAscending;

	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	VERIFY(GetItem(nSortColumn, &hditem));

	hditem.fmt |= HDF_OWNERDRAW;
	VERIFY(SetItem(nSortColumn, &hditem));

	// invalidate the header control so it gets redrawn
	Invalidate();
}

void CMyHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// attath to the device context.
	CDC dc;
	VERIFY( dc.Attach( lpDrawItemStruct->hDC ) );

	// save Device context.
	const int nSavedDC = dc.SaveDC();

	// get columnrect.
	CRect rc(lpDrawItemStruct->rcItem);

	// set clipping region to limit drawing within the column.
	CRgn rgn;
	VERIFY(rgn.CreateRectRgnIndirect(&rc));
    (void)dc.SelectObject(&rgn);
	VERIFY(rgn.DeleteObject());

	// draw background,
	CBrush brush(GetSysColor(COLOR_3DFACE));
	dc.FillRect(rc, &brush);

	// get the column text and format.
	TCHAR szText[256];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;

	VERIFY(GetItem(lpDrawItemStruct->itemID, &hditem));

	// determine the format for drawing the column label.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;

	if(hditem.fmt & HDF_CENTER){

		uFormat |= DT_CENTER;
	}
	else if(hditem.fmt & HDF_RIGHT){

		uFormat |= DT_RIGHT;
	}
	else{

		uFormat |= DT_LEFT;
	}

	// adjust rect if the mouse button is pressed on it.
	if(lpDrawItemStruct->itemState == ODS_SELECTED){

		rc.left++;
		rc.top += 2;
		rc.right++;
	}

	CRect rcIcon(lpDrawItemStruct->rcItem);
	const int iOffset = (rcIcon.bottom - rcIcon.top) / 4;

	// adjust the rect further if the sort arrow is to be displayed.
	if(lpDrawItemStruct->itemID == (UINT)m_nSortColumn){

		rc.right -= 3 * iOffset;
	}

	rc.left += iOffset;
	rc.right -= iOffset;

	// draw the column label.
	if(rc.left < rc.right){

		(void)dc.DrawText(szText, -1, rc, uFormat);
	}

	// draw the sort arrow.
	if(lpDrawItemStruct->itemID == (UINT)m_nSortColumn){

		// set up the pens to use for drawing the arrow.
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen* pOldPen = dc.SelectObject(&penLight);

		if(m_bSortAscending){

			// draw arrow pointing upwards.
			dc.MoveTo(rcIcon.right - 2 * iOffset, iOffset);
			dc.LineTo(rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1);
			dc.LineTo(rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1);
			(void)dc.SelectObject(&penShadow);
			dc.MoveTo(rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1);
			dc.LineTo(rcIcon.right - 2 * iOffset, iOffset - 1);		
		}
		else{

			// draw arrow pointing downwards.
			dc.MoveTo(rcIcon.right - iOffset - 1, iOffset);
			dc.LineTo(rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset);
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo(rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset);
			dc.LineTo(rcIcon.right - 3 * iOffset - 1, iOffset);
			dc.LineTo(rcIcon.right - iOffset - 1, iOffset);		
		}

		// restore the pen.
		(void)dc.SelectObject(pOldPen);
	}

	// restore previous device context.
	VERIFY(dc.RestoreDC(nSavedDC));

	// detach device context before returning.
	(void)dc.Detach();
}


void CMyHeaderCtrl::Serialize( CArchive& ar )
{

	if(ar.IsStoring()){

		const int nItemCount = GetItemCount();

		if(nItemCount != -1){

			ar << nItemCount;

			HD_ITEM hdItem = { 0 };
			hdItem.mask = HDI_WIDTH;

			for(int i = 0; i < nItemCount; i++){

				VERIFY(GetItem(i, &hdItem ));
				ar << hdItem.cxy;
			}
		}
	}
	else{

		int nItemCount;
		ar >> nItemCount;
		
		if(GetItemCount() != nItemCount){

			TRACE( _T("Different number of columns in registry."));
		}
		else{

			HD_ITEM hdItem = { 0 };
			hdItem.mask = HDI_WIDTH;

			for(int i = 0; i < nItemCount; i++){
                
				ar >> hdItem.cxy;
				VERIFY(SetItem( i, &hdItem ));
			}
		}
	}
}
