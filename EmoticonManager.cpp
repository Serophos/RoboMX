#include "stdafx.h"
#include ".\emoticonmanager.h"
#include "settings.h"
#include "RichDocument.h"
#include "RichElement.h"
#include "util.h"

extern CSettings g_sSettings;

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) \
                                 | ((Color << 16) & 0xff0000)

CEmoticonManager::CEmoticonManager(void)
{

	m_pTokens = NULL;
}

CEmoticonManager::~CEmoticonManager(void)
{

	Free();
}

BOOL CEmoticonManager::Load(void)
{

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\emoticons.ini";
	BOOL bReturn = TRUE;
	CStdioFile ini;
	CString strActivationText, strFilename;

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while((ini.ReadString(strActivationText) != NULL) && (ini.ReadString(strFilename) != NULL)){
		
			AddEmoticon((LPTSTR)(LPCTSTR)strFilename, (LPTSTR)(LPCTSTR)strActivationText);
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		char error[512];
		e->GetErrorMessage((char*)&error, 512, 0);
		AfxMessageBox(error, MB_OK+MB_ICONSTOP);
		bReturn = FALSE;
	}END_CATCH;

	BuildTokens();

	return bReturn;
}

void CEmoticonManager::AddEmoticon(char* szFileName, char* szActivationText)
{

	CEmoticon *eEmoticon = new CEmoticon;

	strcpy(eEmoticon->szActivationText, szActivationText);
	strcpy(eEmoticon->szFileName, szFileName);

	HBITMAP hTmp = (HBITMAP)::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_DEFAULTCOLOR);

	if(!hTmp){

		delete eEmoticon;
		return;
	}

	eEmoticon->hBitmap = CEmoticonManager::ReplaceColor(hTmp, RGB(255,0,255), g_sSettings.GetRGBBg(), 0);
	DeleteObject(hTmp);

	if(!eEmoticon->hBitmap){

		delete eEmoticon;
		return;
	}


	m_aEmoticons.Add(eEmoticon);
}

BOOL CEmoticonManager::Free(void)
{

	while(m_aEmoticons.GetSize()){

		CEmoticon *eEmoticon = m_aEmoticons.GetAt(0);
		DeleteObject(eEmoticon->hBitmap);
		m_aEmoticons.RemoveAt(0);
		delete eEmoticon;
	}

	if(m_pTokens != NULL) delete [] m_pTokens;
	m_pTokens = NULL;

	return TRUE;
}

HBITMAP CEmoticonManager::ReplaceColor(HBITMAP hBmp, COLORREF cOldColor, COLORREF cNewColor, HDC hBmpDC)
{

	HBITMAP hRetBmp = NULL;

	if(hBmp){	

		HDC hBufferDC = CreateCompatibleDC(NULL);	// The DC forSourcebitmap

		if(hBufferDC){

			HBITMAP hTmpBitmap = (HBITMAP) NULL;

			if(hBmpDC){ // only ifhbitmap is selected in a dc

				if(hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP)){

					hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
					SelectObject(hBmpDC, hTmpBitmap);
				}
			}
			
			HGDIOBJ hPreviousBufferObject = SelectObject(hBufferDC, hBmp);
			// here BufferDC contains the bitmap
				
			HDC hDirectDC = CreateCompatibleDC(NULL); // Temporary DC

			if(hDirectDC){

				// size
				BITMAP hBm;
				GetObject(hBmp, sizeof(hBm), &hBm);
						
				// create a BITMAPINFO forthe CreateDIBSection
				BITMAPINFO RGB32BitsBITMAPINFO; 
				ZeroMemory(&RGB32BitsBITMAPINFO, sizeof(BITMAPINFO)); // clean it first ;)

				RGB32BitsBITMAPINFO.bmiHeader.biSize	 = sizeof(BITMAPINFOHEADER);
				RGB32BitsBITMAPINFO.bmiHeader.biWidth	 = hBm.bmWidth;
				RGB32BitsBITMAPINFO.bmiHeader.biHeight	 = hBm.bmHeight;
				RGB32BitsBITMAPINFO.bmiHeader.biPlanes	 = 1;
				RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;

				UINT * ptPixels;	

				HBITMAP hDirectBitmap = CreateDIBSection(
															hDirectDC, 
												            (BITMAPINFO*)&RGB32BitsBITMAPINFO, 
												            DIB_RGB_COLORS,
												            (void**)&ptPixels, 
												            NULL, 0
														);


				if(hDirectBitmap){

					// here DirectBitmap != NULL so ptPixels != NULL no need to test

					HGDIOBJ hPreviousObject = SelectObject(hDirectDC, hDirectBitmap);

					BitBlt(hDirectDC, 0, 0, hBm.bmWidth, hBm.bmHeight, hBufferDC, 0, 0, SRCCOPY);
			
					// here the hDirectDC contains the bitmap

					// Convert COLORREF to RGB (Invert RED and BLUE)
					cOldColor = COLORREF2RGB(cOldColor);
					cNewColor = COLORREF2RGB(cNewColor);

					// After all the inits we can do the job : Replace Color
					for(int i = ((hBm.bmWidth *hBm.bmHeight)-1); i >= 0; i--){

						if(ptPixels[i] == cOldColor){
							
							ptPixels[i] = cNewColor;
						}
					}

					// little clean up
					// Don't delete the result of SelectObject because it's 
					// our modified bitmap (hDirectBitmap) ;)
					SelectObject(hDirectDC, hPreviousObject);
							
					// finish
					hRetBmp = hDirectBitmap;
				}

				// clean up some more... :P
				DeleteDC(hDirectDC);
			}			
			if(hTmpBitmap){

				SelectObject(hBmpDC, hBmp);
				DeleteObject(hTmpBitmap);
			}
			
			SelectObject(hBufferDC, hPreviousBufferObject);
			// BufferDC is now useless
			DeleteDC(hBufferDC);
		}
	}
	return hRetBmp;
}


LPCTSTR CEmoticonManager::FindNext(LPCTSTR pszText, int* pnIndex)
{

	LPCTSTR pszBest = NULL;
	int nIndex = 0, nBest;
	
	if(m_pTokens == NULL) return NULL;
	
	for(LPCTSTR pszToken = m_pTokens ; *pszToken ; nIndex++){

		LPCTSTR pszFind = _tcsstr(pszText, pszToken);
		
		if(pszFind != NULL && (pszBest == NULL || pszFind < pszBest)){

			pszBest = pszFind;
			nBest = nIndex;
		}
		
		pszToken += _tcslen(pszToken) + 1;
	}
	
	if(pszBest && pnIndex) *pnIndex = nBest;
	
	return pszBest;
}

int CEmoticonManager::Lookup(LPCTSTR pszText, int nLen) const
{

	TCHAR cSave = 0;
	int nIndex = 0;
	
	if(m_pTokens == NULL) return -1;
	
	if(nLen >= 0){

		cSave = pszText[nLen];
		((LPTSTR)pszText)[nLen] = 0;
	}
	
	for(LPCTSTR pszToken = m_pTokens ; *pszToken ; nIndex++){

		if(_tcscmp(pszToken, pszText) == 0)	{

			break;
		}
		
		pszToken += _tcslen(pszToken) + 1;
	}
	
	if(nLen >= 0) ((LPTSTR)pszText)[nLen] = cSave;
	
	return (*pszToken != 0) ? nIndex : -1;
}

LPCTSTR	CEmoticonManager::GetText(int nIndex) const
{

	if(m_aEmoticons.GetSize() == 0) return NULL;

	return m_aEmoticons[nIndex]->szActivationText;
	/*if(m_pTokens == NULL) return NULL;
	
	
	for(LPCTSTR pszToken = m_pTokens ; *pszToken ;){

		if(nIndex-- <= 0) return pszToken;
		
		pszToken += _tcslen(pszToken) + 1;
	}

	return NULL;*/
}

void CEmoticonManager::BuildTokens()
{

	int nLength = 2;
	
	for(int i = 0; i < m_aEmoticons.GetSize(); i++){

		nLength += strlen(m_aEmoticons.GetAt(i)->szActivationText) + 1;
	}
	
	ASSERT(m_pTokens == NULL);
	LPTSTR pszOut = m_pTokens = new TCHAR[nLength];
	
	for(i = 0; i <  m_aEmoticons.GetSize(); i++){

		_tcscpy(pszOut,m_aEmoticons.GetAt(i)->szActivationText);
		pszOut += strlen(m_aEmoticons.GetAt(i)->szActivationText) + 1;
	}
	
	*pszOut++ = 0;
}

void CEmoticonManager::FormatText(CRichDocument* pDocument, LPCTSTR pszBody, BOOL bNewlines, BOOL bBold, BOOL bItalic, BOOL bUnderline, COLORREF cr, COLORREF crBg, BOOL bName)
{

	static LPCTSTR pszURLs[] = { _T("\r\n"), _T("http://"), _T("gnutella:"), _T("gnet:"), _T("ftp://"), _T("shareaza:"), _T("ed2k://"), _T("sig2dat:"), _T("www."), NULL };
	//BOOL bBold = FALSE, bItalic = FALSE, bUnderline = FALSE;
	//COLORREF cr = 0;
	CString str;
	
	while(*pszBody){

		LPCTSTR pszToken = _tcschr( pszBody, '\n' );
		
		for(int nURL = 0 ; pszURLs[nURL] != NULL ; nURL++)
		{
			LPCTSTR pszFind = _tcsistr(pszBody, pszURLs[nURL]);
			if(pszFind != NULL && (pszToken == NULL || pszFind < pszToken)) pszToken = pszFind;
		}
		
		int nEmoticon = -1;
		LPCTSTR pszEmoticon = NULL;
		if(g_sSettings.GetEmoticons()){
			
			pszEmoticon = FindNext(pszBody, &nEmoticon);
		}
		
		if(pszEmoticon != NULL && (pszToken == NULL || pszEmoticon < pszToken))
		{
			pszToken = pszEmoticon;
		}
		
		if(pszToken != pszBody)
		{
			if(pszToken != NULL)
			{
				TCHAR cSave = *pszToken;
				*(LPTSTR)pszToken = 0;
				str = pszBody;
				*(LPTSTR)pszToken = cSave;
			}
			else
			{
				str = pszBody;
			}

			CRichElement* pE = 	pDocument->Add((bName ? retName : retText), str, NULL,
									(bBold ? retfBold : 0) |
									(bItalic ? retfItalic : 0) |
									(bUnderline ? retfUnderline : 0) |
									(cr ? retfColor : 0));
			pE->m_cColor = cr;
			pE->m_cBgColor = crBg;
		}
		
		if(pszToken == NULL) break;
		
		pszBody = pszToken;
		if(*pszBody == 0) break;
		
		if(pszEmoticon == pszBody){

			//str.Format(_T("%lu"), nEmoticon);
			//pDocument->Add(retBitmap, str);
			str = GetText(nEmoticon);
			pDocument->Add(retBitmap, str);
			pszBody += _tcslen(GetText(nEmoticon));
			continue;
		}
		else if(pszBody[0] == '\n'){

			if(bNewlines){

				pDocument->Add(retNewline, _T("4"));
			}
			
			pszBody += 2;
			continue;
		}
		else{ // if(*pszBody != '['){

			for(; *pszToken ; pszToken++)
			{
				if(! _istalnum(*pszToken) &&
					_tcschr(_T(":@/?=&%._-+;~#"), *pszToken) == NULL)
				{
					break;
				}
			}
			
			TCHAR cSave = *pszToken;
			*(LPTSTR)pszToken = 0;
			str = pszBody;
			*(LPTSTR)pszToken = cSave;
			
			if(_tcsnicmp(str, _T("www."), 4) == 0) str = _T("http://") + str;
			
			pDocument->Add(retLink, str, str,
				(bBold ? retfBold : 0) |
				(bItalic ? retfItalic : 0) |
				(bUnderline ? retfUnderline : 0));
			
			pszBody = pszToken;
		}  /*
		else if(_tcsnicmp(pszBody, _T("[b]"), 3) == 0){

			bBold = TRUE;
		}
		else if(_tcsnicmp(pszBody, _T("[/b]"), 4) == 0){
			bBold = FALSE;
		}
		else if(_tcsnicmp(pszBody, _T("[i]"), 3) == 0){

			bItalic = TRUE;
		}
		else if(_tcsnicmp(pszBody, _T("[/i]"), 4) == 0){

			bItalic = FALSE;
		}
		else if(_tcsnicmp(pszBody, _T("[u]"), 3) == 0){

			bUnderline = TRUE;
		}
		else if(_tcsnicmp(pszBody, _T("[/u]"), 4) == 0){

			bUnderline = FALSE;
		}
		else if(_tcsnicmp(pszBody, _T("[/c]"), 4) == 0){

			cr = 0;
		}
		else if(_tcsnicmp(pszBody, _T("[c:#"), 4) == 0 && _tcslen(pszBody) >= 4 + 6 + 1){

			_tcsncpy(str.GetBuffer(6), pszBody + 4, 6);
			str.ReleaseBuffer(6);
			int nRed, nGreen, nBlue;
			_stscanf(str.Mid(0, 2), _T("%x"), &nRed);
			_stscanf(str.Mid(2, 2), _T("%x"), &nGreen);
			_stscanf(str.Mid(4, 2), _T("%x"), &nBlue);
			cr = RGB(nRed, nGreen, nBlue);
		}
		if(*pszBody == '['){

			pszToken = _tcschr(pszBody, ']');
			if(pszToken != NULL) pszBody = pszToken + 1;
			else pszBody ++;
		}   */
	}
}

