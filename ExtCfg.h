#pragma once

#include "CfgDlg.h"

// CExtCfg dialog

class CExtCfg : public CCfgDlg
{
	DECLARE_DYNAMIC(CExtCfg)

public:
	CExtCfg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExtCfg();

// Dialog Data
	enum { IDD = IDD_EXTENSIONS };
	CListCtrl	m_lcPlugins;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedConfigure();
	afx_msg void OnBnClickedRescan();
	afx_msg void OnBnClickedUninstall();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConfigure(NMHDR *pNMHDR, LRESULT *pResult);
};
