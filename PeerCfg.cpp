// PeerCfg.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "PeerCfg.h"
#include "Settings.h"
#include ".\peercfg.h"

// CPeerCfg dialog
extern CSettings g_sSettings;

//IMPLEMENT_DYNAMIC(CPeerCfg, CCfgDlg)
CPeerCfg::CPeerCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CPeerCfg::IDD, pParent)
{
}

CPeerCfg::~CPeerCfg()
{
}

void CPeerCfg::DoDataExchange(CDataExchange* pDX)
{
	CCfgDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPeerCfg, CCfgDlg)
END_MESSAGE_MAP()


// CPeerCfg message handlers

BOOL CPeerCfg::OnInitDialog(void)
{

	CCfgDlg::OnInitDialog();

	UpdateData(FALSE);
	return TRUE;
}

void CPeerCfg::Apply(void)
{

	UpdateData(TRUE);

}
