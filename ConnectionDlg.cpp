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
#include "settings.h"
#include "MainFrm.h"
#include "Util.h"
#include "Tokenizer.h"
#include ".\connectiondlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog
extern CSettings g_sSettings;

CConnectionDlg::CConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectionDlg)
	m_nLine = -1;
	m_dwFiles = 0;
	m_strName = _T("RoboMX.[:]..");
	m_strRoom = _T("");
	m_bOldJoin = FALSE;
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
	DDV_MaxChars(pDX, m_strName, 44);
	DDX_CBString(pDX, IDC_ROOM, m_strRoom);
	DDX_Check(pDX, IDC_MXCHATD, m_bOldJoin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectionDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectionDlg)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg message handlers

BOOL CConnectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strTmp;
	m_strWd = g_sSettings.GetWorkingDir();

	CIni ini;
	ini.SetIniFileName(m_strWd + "\\RoboMX.ini");

	for(int i = 0; i < 10; i++){

		strTmp.Format("Room_%d", i);
		strTmp = ini.GetValue("RoomList", strTmp, "");
		if(!strTmp.IsEmpty()){

			m_cbRoom.AddString(strTmp);
		}

	}
	
	m_strName = g_sSettings.GetNickname();
	m_nLine   = g_sSettings.GetLine();
	m_dwFiles = g_sSettings.GetFiles();
	
	m_strRoom = ((CMainFrame*)GetApp()->m_pMainWnd)->m_strRoom;
	
	UpdateData(FALSE);

	m_cbRoom.SetFocus();
	m_cbRoom.SetEditSel(m_strRoom.GetLength()-12, -1);

	return FALSE;  // return TRUE unless you set the focus to a control
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
	if(m_strName.GetLength() < 1){

		AfxMessageBox("Username too short.", MB_ICONINFORMATION);
		return;
	}
	if((m_strName.Find(" ") >= 0) || (m_strName.Find("\\rtf") >= 0)){

		AfxMessageBox("Username contains illegal characters!", MB_ICONINFORMATION);
		return;
	}

	Util::MakeValidUserName(m_strName);

	if(m_strName.GetLength() > 44){
		
		AfxMessageBox("Username is too long. Must not be longer then 38 characters (without ID).", MB_ICONINFORMATION);
		return;
	}

	FixRoomName(m_strRoom);

	UpdateData(FALSE);

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
	
	g_sSettings.SetNickname(m_strName);
	g_sSettings.SetLine(m_nLine);
	g_sSettings.SetFiles(m_dwFiles);

	CDialog::OnOK();
}


void CConnectionDlg::OnClear() 
{

	CIni ini;
	ini.SetIniFileName(m_strWd + "\\RoboMX.ini");
	m_cbRoom.ResetContent();

	CString strTmp;

	for(int i = 0; i < 10 ; i++){

		strTmp.Format("Room_%d", i);

		ini.SetValue("RoomList", strTmp, "");
	}
}

void CConnectionDlg::FixRoomName(CString& strRoom)
{

	CString strAppendix;

	strRoom.TrimRight();

	int n = strRoom.ReverseFind('_');
	
	if(n > 0){

		strAppendix = strRoom.Mid(n);
    }
	else return;

	strAppendix.Remove('_');
	
	if(strAppendix.GetLength() == 12) return; // this is the normal case

	// if we are here the rommane has format
	// room_XXX.XXX.XXX.XXX:XXXX

	CTokenizer token(strAppendix, ".:");
	
	CString strTmp;
	int nA = 0, nB = 0, nC = 0, nD = 0, nPort = 0;

	token.Next(strTmp);
	nA = atoi(strTmp);

	token.Next(strTmp);
	nB = atoi(strTmp);

	token.Next(strTmp);
	nC = atoi(strTmp);

	token.Next(strTmp);
	nD = atoi(strTmp);

	token.Next(strTmp);
	nPort = atoi(strTmp);

	strTmp = strRoom.Left(n+1);
	strAppendix.Format("%02X%02X%02X%02X%04X", nD, nC, nB, nA, nPort);

	strRoom = strTmp + strAppendix;
}
