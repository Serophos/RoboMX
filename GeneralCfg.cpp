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
#include "GeneralCfg.h"
#include "Settings.h"
#include ".\generalcfg.h"
#include "MainFrm.h"
#include "FolderDialog.h"

extern CSettings g_sSettings;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralCfg dialog

CGeneralCfg::CGeneralCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CGeneralCfg::IDD, pParent)
	, m_bEmoticons(FALSE)
	, m_bHiliteUsers(FALSE)
	, m_bMaxi(FALSE)
	, m_bUpdate(FALSE)
	, m_bAutoList(FALSE)
	, m_bScroller(FALSE)
{
	m_nHistory = 0;
	m_bHistory = FALSE;
	m_bAllChannels = FALSE;
	m_strChannel = _T("");
	m_strGreeting = _T("");
	m_bLimit = FALSE;
	m_nLimit = 0;
	m_bTimeStamp = FALSE;
	m_nTime = -1;
	m_bPing = FALSE;
	m_bMiniTray = FALSE;
}


void CGeneralCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANNELLIST, m_lbChannels);
	DDX_Control(pDX, IDC_WELCOMES, m_lbGreetings);
	DDX_Text(pDX, IDC_HISTORDEPTH, m_nHistory);
	DDX_Check(pDX, IDC_ENABLE_HISTORY, m_bHistory);
	DDX_Check(pDX, IDC_ALLCHANNELS, m_bAllChannels);
	DDX_Text(pDX, IDC_CHANNELNAME, m_strChannel);
	DDX_Text(pDX, IDC_GREETING, m_strGreeting);
	DDX_Check(pDX, IDC_LINES, m_bLimit);
	DDX_Text(pDX, IDC_LINESNUM, m_nLimit);
	DDX_Check(pDX, IDC_TIMESTAMP, m_bTimeStamp);
	DDX_CBIndex(pDX, IDC_TIME, m_nTime);
	DDX_Check(pDX, IDC_PING, m_bPing);
	DDX_Check(pDX, IDC_MINITRAY, m_bMiniTray);
	DDX_Check(pDX, IDC_EMOTICONS, m_bEmoticons);
	DDX_Check(pDX, IDC_USERHILITE, m_bHiliteUsers);
	DDX_Check(pDX, IDC_MDIMAXI, m_bMaxi);
	DDX_Check(pDX, IDC_UPDATE, m_bUpdate);
	DDX_Check(pDX, IDC_LISTCHANNEL, m_bAutoList);
	DDX_Check(pDX, IDC_SCROLLER, m_bScroller);
	DDX_Check(pDX, IDC_LOG, m_bLog);
	DDX_Text(pDX, IDC_LOGDIR, m_strPath);
}


BEGIN_MESSAGE_MAP(CGeneralCfg, CCfgDlg)
	//{{AFX_MSG_MAP(CGeneralCfg)
	ON_BN_CLICKED(IDC_ALLCHANNELS, OnAllchannels)
	ON_BN_CLICKED(IDC_ADDCHANNEL, OnAddchannel)
	ON_BN_CLICKED(IDC_REMOVECHANNEL, OnRemovechannel)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADDCHANNEL2, OnBnClickedAddchannel2)
	ON_BN_CLICKED(IDC_REMOVECHANNEL2, OnBnClickedRemovechannel2)
	ON_BN_CLICKED(IDC_SELECT_LOG_DIR, OnBnClickedSelectLogDir)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralCfg message handlers

BOOL CGeneralCfg::OnInitDialog() 
{

	CCfgDlg::OnInitDialog();
	
	m_bHistory     = g_sSettings.GetSaveHistory();
	m_nHistory     = g_sSettings.GetHistoryDepth();
	m_bLimit       = g_sSettings.GetLimitChat();
	m_nLimit       = g_sSettings.GetMaxLines();
	m_bAllChannels = g_sSettings.GetDoEnterMsg();
	m_bTimeStamp   = g_sSettings.GetPrintTime();
	m_nTime		   = g_sSettings.GetTimeFmt();
	m_bPing		   = g_sSettings.GetPing();
	m_bMiniTray    = g_sSettings.GetMiniTray();
	m_bEmoticons   = g_sSettings.GetEmoticons();
	m_bHiliteUsers = g_sSettings.GetHiliteUsers();
	m_bUpdate	   = g_sSettings.GetUpdate();
	m_bAutoList	   = g_sSettings.GetAutoList();
	m_bMaxi		   = g_sSettings.GetMaxi();
	m_bScroller    = g_sSettings.GetEnableScroller();
	m_bLog		   = g_sSettings.GetLog();
	m_strPath	   = g_sSettings.GetLogDir();

	UpdateData(FALSE);
	LoadRooms();
	OnAllchannels();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralCfg::OnAllchannels() 
{

	UpdateData(TRUE);
	GetDlgItem(IDC_CHANNELNAME)->EnableWindow(!m_bAllChannels);
	GetDlgItem(IDC_CHANNELLIST)->EnableWindow(!m_bAllChannels);
	GetDlgItem(IDC_ADDCHANNEL)->EnableWindow(!m_bAllChannels);
	GetDlgItem(IDC_REMOVECHANNEL)->EnableWindow(!m_bAllChannels);
}

void CGeneralCfg::LoadRooms()
{

	g_sSettings.LoadRooms();

	m_lbChannels.ResetContent();
	m_lbGreetings.ResetContent();
	for(int i = 0; i < g_sSettings.m_aRooms.GetSize(); i++){

		m_lbChannels.InsertString(m_lbChannels.GetCount(), g_sSettings.m_aRooms[i]);
	}
	for(i = 0; i < g_sSettings.m_aGreetings.GetSize(); i++){

		m_lbGreetings.InsertString(m_lbGreetings.GetCount(), g_sSettings.m_aGreetings[i]);
	}
}

void CGeneralCfg::SaveRooms()
{

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\rooms.ini";
	CStdioFile ini;
	CString strBuffer;

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareExclusive);
		ini.SetLength(0);

		for(int i = 0; i < m_lbChannels.GetCount(); i++){
			
			m_lbChannels.GetText(i, strBuffer);
			ini.WriteString(strBuffer + "\n");
		}
		ini.Flush();
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);

	}END_CATCH;

	strIniFile = g_sSettings.GetWorkingDir() + "\\hello.ini";
	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareExclusive);
		ini.SetLength(0);

		for(int i = 0; i < m_lbGreetings.GetCount(); i++){
			
			m_lbGreetings.GetText(i, strBuffer);
			ini.WriteString(strBuffer + "\n");
		}
		ini.Flush();
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);

	}END_CATCH;
}

void CGeneralCfg::Apply()
{

	UpdateData(TRUE);

	g_sSettings.SetDoEnterMsg(m_bAllChannels);	
	g_sSettings.SetSaveHistory(m_bHistory);
	g_sSettings.SetHistoryDepth(m_nHistory);
	g_sSettings.SetLimitChat(m_bLimit);
	g_sSettings.SetMaxLines(m_nLimit);
	g_sSettings.SetPrintTime(m_bTimeStamp);
	g_sSettings.SetPing(m_bPing);
	g_sSettings.SetTimeFmt(m_nTime);
	g_sSettings.SetMiniTray(m_bMiniTray);
	g_sSettings.SetEmoticons(m_bEmoticons);
	g_sSettings.SetHiliteUsers(m_bHiliteUsers);
	g_sSettings.SetUpdate(m_bUpdate);
	g_sSettings.SetAutoList(m_bAutoList);
	g_sSettings.SetMaxi(m_bMaxi);
	g_sSettings.SetEnableScroller(m_bScroller);
	g_sSettings.SetLog(m_bLog);
	g_sSettings.SetLogDir(m_strPath);
	SaveRooms();
	LoadRooms();
}

void CGeneralCfg::OnAddchannel() 
{
	
	UpdateData(TRUE);
	if(m_strChannel.IsEmpty()) return;

	m_lbChannels.AddString(m_strChannel);
	m_strChannel.Empty();
	UpdateData(FALSE);
}

void CGeneralCfg::OnRemovechannel() 
{

	if(m_lbChannels.GetCurSel() != -1){

		m_lbChannels.DeleteString(m_lbChannels.GetCurSel());
	}
}


void CGeneralCfg::OnBnClickedAddchannel2()
{

	UpdateData(TRUE);
	if(m_strGreeting.IsEmpty()) return;

	m_lbGreetings.AddString(m_strGreeting);
	m_strGreeting.Empty();
	UpdateData(FALSE);
}

void CGeneralCfg::OnBnClickedRemovechannel2()
{

	if(m_lbGreetings.GetCurSel() != -1){

		m_lbGreetings.DeleteString(m_lbGreetings.GetCurSel());
	}
}

void CGeneralCfg::OnBnClickedSelectLogDir()
{

	UpdateData(TRUE);
	CFolderDialog dlg;
	if(dlg.DoModal() == IDOK){

		m_strPath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}
