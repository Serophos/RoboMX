// FunCfg.cpp : implementation file
//

#include "stdafx.h"
#include "Metis3.h"
#include "FunCfg.h"
#include "Ini.h"
#include "Settings.h"
#include ".\funcfg.h"
#include "MainFrm.h"
#include "EmoticonManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFunCfg dialog

extern CSettings g_sSettings;
extern CEmoticonManager g_eEmoticons;

CFunCfg::CFunCfg(CWnd* pParent /*=NULL*/)
	: CCfgDlg(CFunCfg::IDD, pParent)
	, m_strActivation(_T(""))
	, m_strPath(_T(""))
{
	//{{AFX_DATA_INIT(CFunCfg)
	m_strVendor = _T("");
	m_strSuff = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_nAct = 0;
	m_hPreview = NULL;
}


void CFunCfg::DoDataExchange(CDataExchange* pDX)
{
	CCfgDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFunCfg)
	DDX_Text(pDX, IDC_VENDOR_ADD, m_strVendor);
	DDX_Text(pDX, IDC_SUFFIX_ADD, m_strSuff);
	DDX_Text(pDX, IDC_NAME_ADD, m_strName);
	DDX_Text(pDX, IDC_ACTTEXT, m_strActivation);
	DDX_Text(pDX, IDC_BMP, m_strPath);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_VENDORS, m_lcVendors);
	DDX_Control(pDX, IDC_NAMES, m_lcNames);
	DDX_Control(pDX, IDC_SUFFIXES, m_lcSuff);
	DDX_Control(pDX, IDC_EMOTICONLIST, m_lcEmo);
	DDX_Control(pDX, IDC_PREVIEW, m_stPreview);
}


BEGIN_MESSAGE_MAP(CFunCfg, CCfgDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_VENDOR_ADD, OnBtnVendorAdd)
	ON_BN_CLICKED(IDC_BTN_NAME_ADD, OnBtnNameAdd)
	ON_BN_CLICKED(IDC_BTN_SUFFIX_ADD, OnBtnSuffixAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_VENDORS, OnNMDblclkVendors)
	ON_NOTIFY(NM_DBLCLK, IDC_NAMES, OnNMDblclkNames)
	ON_NOTIFY(NM_DBLCLK, IDC_SUFFIXES, OnNMDblclkSuffixes)
	ON_BN_CLICKED(IDC_SELECTBMP, OnBnClickedSelectbmp)
	ON_BN_CLICKED(IDC_ADD_EMOTICON, OnBnClickedAddEmoticon)
	ON_BN_CLICKED(IDC_DELETE_EMOTICON, OnBnClickedDeleteEmoticon)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_EMOTICONLIST, OnLvnItemchangedEmoticonlist)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFunCfg message handlers


void CFunCfg::OnDestroy()
{

	CCfgDlg::OnDestroy();

	if(m_hPreview != NULL){

		DeleteObject(m_hPreview);
	}
}
BOOL CFunCfg::OnInitDialog() 
{
	CCfgDlg::OnInitDialog();
	m_lcSuff.InsertColumn(0, "Items", LVCFMT_LEFT, 250);
	m_lcNames.InsertColumn(0, "Items", LVCFMT_LEFT, 250);
	m_lcVendors.InsertColumn(0, "Items", LVCFMT_LEFT, 250);
	ListView_SetExtendedListViewStyle(m_lcSuff.m_hWnd, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(m_lcNames.m_hWnd, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(m_lcVendors.m_hWnd, LVS_EX_FULLROWSELECT);

	CString strTitle;
	strTitle.LoadString(IDS_EMO_ACT);
	m_lcEmo.InsertColumn(0, strTitle, LVCFMT_LEFT, 60);
	strTitle.LoadString(IDS_EMO_FILE);
	m_lcEmo.InsertColumn(1, strTitle, LVCFMT_LEFT, 350);
	ListView_SetExtendedListViewStyle(m_lcEmo.m_hWnd, LVS_EX_FULLROWSELECT);

	Load();

	m_lcSuff.SetColumnWidth(0, LVSCW_AUTOSIZE);
	m_lcNames.SetColumnWidth(0, LVSCW_AUTOSIZE);
	m_lcVendors.SetColumnWidth(0, LVSCW_AUTOSIZE);

	int nIn = 0;
	for(int i = 0; i < g_eEmoticons.m_aEmoticons.GetSize(); i++){

		nIn = m_lcEmo.InsertItem(m_lcEmo.GetItemCount(), g_eEmoticons.m_aEmoticons[i]->szActivationText);
		m_lcEmo.SetItemText(nIn, 1, g_eEmoticons.m_aEmoticons[i]->szFileName);
	}
	return TRUE;
}


void CFunCfg::Apply()
{

	if(!m_lcVendors.GetItemCount() || !m_lcNames.GetItemCount() || !m_lcSuff.GetItemCount()){

		AfxMessageBox(IDS_ERROR_NAMES_EMPTY, MB_ICONWARNING);
	}
	Save();
	WriteEmoticons();
	g_eEmoticons.Free();
	g_eEmoticons.Load();
}

void CFunCfg::OnBtnVendorAdd() 
{

	UpdateData(TRUE);
	
	if(!m_strVendor.IsEmpty()){

		m_lcVendors.InsertItem(0, m_strVendor, 0);
		m_strVendor.Empty();
		UpdateData(FALSE);
	}
	m_lcVendors.SetColumnWidth(0, LVSCW_AUTOSIZE);
}

void CFunCfg::OnBtnNameAdd() 
{
	
	UpdateData(TRUE);
	
	if(!m_strName.IsEmpty()){

		m_lcNames.InsertItem(0, m_strName, 0);
		m_strName.Empty();
		UpdateData(FALSE);
	}
	m_lcNames.SetColumnWidth(0, LVSCW_AUTOSIZE);
}

void CFunCfg::OnBtnSuffixAdd() 
{
	
	UpdateData(TRUE);
	
	if(!m_strSuff.IsEmpty()){

		m_lcSuff.InsertItem(0, m_strSuff, 0);
		m_strSuff.Empty();
		UpdateData(FALSE);
	}
	m_lcSuff.SetColumnWidth(0, LVSCW_AUTOSIZE);
}

void CFunCfg::Load()
{

	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir() + "\\RoboMX.ini");

	int i = 0, nNum = 0;
	
	CString strNum;

	nNum = ini.GetValue("Title", "NumVendors", 1);
	
	for(i = 0; i < nNum; i++){

		strNum.Format("Vendor_%02d", i);
		m_lcVendors.InsertItem(0, ini.GetValue("Title", strNum, "BendSoft"));
	}

	nNum = ini.GetValue("Title", "NumNames", 1);
	
	for(i = 0; i < nNum; i++){

		strNum.Format("Name_%02d", i);
		m_lcNames.InsertItem(0, ini.GetValue("Title", strNum, "Robo"));
	}

	nNum = ini.GetValue("Title", "NumSuffix", 1);
	
	for(i = 0; i < nNum; i++){

		strNum.Format("Suffix_%02d", i);
		m_lcSuff.InsertItem(0, ini.GetValue("Title", strNum, "MX"));
	}
}

void CFunCfg::Save()
{

	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir() + "\\RoboMX.ini");

	int i = 0, nNum = 0;
	
	CString strNum;

	nNum = m_lcVendors.GetItemCount();
	ini.SetValue("Title", "NumVendors", nNum);

	for(i = 0; i < nNum; i++){

		strNum.Format("Vendor_%02d", i);
		ini.SetValue("Title", strNum, m_lcVendors.GetItemText(i, 0));
	}

	nNum = m_lcNames.GetItemCount();
	ini.SetValue("Title", "NumNames", nNum);

	for(i = 0; i < nNum; i++){

		strNum.Format("Name_%02d", i);
		ini.SetValue("Title", strNum, m_lcNames.GetItemText(i, 0));
	}

	nNum = m_lcSuff.GetItemCount();
	ini.SetValue("Title", "NumSuffix", nNum);

	for(i = 0; i < nNum; i++){

		strNum.Format("Suffix_%02d", i);
		ini.SetValue("Title", strNum, m_lcSuff.GetItemText(i, 0));
	}
}


void CFunCfg::OnNMDblclkVendors(NMHDR *pNMHDR, LRESULT *pResult)
{

	int nSel = m_lcVendors.GetNextItem(-1, LVNI_SELECTED);
	if(nSel >= 0){

		m_lcVendors.DeleteItem(nSel);
	}
	*pResult = 0;
}

void CFunCfg::OnNMDblclkNames(NMHDR *pNMHDR, LRESULT *pResult)
{

	int nSel = m_lcNames.GetNextItem(-1, LVNI_SELECTED);
	if(nSel >= 0){

		m_lcNames.DeleteItem(nSel);
	}
	*pResult = 0;
}

void CFunCfg::OnNMDblclkSuffixes(NMHDR *pNMHDR, LRESULT *pResult)
{

	int nSel = m_lcSuff.GetNextItem(-1, LVNI_SELECTED);
	if(nSel >= 0){

		m_lcSuff.DeleteItem(nSel);
	}
	*pResult = 0;
}

void CFunCfg::OnBnClickedSelectbmp()
{

	UpdateData(TRUE);

	CString strFilter;
	strFilter.LoadString(IDS_BITMAP_FILTER);

	CFileDialog dlg(TRUE, ".bmp", m_strPath, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, strFilter, this);

	if(dlg.DoModal() == IDOK){

		if(dlg.GetPathName() != m_strPath){

			m_strPath = dlg.GetPathName();
			UpdateData(FALSE);
		}
	}
}

void CFunCfg::OnBnClickedAddEmoticon()
{

	UpdateData(TRUE);
	if(m_strPath.IsEmpty() || m_strActivation.IsEmpty()){

		AfxMessageBox(IDS_ERROR_MISSING_INPUT, MB_ICONINFORMATION);
		return;
	}
	int nPos = m_lcEmo.InsertItem(m_lcEmo.GetItemCount(), m_strActivation);
	m_lcEmo.SetItemText(nPos, 1, m_strPath);
	m_strActivation.Empty();
	m_strPath.Empty();
	UpdateData(FALSE);
}

void CFunCfg::OnBnClickedDeleteEmoticon()
{

	int nSel = m_lcEmo.GetSelectionMark();
	if(nSel >= 0){

		m_lcEmo.DeleteItem(nSel);
	}
	else{

		AfxMessageBox(IDS_ERROR_NO_SELECTION, MB_ICONSTOP);
	}
}

void CFunCfg::WriteEmoticons(void)
{

	CString strIniFile = g_sSettings.GetWorkingDir() + "\\emoticons.ini";
	CStdioFile ini;
	CString strBuffer;

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareExclusive);
		ini.SetLength(0);

		for(int i = 0; i < m_lcEmo.GetItemCount(); i++){

			ini.WriteString(m_lcEmo.GetItemText(i, 0) + "\n");
			ini.WriteString(m_lcEmo.GetItemText(i, 1) + "\n");
		}
		ini.Flush();
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);

	}END_CATCH;
}

void CFunCfg::OnLvnItemchangedEmoticonlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int n = m_lcEmo.GetNextItem(-1, LVNI_SELECTED);
	
	if(n >= 0){

        CString strIcon = m_lcEmo.GetItemText(n, 1);

		if(m_hPreview != NULL){

			DeleteObject(m_hPreview);
		}
		m_hPreview = (HBITMAP)::LoadImage(NULL, strIcon, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADTRANSPARENT|LR_LOADFROMFILE);
		m_stPreview.SetBitmap(m_hPreview);
	}
	*pResult = 0;
}
