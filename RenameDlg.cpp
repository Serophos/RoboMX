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
#include "Metis3.h"
#include "RenameDlg.h"
#include ".\renamedlg.h"
#include "settings.h"
#include "util.h"
#include "ini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg dialog

extern CSettings g_sSettings;

CRenameDlg::CRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameDlg::IDD, pParent)
	, m_bAllRooms(FALSE)
{
	//{{AFX_DATA_INIT(CRenameDlg)
	m_strName = _T("");
	m_nLine = -1;
	m_dwFiles = 0;
	//}}AFX_DATA_INIT
}


void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_CBIndex(pDX, IDC_LINE, m_nLine);
	DDX_Text(pDX, IDC_FILES, m_dwFiles);
	DDV_MaxChars(pDX, m_strName, 44);
	DDV_MinMaxDWord(pDX, m_dwFiles, 0, 65535);
	DDX_Check(pDX, IDC_ALLROOMS, m_bAllRooms);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg message handlers

void CRenameDlg::OnOK() 
{

	if(!UpdateData(TRUE)) return;

	if(m_strName.GetLength() < 1){

		AfxMessageBox(IDS_ERROR_NAME_SHORT, MB_ICONINFORMATION);
		return;
	}

	if((m_strName.Find(" ") >= 0) || (m_strName.Find("\\rtf") >= 0)){

		AfxMessageBox(IDS_ERROR_NAME_INVALID, MB_ICONINFORMATION);
		return;
	}

	Util::MakeValidUserName(m_strName);

	if(m_strName.GetLength() > 44){
		
		AfxMessageBox(IDS_ERROR_NAME_LONG, MB_ICONINFORMATION);
		return;
	}

	UpdateData(FALSE);
	
	g_sSettings.SetNickname(m_strName);
	g_sSettings.SetLine(m_nLine);
	g_sSettings.SetFiles(m_dwFiles);
	
	CDialog::OnOK();
}
