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
#include "ColorCfg.h"
#include "Settings.h"
#include ".\colorcfg.h"
#include "Util.h"
extern CSettings g_sSettings;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorCfg dialog


CColorCfg::CColorCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CColorCfg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorCfg)
	m_strActionEnd = _T("");
	m_strActionFront = _T("");
	m_nEncoding = -1;
	m_nSize = 12;
	m_strMsgEnd = _T("");
	m_strMsgFront = _T("");
	m_strFont = _T("");
	m_bBold = FALSE;
	m_bItalic = FALSE;
	m_bLine = FALSE;
	m_bImage = FALSE;
	m_strImage = _T("");
	m_bFocus = FALSE;
	//}}AFX_DATA_INIT
}


void CColorCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FOCUS, m_stFocus);
	DDX_Control(pDX, IDC_STATIC_PENDING, m_stPend);
	DDX_Control(pDX, IDC_STATIC_SUCCESS, m_stOK);
	DDX_Control(pDX, IDC_STATIC_ERROR, m_stErr);
	DDX_Control(pDX, IDC_STATIC_LINE, m_stLine);
	DDX_Control(pDX, IDC_STATIC_FILES, m_stFiles);
	DDX_Control(pDX, IDC_STATIC_NODEIP, m_stIP);
	DDX_Control(pDX, IDC_STATIC_NODEPORT, m_stPort);
	DDX_Control(pDX, IDC_STATIC_BG, m_stBg);
	DDX_Control(pDX, IDC_STATIC_COLOR_MSG_PREV, m_stMsg);
	DDX_Control(pDX, IDC_STATIC_COLOR_ENC_MSG_PREV, m_stMsgEnc);
	DDX_Control(pDX, IDC_STATIC_JOIN, m_stJoin);
	DDX_Control(pDX, IDC_STATIC_MOTD, m_stMotd);
	DDX_Control(pDX, IDC_STATIC_PART, m_stPart);
	DDX_Control(pDX, IDC_STATIC_TOPIC, m_stTopic);
	DDX_Control(pDX, IDC_STATIC_DOCHI, m_stDocHi);
	DDX_Control(pDX, IDC_STATIC_COLOR_ENG_ACTION_PREV, m_stActEnc);
	DDX_Control(pDX, IDC_STATIC_COLOR_ACTION_PREV, m_stAction);
	DDX_Control(pDX, IDC_FONT, m_cbFont);
	DDX_Control(pDX, IDC_ENCODING, m_cbEncoding);
	DDX_Text(pDX, IDC_ACTION_END, m_strActionEnd);
	DDX_Text(pDX, IDC_ACTION_FRONT, m_strActionFront);
	DDX_CBIndex(pDX, IDC_ENCODING, m_nEncoding);
	DDX_Text(pDX, IDC_FONTSIZE, m_nSize);
	DDX_Text(pDX, IDC_MSG_END, m_strMsgEnd);
	DDX_Text(pDX, IDC_MSG_FRONT, m_strMsgFront);
	DDX_Text(pDX, IDC_FONT, m_strFont);
	DDX_Check(pDX, IDC_BOLD, m_bBold);
	DDX_Check(pDX, IDC_ITALIC, m_bItalic);
	DDX_Check(pDX, IDC_UNDERLINE, m_bLine);
	DDX_Check(pDX, IDC_USE_BGIMAGE, m_bImage);
	DDX_Text(pDX, IDC_STATIC_IMAGEPATH, m_strImage);
	DDX_Check(pDX, IDC_FOCUSCLR, m_bFocus);
	DDX_Control(pDX, IDC_STATIC_COLOR_NAME_PREV, m_stName);
	DDX_Control(pDX, IDC_STATIC_TIME, m_stTime);
	DDX_Control(pDX, IDC_STATIC_HILITE, m_stHiLite);
}


BEGIN_MESSAGE_MAP(CColorCfg, CCfgDlg)
	ON_BN_CLICKED(IDC_ENC_MSG_COLOR, OnEncMsgColor)
	ON_BN_CLICKED(IDC_COLOR_MESSAGE, OnColorMessage)
	ON_BN_CLICKED(IDC_ENC_ACTION_COLOR, OnEncActionColor)
	ON_BN_CLICKED(IDC_COLOR_ACTION, OnColorAction)
	ON_BN_CLICKED(IDC_TOPIC, OnTopic)
	ON_BN_CLICKED(IDC_MOTD, OnMotd)
	ON_BN_CLICKED(IDC_JOIN, OnJoin)
	ON_BN_CLICKED(IDC_PART, OnPart)
	ON_BN_CLICKED(IDC_BACKG, OnBackg)
	ON_BN_CLICKED(IDC_LINE, OnLine)
	ON_BN_CLICKED(IDC_FILES, OnFiles)
	ON_BN_CLICKED(IDC_NODEIP, OnNodeip)
	ON_BN_CLICKED(IDC_PORT, OnPort)
	ON_BN_CLICKED(IDC_PENDING, OnPending)
	ON_BN_CLICKED(IDC_SUCCESS, OnSuccess)
	ON_BN_CLICKED(IDC_ERROR, OnError)
	ON_BN_CLICKED(IDC_SELECT_IMAGE, OnSelectImage)
	ON_BN_CLICKED(IDC_COLORFOCUS, OnColorfocus)
	ON_BN_CLICKED(IDC_COLOR_NAME, OnBnClickedColorName)
	ON_BN_CLICKED(IDC_TIME, OnBnClickedTime)
	ON_BN_CLICKED(IDC_HILITE, OnBnClickedHilite)
	ON_BN_CLICKED(IDC_EDITHILITE, OnBnClickedEdithilite)
	ON_BN_CLICKED(IDC_DOCHI, OnBnClickedDochi)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCfg message handlers

BOOL CColorCfg::OnInitDialog() 
{

	CCfgDlg::OnInitDialog();
	

	CClientDC dc(this);		
	EnumFonts(dc, 0, (FONTENUMPROC)EnumFontsProc, (LPARAM)&m_cbFont);

	m_strFont		= g_sSettings.GetFont();
	
	int nFont = m_cbFont.FindStringExact(-1, m_strFont);
	if(nFont != CB_ERR){

		m_cbFont.SetCurSel(nFont);
	}
	
	m_nSize = g_sSettings.GetFontSize();
	m_nEncoding = g_sSettings.GetCharSet(TRUE);

	m_stJoin.SetBkColor(g_sSettings.GetRGBJoin());
	m_stMotd.SetBkColor(g_sSettings.GetRGBMotd());
	m_stMsg.SetBkColor(g_sSettings.GetRGBNormalMsg());
	m_stName.SetBkColor(g_sSettings.GetRGBNormalName());
	m_stMsgEnc.SetBkColor(g_sSettings.GetRGBBrMessage());
	m_stPart.SetBkColor(g_sSettings.GetRGBPart());
    m_stAction.SetBkColor(g_sSettings.GetRGBActionMsg());
	m_stActEnc.SetBkColor(g_sSettings.GetRGBBrAction());
	m_stTopic.SetBkColor(g_sSettings.GetRGBTopic());
	m_stBg.SetBkColor(g_sSettings.GetRGBBg());
	m_stFiles.SetBkColor(g_sSettings.GetRGBFiles());
	m_stLine.SetBkColor(g_sSettings.GetRGBLine());
	m_stIP.SetBkColor(g_sSettings.GetRGBIP());
	m_stPort.SetBkColor(g_sSettings.GetRGBPort());
	m_stOK.SetBkColor(g_sSettings.GetRGBOK());
	m_stPend.SetBkColor(g_sSettings.GetRGBPend());
	m_stErr.SetBkColor(g_sSettings.GetRGBErr());
	m_stFocus.SetBkColor(g_sSettings.GetRGBFocus());
	m_stTime.SetBkColor(g_sSettings.GetRGBTime());
	m_stHiLite.SetBkColor(g_sSettings.GetRGBHiLite());
	m_stDocHi.SetBkColor(g_sSettings.GetRGBDocHiLite());

	m_strMsgFront = g_sSettings.GetBrMsgFront();
	m_strMsgEnd   = g_sSettings.GetBrMsgEnd();
	m_strActionFront = g_sSettings.GetBrActionFront();
	m_strActionEnd = g_sSettings.GetBrActionEnd();
	m_bBold = g_sSettings.GetMessageBold();
	m_bItalic = g_sSettings.GetMessageItalic();
	m_bLine = g_sSettings.GetMessageLine();
	m_bImage = g_sSettings.GetUseImage();
	m_strImage = g_sSettings.GetImage();
	m_bFocus = g_sSettings.GetFocus();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorCfg::Apply()
{

	UpdateData(TRUE);
	
	g_sSettings.SetRGBJoin(m_stJoin.GetBkColor());
	g_sSettings.SetRGBMotd(m_stMotd.GetBkColor());
	g_sSettings.SetRGBNormalName(m_stName.GetBkColor());
	g_sSettings.SetRGBNormalMsg(m_stMsg.GetBkColor());
	g_sSettings.SetRGBBrMessage(m_stMsgEnc.GetBkColor());
	g_sSettings.SetRGBPart(m_stPart.GetBkColor());
    g_sSettings.SetRGBActionMsg(m_stAction.GetBkColor());
	g_sSettings.SetRGBBrAction(m_stActEnc.GetBkColor());
	g_sSettings.SetRGBTopic(m_stTopic.GetBkColor());
	g_sSettings.SetRGBBg(m_stBg.GetBkColor());
	g_sSettings.SetRGBFiles(m_stFiles.GetBkColor());
	g_sSettings.SetRGBLine(m_stLine.GetBkColor());
	g_sSettings.SetRGBIP(m_stIP.GetBkColor());
	g_sSettings.SetRGBPort(m_stPort.GetBkColor());
	g_sSettings.SetRGBOK(m_stOK.GetBkColor());
	g_sSettings.SetRGBPend(m_stPend.GetBkColor());
	g_sSettings.SetRGBErr(m_stErr.GetBkColor());
	g_sSettings.SetRGBFocus(m_stFocus.GetBkColor());
	g_sSettings.SetRGBTime(m_stTime.GetBkColor());
	g_sSettings.SetRGBHiLite(m_stHiLite.GetBkColor());
	g_sSettings.SetRGBDocHiLite(m_stDocHi.GetBkColor());

	g_sSettings.SetBrMsgFront(m_strMsgFront);
	g_sSettings.SetBrMsgEnd(m_strMsgEnd);
	g_sSettings.SetBrActionFront(m_strActionFront);
	g_sSettings.SetBrActionEnd(m_strActionEnd);
	g_sSettings.SetCharSet(m_nEncoding);
	g_sSettings.SetFontSize(m_nSize);
	g_sSettings.SetFont(m_strFont);
	g_sSettings.SetMessageBold(m_bBold);
	g_sSettings.SetMessageItalic(m_bItalic);
	g_sSettings.SetMessageLine(m_bLine);
	g_sSettings.SetImage(m_strImage);
	g_sSettings.SetUseImage(m_bImage);
	g_sSettings.SetFocus(m_bFocus);

	GetApp()->ApplyPic();
}

void CColorCfg::OnBnClickedColorName()
{

	CColorDialog cDlg(m_stName.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stName.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnEncMsgColor() 
{
	
	CColorDialog cDlg(m_stMsgEnc.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stMsgEnc.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnColorMessage() 
{
	
	CColorDialog cDlg(m_stMsg.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stMsg.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnEncActionColor() 
{
	
	CColorDialog cDlg(m_stActEnc.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stActEnc.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnColorAction() 
{
	
	CColorDialog cDlg(m_stAction.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stAction.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnTopic() 
{
	
	CColorDialog cDlg(m_stTopic.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stTopic.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnMotd() 
{
	
	CColorDialog cDlg(m_stMotd.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stMotd.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnJoin() 
{
	
	CColorDialog cDlg(m_stJoin.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stJoin.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnPart() 
{
	
	CColorDialog cDlg(m_stPart.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stPart.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnBackg() 
{
	
	CColorDialog cDlg(m_stBg.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stBg.SetBkColor(cDlg.GetColor());
	}
}

BOOL CALLBACK EnumFontsProc(LPLOGFONT lpFont, LPTEXTMETRIC lpMetric, DWORD dwType, LPARAM lpData)
{


	CComboBox *pThis = (CComboBox*)(lpData);		
	int index = pThis->AddString(lpFont->lfFaceName);
	
	ASSERT(index!=-1);
	
	return TRUE;
}

void CColorCfg::OnLine() 
{
	
	CColorDialog cDlg(m_stLine.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stLine.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnFiles() 
{
	
	CColorDialog cDlg(m_stFiles.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stFiles.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnNodeip() 
{
	
	CColorDialog cDlg(m_stIP.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stIP.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnPort() 
{
	
	CColorDialog cDlg(m_stPort.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stPort.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnPending() 
{
	
	CColorDialog cDlg(m_stPend.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stPend.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnSuccess() 
{
	
	CColorDialog cDlg(m_stOK.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stOK.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnError() 
{
	
	CColorDialog cDlg(m_stErr.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stErr.SetBkColor(cDlg.GetColor());
	}
}


void CColorCfg::OnColorfocus() 
{

	CColorDialog cDlg(m_stFocus.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stFocus.SetBkColor(cDlg.GetColor());
	}
}


void CColorCfg::OnBnClickedTime()
{

	CColorDialog cDlg(m_stTime.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stTime.SetBkColor(cDlg.GetColor());
	}
}


void CColorCfg::OnBnClickedHilite()
{

	CColorDialog cDlg(m_stHiLite.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stHiLite.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnBnClickedDochi()
{

	CColorDialog cDlg(m_stDocHi.GetBkColor());
	
	if(cDlg.DoModal() == IDOK){

		m_stDocHi.SetBkColor(cDlg.GetColor());
	}
}

void CColorCfg::OnSelectImage() 
{

	static char BASED_CODE szFilter[] = "Image Files (*.bmp;*.jpg;*.gif;*.png;*.tiff)|*.bmp;*.jpg;*.gif;*.png;*.tiff|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, ".bmp", m_strImage, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);

	if(dlg.DoModal() == IDOK){

		if(dlg.GetPathName() != m_strImage){

			m_strImage = dlg.GetPathName();
			UpdateData(FALSE);
			//AfxMessageBox("You need to restart RoboMX to take the change into effect.", MB_ICONINFORMATION);
		}
	}
}

void CColorCfg::OnBnClickedEdithilite()
{

	char buffer[MAX_PATH+1];
	GetWindowsDirectory((LPSTR)&buffer, MAX_PATH+1);

	CString strExec, strParam;
	strExec.Format("%s\\notepad.exe", buffer);
	strParam.Format("%s\\hilite.ini", g_sSettings.GetWorkingDir());
	Util::ShellExecuteWait(strExec, strParam);
	g_sSettings.LoadHiLite();
	AfxMessageBox("Hilight list was loaded", MB_ICONINFORMATION);
}

