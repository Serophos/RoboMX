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
	
	m_strTopic		= "";
	m_strMotd		= "";
	m_strKeyMsg		= "";
	m_strGodName	= "";
	m_dwLimit		= 40;
	m_bModerated	= FALSE;
	m_bMultiIPOk	= TRUE;
	m_bLocalIsOp	= TRUE;
	m_bBlockNushi	= FALSE;
	m_bGodVisible	= FALSE;
	m_dwGodFiles	= 0;
	m_nLine			= 0;
}

CPeerCfg::~CPeerCfg()
{
}

void CPeerCfg::DoDataExchange(CDataExchange* pDX)
{
	CCfgDlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_DEFTOPIC, m_strTopic);
	DDX_Text(pDX, IDC_DEFMOTD, m_strMotd);
	DDX_Text(pDX, IDC_KEYMSG, m_strKeyMsg);
	DDX_Text(pDX, IDC_GOD_NAME, m_strGodName);
	DDX_Text(pDX, IDC_LIMIT, m_dwLimit);
	DDX_Check(pDX, IDC_MODERATED, m_bModerated);
	DDX_Check(pDX, IDC_SINGLEUSER_PER_IP, m_bMultiIPOk);
	DDX_Check(pDX, IDC_AUTO_OP_LOCALHOST, m_bLocalIsOp);
	DDX_Check(pDX, IDC_BLOCK_MX331, m_bBlockNushi);
	DDX_Check(pDX, IDC_VISIBLE, m_bGodVisible);
	DDX_Text(pDX, IDC_GOD_FILES, m_dwGodFiles);
	DDX_CBIndex(pDX, IDC_GOD_LINE, m_nLine);
	DDV_MaxChars(pDX, m_strGodName, 44);
	DDV_MinMaxDWord(pDX, m_dwGodFiles, 0, 65535);
}


BEGIN_MESSAGE_MAP(CPeerCfg, CCfgDlg)
END_MESSAGE_MAP()


// CPeerCfg message handlers

BOOL CPeerCfg::OnInitDialog(void)
{

	CCfgDlg::OnInitDialog();


	m_strTopic		= g_sSettings.GetTopic();
	m_strMotd		= g_sSettings.GetMotd();
	m_strKeyMsg		= g_sSettings.GetKeyMsg();
	m_strGodName	= g_sSettings.GetGodName();
	m_dwLimit		= g_sSettings.GetLimit();
	m_bModerated	= g_sSettings.GetModerated();
	m_bMultiIPOk	= g_sSettings.GetMultiIPOk();
	m_bLocalIsOp	= g_sSettings.GetLocalIsOp();
	m_bBlockNushi	= g_sSettings.GetBlockNushi();
	m_bGodVisible	= g_sSettings.GetGodVisible();
	m_dwGodFiles	= g_sSettings.GetGodFiles();
	m_nLine			= g_sSettings.GetGodLine();

	UpdateData(FALSE);
	return TRUE;
}

void CPeerCfg::Apply(void)
{
 
	UpdateData(TRUE);

	g_sSettings.SetTopic(m_strTopic);
	g_sSettings.SetMotd(m_strMotd);
	g_sSettings.SetKeyMsg(m_strKeyMsg);
	g_sSettings.SetGodName(m_strGodName);
	g_sSettings.SetLimit(m_dwLimit);
	g_sSettings.SetModerated(m_bModerated);
	g_sSettings.SetMultiIPOk(m_bMultiIPOk);
	g_sSettings.SetLocalIsOp(m_bLocalIsOp);
	g_sSettings.SetBlockNushi(m_bBlockNushi);
	g_sSettings.SetGodVisible(m_bGodVisible);
	g_sSettings.SetGodFiles(m_dwGodFiles);
	g_sSettings.SetGodLine(m_nLine);
}
