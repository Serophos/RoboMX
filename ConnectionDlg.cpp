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
#include "ConnectionDlg.h"
#include "Ini.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog


CConnectionDlg::CConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectionDlg)
	m_nLine = -1;
	m_dwFiles = 0;
	m_strName = _T("RoboMX.[:]..");
	m_strRoom = _T("");
	//}}AFX_DATA_INIT
}


void CConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionDlg)
	DDX_Control(pDX, IDC_ROOM, m_cbRoom);
	DDX_CBIndex(pDX, IDC_LINE, m_nLine);
	DDX_Text(pDX, IDC_FILES, m_dwFiles);
	DDV_MinMaxDWord(pDX, m_dwFiles, 0, 65535);
	DDX_Text(pDX, IDC_USERNAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 38);
	DDX_CBString(pDX, IDC_ROOM, m_strRoom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectionDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectionDlg)
	ON_BN_CLICKED(IDC_USENODE, OnUsenode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg message handlers

BOOL CConnectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	TCHAR szBuffer[_MAX_PATH]; 
	::GetModuleFileName(GetApp()->m_hInstance, szBuffer, _MAX_PATH);
	CString strTmp;
	m_strWd.Format("%s", szBuffer);
	m_strWd = m_strWd.Left(m_strWd.ReverseFind('\\'));

	CIni ini;
	ini.SetIniFileName(m_strWd + "\\RoboMX.ini");

	for(int i = 0; i < 10; i++){

		strTmp.Format("Room_%d", i);
		strTmp = ini.GetValue("RoomList", strTmp, "");
		if(!strTmp.IsEmpty()){

			m_cbRoom.AddString(strTmp);
		}

	}
	
	m_strName = ini.GetValue("UserInfo", "Name", "RoboMX.[:].494");
	m_nLine   = ini.GetValue("UserInfo", "Line", 0);
	m_dwFiles = ini.GetValue("UserInfo", "Files", 494);
	
	m_strRoom = ((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom;
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectionDlg::OnOK() 
{

	if(!UpdateData(TRUE)) return;

	if(m_strRoom.IsEmpty()){

		AfxMessageBox("Roomname must not be emtpy!", MB_ICONINFORMATION);
		return;
	}
	if(m_strRoom.Find("_", 0) < 0){

		AfxMessageBox("Roomname does not appear to be valid!", MB_ICONINFORMATION);
		return;
	}
	if(m_strName.IsEmpty()){

		AfxMessageBox("Username must not be emtpy!", MB_ICONINFORMATION);
		return;
	}
	if((m_strName.Find(" ") >= 0) || (m_strName.Find("\\rtf") >= 0)){

		AfxMessageBox("Username contains illegal characters!", MB_ICONINFORMATION);
		return;
	}
	
	if(m_cbRoom.FindStringExact(-1, m_strRoom) == CB_ERR){

		m_cbRoom.AddString(m_strRoom);
	}
	
	CString strTmp, strTmp2;
	CIni ini;
	ini.SetIniFileName(m_strWd + "\\RoboMX.ini");

	for(int i = 0; (i < m_cbRoom.GetCount()) && (i < 10) ; i++){

		strTmp.Format("Room_%d", i);
		m_cbRoom.GetLBText(i, strTmp2);

		if(!strTmp2.IsEmpty()){

			ini.SetValue("RoomList", strTmp, strTmp2);
		}

	}
	
	ini.SetValue("UserInfo", "Name", m_strName);
	ini.SetValue("UserInfo", "Line", m_nLine);
	ini.SetValue("UserInfo", "Files", m_dwFiles);

	CDialog::OnOK();
}

void CConnectionDlg::OnUsenode() 
{

	AfxMessageBox("Sorry, this features is not implemented yet");
}
