/*
** Copyright (C) 2004 Thees Winkler
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include "stdafx.h"
#include "ImageDataObject.h"
#include "RichEditExCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CImageDataObject::InsertBitmap(IRichEditOle* pRichEditOle, CRichEditExCtrl *Conv, HBITMAP hBitmap)
{

	ASSERT(pRichEditOle);
	TRY{

		CoInitialize(NULL);
		SCODE sc;

		CImageDataObject *pods = new CImageDataObject;
		LPDATAOBJECT lpDataObject;
		pods->QueryInterface(IID_IDataObject, (void **)&lpDataObject);

		pods->SetBitmap(hBitmap);

		IOleClientSite *pOleClientSite;	
		pRichEditOle->GetClientSite(&pOleClientSite);

		IStorage *pStorage;	

		LPLOCKBYTES lpLockBytes = NULL;
		sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
		if (sc != S_OK)
			AfxThrowOleException(sc);
		ASSERT(lpLockBytes != NULL);
		
		sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
		{
			VERIFY(lpLockBytes->Release() == 0);
			lpLockBytes = NULL;
			AfxThrowOleException(sc);
		}
		ASSERT(pStorage != NULL);

		IOleObject *pOleObject; 
		pOleObject = pods->GetOleObject(pOleClientSite, pStorage);

		OleSetContainedObject(pOleObject, TRUE);

		REOBJECT reobject;
		ZeroMemory(&reobject, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		
		CLSID clsid;
		sc = pOleObject->GetUserClassID(&clsid);
		if (sc != S_OK)
			AfxThrowOleException(sc);

		reobject.clsid = clsid;
		reobject.cp = REO_CP_SELECTION;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.poleobj = pOleObject;
		reobject.polesite = pOleClientSite;
		reobject.pstg = pStorage;

		pRichEditOle->InsertObject(&reobject);

		pOleObject->Release();
		pOleClientSite->Release();

		lpLockBytes->Release();

		pStorage->Release();
		lpDataObject->Release();
	}
	CATCH(CException, e)
    {

		char lpszError[400];
		e->GetErrorMessage(lpszError, 400);
		CString strOut;
		strOut.Format("Could not insert emoticon due to OleException. Returned error message was:\n%s", lpszError);
        AfxMessageBox(strOut, MB_ICONSTOP);
    }
    END_CATCH
}

void CImageDataObject::SetBitmap(HBITMAP hBitmap)
{
	ASSERT(hBitmap);

	STGMEDIUM stgm;
	stgm.tymed = TYMED_GDI;
	stgm.hBitmap = hBitmap;
	stgm.pUnkForRelease = NULL;

	FORMATETC fm;
	fm.cfFormat = CF_BITMAP;
	fm.ptd = NULL;
	fm.dwAspect = DVASPECT_CONTENT;
	fm.lindex = -1;
	fm.tymed = TYMED_GDI;

	SetData(&fm, &stgm, TRUE);		
}

IOleObject *CImageDataObject::GetOleObject(IOleClientSite *pOleClientSite, IStorage *pStorage)
{
	ASSERT(m_stgmed.hBitmap);

	SCODE sc;
	IOleObject *pOleObject;
	sc = ::OleCreateStaticFromData(this, IID_IOleObject, OLERENDER_FORMAT, 
			&m_fromat, pOleClientSite, pStorage, (void **)&pOleObject);
	if (sc != S_OK)
		AfxThrowOleException(sc);
	return pOleObject;
}