// InputRequest.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "InputRequest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputRequest dialog
extern UINT UWM_INPUT;

CInputRequest::CInputRequest(CWnd* pParent /*=NULL*/)
	: CDialog(CInputRequest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputRequest)
	m_strInput = _T("");
	m_bConvert = FALSE;
	//}}AFX_DATA_INIT
}


void CInputRequest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputRequest)
	DDX_Control(pDX, IDC_INPUT, m_eInput);
	DDX_Text(pDX, IDC_INPUT, m_strInput);
	DDV_MaxChars(pDX, m_strInput, 400);
	DDX_Check(pDX, IDC_CONVERT, m_bConvert);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputRequest, CDialog)
	//{{AFX_MSG_MAP(CInputRequest)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UWM_INPUT, OnEnter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputRequest message handlers

void CInputRequest::OnOK() 
{

	CDialog::OnOK();

	CString strTmp;
	int i = 0;
	
	switch(m_dwMode){

	case 0: // l33t
	
		m_strInput.Replace("a", "4");
		m_strInput.Replace("A", "4");
		m_strInput.Replace("c", "(");
		m_strInput.Replace("C", "(");
		m_strInput.Replace("d", "Ð");
		m_strInput.Replace("D", "Ð");
		m_strInput.Replace("e", "3");
		m_strInput.Replace("E", "3");
		m_strInput.Replace("f", "ƒ");
		m_strInput.Replace("F", "ƒ");
		m_strInput.Replace("g", "9");
		m_strInput.Replace("G", "9");
		m_strInput.Replace("i", "1");
		m_strInput.Replace("I", "1");
		m_strInput.Replace("k", "|{");
		m_strInput.Replace("K", "|{");
		m_strInput.Replace("l", "£");
		m_strInput.Replace("L", "£");
		m_strInput.Replace("o", "0");
		m_strInput.Replace("O", "0");
		m_strInput.Replace("s", "$");
		m_strInput.Replace("S", "$");
		m_strInput.Replace("t", "7");
		m_strInput.Replace("T", "7");
		m_strInput.Replace("u", "µ");
		m_strInput.Replace("U", "µ");
		m_strInput.Replace("y", "¥");
		m_strInput.Replace("Y", "¥");
		m_strInput.Replace("z", "2");
		m_strInput.Replace("Z", "2");
		break;
	case 1: // bubbles;
		m_strInput.MakeUpper();
		for(i = 0; i < m_strInput.GetLength(); i++){

			if(m_strInput[i] != ' ')
				strTmp += "(" + m_strInput.Mid(i, 1) + ")";
			else
				strTmp += " ";
		}
		m_strInput = strTmp;
		break;
	case 2: // box;
		m_strInput.MakeUpper();
		for(i = 0; i < m_strInput.GetLength(); i++){

			if(m_strInput[i] != ' ')
				strTmp += "[" + m_strInput.Mid(i, 1) + "]";
			else
				strTmp += " ";
		}
		m_strInput = strTmp;
		break;
	case 3: // 3D buttons msg;
		m_strInput.MakeLower();
		strTmp = "-=[";
		for(i = 0; i < m_strInput.GetLength(); i++){

			if(m_strInput[i] != ' ')
				strTmp += "(" + m_strInput.Mid(i, 1) + ")";
			else
				strTmp += " ";
		}
		m_strInput = strTmp + "]=-";
		break;
	case 4: // 3D buttons action;
		m_strInput.MakeLower();
		strTmp = "/me -=[";
		for(i = 0; i < m_strInput.GetLength(); i++){

			if(m_strInput[i] != ' ')
				strTmp += "(" + m_strInput.Mid(i, 1) + ")";
			else
				strTmp += " ";
		}
		m_strInput = strTmp + "]=-";
		break;
	case 5: // ASCII designer
		break;
	default:
		ASSERT(FALSE);
	}

	if(m_bConvert){

		m_strInput.Replace("\n", "#\\r\\n#");
	}
}

void CInputRequest::SetMode(DWORD dwMode, CFont* pFont)
{

	m_dwMode = dwMode;
	m_pFont = pFont;
}

BOOL CInputRequest::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT(m_pFont);
	
	m_eInput.SetFont(m_pFont);
	m_eInput.SetExtended();

	if(m_dwMode == 5){

		GetDlgItem(IDC_CONVERT)->EnableWindow(TRUE);
		m_bConvert = TRUE;
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CInputRequest::OnEnter(WPARAM w, LPARAM l)
{

	SendMessage(WM_COMMAND, IDOK, 0);
	return 1;
}
