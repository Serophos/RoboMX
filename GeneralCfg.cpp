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

extern CSettings g_sSettings;
extern CStringArray g_aRooms;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralCfg dialog


CGeneralCfg::CGeneralCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CGeneralCfg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeneralCfg)
	m_strLanguage = _T("");
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
	//}}AFX_DATA_INIT
}


void CGeneralCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralCfg)
	DDX_Control(pDX, IDC_PLUGINLIST, m_lcPlugins);
	DDX_Control(pDX, IDC_CHANNELLIST, m_lbChannels);
	DDX_Control(pDX, IDC_LANG, m_cbLang);
	DDX_CBString(pDX, IDC_LANG, m_strLanguage);
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralCfg, CCfgDlg)
	//{{AFX_MSG_MAP(CGeneralCfg)
	ON_BN_CLICKED(IDC_ALLCHANNELS, OnAllchannels)
	ON_BN_CLICKED(IDC_ADDCHANNEL, OnAddchannel)
	ON_BN_CLICKED(IDC_REMOVECHANNEL, OnRemovechannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralCfg message handlers

BOOL CGeneralCfg::OnInitDialog() 
{

	CCfgDlg::OnInitDialog();
	
	m_lcPlugins.InsertColumn(0, "Plugin", LVCFMT_LEFT, 100);
	m_lcPlugins.InsertColumn(1, "Description", LVCFMT_LEFT, 150);
	m_lcPlugins.InsertColumn(2, "Author", LVCFMT_LEFT, 120);
	ListView_SetExtendedListViewStyle(m_lcPlugins.m_hWnd, LVS_EX_FULLROWSELECT);

	m_bHistory     = g_sSettings.GetSaveHistory();
	m_nHistory     = g_sSettings.GetHistoryDepth();
	m_bLimit       = g_sSettings.GetLimitChat();
	m_nLimit       = g_sSettings.GetMaxLines();
	m_strGreeting  = g_sSettings.GetEnterMsg();
	m_strLanguage  = g_sSettings.GetLanguage();
	m_bAllChannels = g_sSettings.GetDoEnterMsg();
	m_bTimeStamp   = g_sSettings.GetPrintTime();
	m_nTime		   = g_sSettings.GetTimeFmt();
	m_bPing		   = g_sSettings.GetPing();
	m_bMiniTray    = g_sSettings.GetMiniTray();

	CFileFind finder;
	BOOL bResult = finder.FindFile(g_sSettings.GetWorkingDir() + "\\languages\\*.txt");
		
	while(bResult){

		bResult = finder.FindNextFile();
		m_cbLang.AddString(finder.GetFileTitle());
	}

	if(m_cbLang.GetCount() > 0){

		int nIndex = m_cbLang.FindStringExact(0, m_strLanguage);
		if(nIndex != CB_ERR){

			m_cbLang.SetCurSel(nIndex);
		}
	}
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

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\rooms.ini";
	BOOL bReturn = TRUE;
	CStdioFile ini;
	CString strBuffer;
	g_aRooms.RemoveAll();
	m_lbChannels.ResetContent();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				g_aRooms.Add(strBuffer);
				m_lbChannels.InsertString(m_lbChannels.GetCount(), strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		//AfxMessageBox("Error while reading Autocompletion text!", MB_OK+MB_ICONSTOP);
		return;
	}END_CATCH;
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

		AfxMessageBox("Error during file operation.", MB_OK+MB_ICONSTOP);

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
	g_sSettings.SetEnterMsg(m_strGreeting);
	g_sSettings.SetLanguage(m_strLanguage);
	g_sSettings.SetPrintTime(m_bTimeStamp);
	g_sSettings.SetPing(m_bPing);
	g_sSettings.SetTimeFmt(m_nTime);
	g_sSettings.SetMiniTray(m_bMiniTray);
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
