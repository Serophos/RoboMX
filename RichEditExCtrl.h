/*
** Copyright (C) 2004 Thees Winkler
**  
4** This program is free software; you can redistribute it and/or modify
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

#if !defined(AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_)
#define AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichEditExCtrl.h : header file
//
#include "ImageDataObject.h"

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) \
                                 | ((Color << 16) & 0xff0000)

class Emoticon
{
public:
	Emoticon(){ hBitmap = 0;}
	~Emoticon(){ if(hBitmap) DeleteObject(hBitmap);}

	HBITMAP hBitmap;
	char szFileName[1024];
	char szActivationText[64];


	static HBITMAP ReplaceColor(HBITMAP hBmp,COLORREF cOldColor,COLORREF cNewColor,HDC hBmpDC)
	{

		HBITMAP hRetBmp = NULL;

		if(hBmp){	

			HDC hBufferDC = CreateCompatibleDC(NULL);	// The DC for Sourcebitmap

			if(hBufferDC){

				HBITMAP hTmpBitmap = (HBITMAP) NULL;

				if(hBmpDC){ // only if hbitmap is selected in a dc

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
							
					// create a BITMAPINFO for the CreateDIBSection
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
};

///////////////////////////////////////////////////////////////////////////////
// CRichEditExCtrl window

class CRichEditExCtrl : public CRichEditCtrl
{
// Construction
public:
	CRichEditExCtrl();

	CHARFORMAT2 m_cfDefault;
	CHARFORMAT2 m_cfUse;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditExCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetSelectionCharFormat(CHARFORMAT2& cf);
	void Init(UINT m_nID);
	BOOL InsertBitmap(HBITMAP hBitmap);
	void SetText(LPCSTR lpszText, COLORREF text, COLORREF bg);
	virtual ~CRichEditExCtrl();

	static char* stristr(const char * str1, const char * str2);
	static void strstp(const char *in, char *before, const char *txt, char *after);
	// Generated message map functions
protected:
	//{{AFX_MSG(CRichEditExCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	UINT m_nID;

	void AppendText(LPCSTR lpszText, COLORREF text, COLORREF bg);
	// datamembers for bitmap storage
};

///////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITEXCTRL_H__4714BC01_81C2_4626_919B_262F32C149B5__INCLUDED_)
