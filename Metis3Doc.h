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


#if !defined(AFX_METIS3DOC_H__BC25CF82_EA94_456F_A976_8D1CC1E2D97B__INCLUDED_)
#define AFX_METIS3DOC_H__BC25CF82_EA94_456F_A976_8D1CC1E2D97B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMetis3Doc : public CDocument
{
protected: // create from serialization only
	CMetis3Doc();
	DECLARE_DYNCREATE(CMetis3Doc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetis3Doc)
	public:
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_strRoomShort;
	virtual ~CMetis3Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CString m_strRoom;
	CString m_strName;
	DWORD   m_dwFiles;
	WORD    m_wLine;

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMetis3Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METIS3DOC_H__BC25CF82_EA94_456F_A976_8D1CC1E2D97B__INCLUDED_)
