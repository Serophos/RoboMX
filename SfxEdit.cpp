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
#include "SfxEdit.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSfxEdit dialog


CSfxEdit::CSfxEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CSfxEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSfxEdit)
	m_nEvent = -1;
	m_strReaction = _T("");
	m_strTrigger = _T("");
	//}}AFX_DATA_INIT
}


void CSfxEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSfxEdit)
	DDX_Control(pDX, IDC_TRIGGER, m_wndTrigger);
	DDX_Control(pDX, IDC_REACTION, m_wndRaction);
	DDX_Control(pDX, IDC_BROWSE_SOUND, m_wndBrowseBtn);
	DDX_Text(pDX, IDC_REACTION, m_strReaction);
	DDX_Text(pDX, IDC_TRIGGER, m_strTrigger);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSfxEdit, CDialog)
	//{{AFX_MSG_MAP(CSfxEdit)
	ON_BN_CLICKED(IDC_BROWSE_SOUND, OnBrowseSound)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSfxEdit message handlers


void CSfxEdit::OnBrowseSound() 
{
	
	UpdateData(TRUE);

	static char BASED_CODE szFilter[] = "Wave Audio Files (*.wav)|*.wav|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".wav", NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
	
	if(dlg.DoModal() == IDOK){

		m_strReaction = dlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CSfxEdit::Init(int nEventType, CString strTrigger, CString strReaction)
{

	m_nEvent = nEventType;
	m_strTrigger = strTrigger;
	m_strReaction = strReaction;
}

BOOL CSfxEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//m_tooltip.Create(this);
	//m_tooltip.SetBehaviour(PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_TRACKING_MOUSE | PPTOOLTIP_CLOSE_LEAVEWND, -1);

	//m_tooltip.AddTool(GetDlgItem(IDC_TRIGGER), "Word(s) that trigger the sound event.", IDI_TIP);
	//m_tooltip.AddTool(GetDlgItem(IDC_REACTION), "Filename of sound to be played (must be a valid Wave Audio file).", IDI_TIP);

	UpdateData(FALSE);
	
	m_wndTrigger.SetFocus();
	m_wndTrigger.SetSel(0, -1);
	
	return TRUE;
}

void CSfxEdit::GetValues(int &nEvent, CString &strTrigger, CString &strReaction)
{

	nEvent = m_nEvent;
	strTrigger = m_strTrigger;
	strReaction = m_strReaction;
}

void CSfxEdit::OnOK() 
{

	if(UpdateData(TRUE)){
		
		CDialog::OnOK();
	}
}

void CSfxEdit::OnPlay() 
{

	UpdateData(TRUE);

	PlaySound(m_strReaction, NULL, SND_FILENAME|SND_ASYNC);	
}
