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

#if !defined(AFX_METIS3_H__25DFE5AD_544B_4E6F_89A8_1C06FF7D7318__INCLUDED_)
#define AFX_METIS3_H__25DFE5AD_544B_4E6F_89A8_1C06FF7D7318__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMetis3App:
// See Metis3.cpp for the implementation of this class
//

class CMetis3View;

class CMetis3App : public CWinApp
{
public:
	CMetis3App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetis3App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	
	int m_nWSA;
	//CMetis3View* m_pView;
// Implementation
	//{{AFX_MSG(CMetis3App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

inline CMetis3App *GetApp()
{
	return (CMetis3App *)AfxGetApp();
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METIS3_H__25DFE5AD_544B_4E6F_89A8_1C06FF7D7318__INCLUDED_)
