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
#include "SfxCfg.h"
#include "Settings.h"
#include "Tokenizer.h"
#include "SfxEdit.h"
#include <mmsystem.h>
#include ".\sfxcfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSfxCfg dialog

CArray<SOUND, SOUND> g_aSounds;

extern CSettings g_sSettings;

CSfxCfg::CSfxCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CSfxCfg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSfxCfg)
	m_strConnect = _T("");
	m_strDisconnect = _T("");
	m_strMotd = _T("");
	m_strRedirect = _T("");
	m_strStart = _T("");
	m_strError = _T("");
	m_strTopic = _T("");
	m_bChatSfx = FALSE;
	m_bSfx = FALSE;
	//}}AFX_DATA_INIT
}


void CSfxCfg::DoDataExchange(CDataExchange* pDX)
{
	CCfgDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSfxCfg)
	DDX_Control(pDX, IDC_SOUNDLIST, m_lcSounds);
	DDX_Text(pDX, IDC_STATIC_CONNECT, m_strConnect);
	DDX_Text(pDX, IDC_STATIC_DISCONNECTED, m_strDisconnect);
	DDX_Text(pDX, IDC_STATIC_MOTD, m_strMotd);
	DDX_Text(pDX, IDC_STATIC_REDIRECTED, m_strRedirect);
	DDX_Text(pDX, IDC_STATIC_STARTUP, m_strStart);
	DDX_Text(pDX, IDC_STATIC_SYSERROR, m_strError);
	DDX_Text(pDX, IDC_STATIC_TOPIC, m_strTopic);
	DDX_Check(pDX, IDC_ENABLE_CSFX, m_bChatSfx);
	DDX_Check(pDX, IDC_ENABLE_SFX, m_bSfx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSfxCfg, CCfgDlg)
	//{{AFX_MSG_MAP(CSfxCfg)
	ON_BN_CLICKED(IDC_CONNECTED, OnConnected)
	ON_BN_CLICKED(IDC_DISCONNECTED, OnDisconnected)
	ON_BN_CLICKED(IDC_MOTD, OnMotd)
	ON_BN_CLICKED(IDC_REDIRECTED, OnRedirected)
	ON_BN_CLICKED(IDC_STARTUP, OnStartup)
	ON_BN_CLICKED(IDC_SYSERROR, OnSyserror)
	ON_BN_CLICKED(IDC_TOPIC, OnTopic)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PLAY_CONNECTED, OnBnClickedPlayConnected)
	ON_BN_CLICKED(IDC_PLAY_DISCONNECTED, OnBnClickedPlayDisconnected)
	ON_BN_CLICKED(IDC_PLAY_TOPIC, OnBnClickedPlayTopic)
	ON_BN_CLICKED(IDC_PLAY_MOTD, OnBnClickedPlayMotd)
	ON_BN_CLICKED(IDC_PLAY_REDIRECTED, OnBnClickedPlayRedirected)
	ON_BN_CLICKED(IDC_PLAY_START, OnBnClickedPlayStart)
	ON_BN_CLICKED(IDC_PLAY_ERROR, OnBnClickedPlayError)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSfxCfg message handlers

BOOL CSfxCfg::OnInitDialog() 
{

	CCfgDlg::OnInitDialog();
	
	ListView_SetExtendedListViewStyle(m_lcSounds.m_hWnd, LVS_EX_FULLROWSELECT);

	m_lcSounds.InsertColumn(0, "Trigger", LVCFMT_LEFT, 150);
	m_lcSounds.InsertColumn(1, "Sound", LVCFMT_LEFT, 250);
	
	m_strConnect = g_sSettings.GetSfxJoin();
	m_strDisconnect = g_sSettings.GetSfxPart();
	m_strMotd = g_sSettings.GetSfxMotd();
	m_strRedirect = g_sSettings.GetSfxRedirect();
	m_strStart = g_sSettings.GetSfxStart();
	m_strError = g_sSettings.GetSfxError();
	m_strTopic = g_sSettings.GetSfxTopic();
	m_bChatSfx = g_sSettings.GetSfxChatSfx();
	m_bSfx = g_sSettings.GetSoundFX();
	
	LoadSounds();

	UpdateData(FALSE);

	return TRUE;  
}

void CSfxCfg::Apply()
{

	UpdateData(TRUE);

	g_sSettings.SetSfxJoin(m_strConnect);
	g_sSettings.SetSfxPart(m_strDisconnect);
	g_sSettings.SetSfxMotd(m_strMotd);
	g_sSettings.SetSfxRedirect(m_strRedirect);
	g_sSettings.SetSfxStart(m_strStart);
	g_sSettings.SetSfxError(m_strError);
	g_sSettings.SetSfxTopic(m_strTopic);
	g_sSettings.SetSfxChatSfx(m_bChatSfx);
	g_sSettings.SetSoundFX(m_bSfx);
	WriteSounds();
	LoadSounds();
}

void CSfxCfg::LoadSounds()
{

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\MXSound.ini";

	CStdioFile ini;
	CString strBuffer;
	int nMode = -1;
	int p = 0;
	
	g_aSounds.RemoveAll();
	m_lcSounds.DeleteAllItems();

	CString strSection;
	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText);

		while(ini.ReadString(strBuffer)){

			strBuffer.TrimLeft();
			strBuffer.TrimRight();

			if(strBuffer.Left(2) == "//") continue;
			if(strBuffer.Find("[Sounds]", 0) == 0){
				
				nMode = 0;
				continue;
			}
			if(strBuffer.Left(1) == "#"){
				
				strBuffer = strBuffer.Mid(1);
				if(strBuffer.IsEmpty() || (nMode != 0)){

					continue;
				}

				CString strEvent, strResp;
				
				CTokenizer token(strBuffer, "¨");

				if(!token.Next(strEvent) || !token.Next(strResp)) continue;
					
				if(nMode == 0){
				
					
					strEvent.MakeLower();
					if(strResp.GetLength() < 8){

						strResp = g_sSettings.GetWorkingDir() + "\\" + strResp;
					}
					else if(strResp.GetAt(1) != ':'){

						strResp = g_sSettings.GetWorkingDir() + "\\" + strResp;
					}

					SOUND s;
					s.strTrigger = strEvent;
					s.strSound = strResp;
					g_aSounds.Add(s);

					p = m_lcSounds.InsertItem(m_lcSounds.GetItemCount(), strEvent, 0);
					m_lcSounds.SetItemText(p, 1, strResp);
				}
				else{

					TRACE("HELP\n");

				}

			}
		}
		ini.Close();
	}
	CATCH(CFileException, e){

		AfxMessageBox("Error during file operation!", MB_OK+MB_ICONSTOP);

	}END_CATCH;
}

void CSfxCfg::WriteSounds()
{

	CStdioFile ini;
	CString strLine;
	TRY{

		ini.Open(g_sSettings.GetWorkingDir() + "\\MXSound.ini", CFile::modeCreate|CFile::modeWrite|CFile::typeText);
		ini.SetLength(0);
		
		ini.WriteString("// MXSound.ini. Automatically generated by Configurator,exe\n");
		ini.WriteString("// Do not edit manually if you dont know what you are doing\n");
		
		ini.WriteString("\n\n[Sounds]\n");
		for(int i = 0; i < m_lcSounds.GetItemCount(); i++){
			

			strLine.Format("#%s¨%s\n", m_lcSounds.GetItemText(i, 0), m_lcSounds.GetItemText(i, 1));
			ini.WriteString(strLine);
		}
		ini.WriteString("//Sounds End\n\n");
		ini.Flush();
		ini.Close();
	}
	CATCH(CFileException, e){

		AfxMessageBox("Error during file operation!", MB_OK+MB_ICONSTOP);

	}END_CATCH;
}

void CSfxCfg::OnConnected() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strConnect, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strConnect = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnDisconnected() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strDisconnect, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strDisconnect = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnMotd() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strMotd, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strMotd = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnRedirected() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strRedirect, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strRedirect = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnStartup() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strStart, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strStart = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnSyserror() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strError, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strError = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnTopic() 
{

	static char BASED_CODE szFilter[] = "Wave Audio Files(*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", m_strTopic, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strTopic = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSfxCfg::OnAdd() 
{

	LVITEM iItem = {0};
	iItem.mask = LVIF_IMAGE | LVIF_TEXT;
	iItem.iItem = m_lcSounds.GetItemCount();

	CSfxEdit dlg;
	dlg.Init(0, "Trigger", "sound.wav");
	if(dlg.DoModal() == IDOK){
		
		int nEvent;
		CString strTrigger, strReaction, strUser;
		dlg.GetValues(nEvent, strTrigger, strReaction);
		iItem.mask = LVIF_TEXT | LVIF_IMAGE;
		iItem.pszText = (LPTSTR)(LPCSTR)strTrigger;
		iItem.iImage = nEvent;
		m_lcSounds.InsertItem(&iItem);
		m_lcSounds.SetItemText(iItem.iItem, 1, strReaction);
	}
}

void CSfxCfg::OnDelete() 
{

	if(m_lcSounds.GetSelectedCount() == 0){

		AfxMessageBox("Please select Event to delete first!", MB_OK+MB_ICONINFORMATION);
		return;
	}
	LVITEM iItem = {0};
	iItem.mask = LVIF_IMAGE | LVIF_TEXT;
	iItem.iItem = m_lcSounds.GetSelectionMark();
	m_lcSounds.GetItem(&iItem);

	m_lcSounds.DeleteItem(m_lcSounds.GetSelectionMark());
}

void CSfxCfg::OnEdit() 
{

	if(m_lcSounds.GetSelectedCount() == 0){

		AfxMessageBox("Please select Event to edit first!", MB_OK+MB_ICONINFORMATION);
		return;
	}

	LVITEM iItem = {0};
	iItem.mask = LVIF_IMAGE | LVIF_TEXT;
	iItem.iItem = m_lcSounds.GetSelectionMark();
	m_lcSounds.GetItem(&iItem);

	CSfxEdit dlg;
	dlg.Init(
			   iItem.iImage, 
			   m_lcSounds.GetItemText(iItem.iItem, 0), 
			   m_lcSounds.GetItemText(iItem.iItem, 1)
		   );

	if(dlg.DoModal() == IDOK){
		
		int nEvent;
		CString strTrigger, strReaction;
		dlg.GetValues(nEvent, strTrigger, strReaction);
		iItem.mask = LVIF_TEXT | LVIF_IMAGE;
		iItem.pszText = (LPTSTR)(LPCSTR)strTrigger;
		iItem.iImage = nEvent;
		m_lcSounds.SetItem(&iItem);
		m_lcSounds.SetItemText(iItem.iItem, 1, strReaction);
	}
}

void CSfxCfg::OnPlay() 
{

	if(m_lcSounds.GetSelectedCount() == 0){

		AfxMessageBox("Please select Event to play first!", MB_OK+MB_ICONINFORMATION);
		return;
	}

	LVITEM iItem = {0};
	iItem.mask = LVIF_IMAGE | LVIF_TEXT;
	iItem.iItem = m_lcSounds.GetSelectionMark();
	m_lcSounds.GetItem(&iItem);
	if(!PlaySound(m_lcSounds.GetItemText(iItem.iItem, 1), NULL, SND_FILENAME|SND_ASYNC)){

		AfxMessageBox("Could not play file " + m_lcSounds.GetItemText(iItem.iItem, 1), MB_ICONSTOP);
	}
}

void CSfxCfg::OnBnClickedPlayConnected()
{

	PlaySound(m_strConnect, NULL, SND_FILENAME|SND_ASYNC);
}

void CSfxCfg::OnBnClickedPlayDisconnected()
{

	PlaySound(m_strDisconnect, NULL, SND_FILENAME|SND_ASYNC);
}

void CSfxCfg::OnBnClickedPlayTopic()
{

	PlaySound(m_strTopic, NULL, SND_FILENAME|SND_ASYNC);
}

void CSfxCfg::OnBnClickedPlayMotd()
{

	PlaySound(m_strMotd, NULL, SND_FILENAME|SND_ASYNC);
}

void CSfxCfg::OnBnClickedPlayRedirected()
{

	PlaySound(m_strRedirect, NULL, SND_FILENAME|SND_ASYNC);}

void CSfxCfg::OnBnClickedPlayStart()
{

	PlaySound(m_strStart, NULL, SND_FILENAME|SND_ASYNC);}

void CSfxCfg::OnBnClickedPlayError()
{

	PlaySound(m_strError, NULL, SND_FILENAME|SND_ASYNC);
}
