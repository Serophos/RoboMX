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
#include "MessageCfg.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageCfg dialog

extern CStringArray g_aQuick;
extern CSettings    g_sSettings;

CMessageCfg::CMessageCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CMessageCfg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageCfg)
	m_strAdd = _T("");
	m_strJoin = _T("");
	m_strWatch = _T("");
	m_strListen = _T("");
	m_strPart = _T("");
	//}}AFX_DATA_INIT
}


void CMessageCfg::DoDataExchange(CDataExchange* pDX)
{
	CCfgDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageCfg)
	DDX_Control(pDX, IDC_QUICKCMDS, m_lbQuick);
	DDX_Text(pDX, IDC_MSG_ADD, m_strAdd);
	DDX_Text(pDX, IDC_JOIN, m_strJoin);
	DDX_Text(pDX, IDC_NOW_WATCHING, m_strWatch);
	DDX_Text(pDX, IDC_NOWLISTENING, m_strListen);
	DDX_Text(pDX, IDC_PART, m_strPart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageCfg, CCfgDlg)
	//{{AFX_MSG_MAP(CMessageCfg)
	ON_BN_CLICKED(IDC_MSG_ADD_BTN, OnMsgAddBtn)
	ON_BN_CLICKED(IDC_QUICKREM, OnQuickrem)
	ON_BN_CLICKED(IDC_QUICKUP, OnQuickup)
	ON_BN_CLICKED(IDC_QUICKDOWN, OnQuickdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageCfg message handlers

BOOL CMessageCfg::OnInitDialog() 
{

	CCfgDlg::OnInitDialog();
	
	LoadQuickCmds();

	m_strJoin = g_sSettings.GetJoin();
	m_strListen = g_sSettings.GetWinampMsg();
	m_strPart = g_sSettings.GetPart();
	m_strWatch = g_sSettings.GetVideoMsg();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageCfg::Apply()
{
	UpdateData(TRUE);
	
	SaveQuickCmds();
	g_sSettings.SetJoin(m_strJoin);
	g_sSettings.SetWinampMsg(m_strListen);
	g_sSettings.SetPart(m_strPart);
	g_sSettings.SetVideoMsg(m_strWatch);
	LoadQuickCmds();
}


void CMessageCfg::LoadQuickCmds()
{

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\quick.ini";
	BOOL bReturn = TRUE;
	CStdioFile ini;
	CString strBuffer;
	g_aQuick.RemoveAll();
	m_lbQuick.ResetContent();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				g_aQuick.Add(strBuffer);
				m_lbQuick.InsertString(m_lbQuick.GetCount(), strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

	}END_CATCH;
}

void CMessageCfg::SaveQuickCmds()
{

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\quick.ini";
	CStdioFile ini;
	CString strBuffer;

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareExclusive);

		for(int i = 0; i < m_lbQuick.GetCount(); i++){
			
			m_lbQuick.GetText(i, strBuffer);
			ini.WriteString(strBuffer + "\n");
		}
		ini.Flush();
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox("Error during file operation", MB_OK+MB_ICONSTOP);

	}END_CATCH;
}

void CMessageCfg::OnMsgAddBtn() 
{

	UpdateData(TRUE);
	if(m_strAdd.IsEmpty()) return;

	m_lbQuick.AddString(m_strAdd);
	g_aQuick.Add(m_strAdd);
	m_strAdd.Empty();

	UpdateData(FALSE);
}

void CMessageCfg::OnQuickrem() 
{

	int nSel = m_lbQuick.GetCurSel();
	if(nSel != -1){

		m_lbQuick.DeleteString(nSel);
		g_aQuick.RemoveAt(nSel);
	}
}

void CMessageCfg::OnQuickup() 
{
	
	int nSel = m_lbQuick.GetCurSel();
	if(nSel != LB_ERR){

		CString strTmp;
		m_lbQuick.GetText(nSel, strTmp);
		m_lbQuick.DeleteString(nSel);
		g_aQuick.RemoveAt(nSel--);
		m_lbQuick.InsertString(nSel < 0 ? m_lbQuick.GetCount() : nSel, strTmp);
		g_aQuick.InsertAt(nSel < 0 ? g_aQuick.GetSize() : nSel, strTmp);
		m_lbQuick.SetCurSel(nSel < 0 ? m_lbQuick.GetCount()-1 : nSel);
	}	
}

void CMessageCfg::OnQuickdown() 
{
	
	int nSel = m_lbQuick.GetCurSel();
	if(nSel != LB_ERR){

		CString strTmp;
		m_lbQuick.GetText(nSel, strTmp);
		m_lbQuick.DeleteString(nSel);
		g_aQuick.RemoveAt(nSel++);
		m_lbQuick.InsertString(nSel > m_lbQuick.GetCount() ? 0 : nSel,strTmp);
		g_aQuick.InsertAt(nSel > g_aQuick.GetSize() ? 0 : nSel, strTmp);
		m_lbQuick.SetCurSel(nSel > m_lbQuick.GetCount()-1 ? 0 : nSel);
	}	
}
