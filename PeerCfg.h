#pragma once

#include "CfgDlg.h"
#include "afxcmn.h"

// CPeerCfg dialog

class CPeerCfg : public CCfgDlg
{
	//DECLARE_DYNAMIC(CPeerCfg)

public:
	CPeerCfg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPeerCfg();

// Dialog Data
	enum { IDD = IDD_PAGERCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);

	DECLARE_MESSAGE_MAP()
public:
	virtual void Apply(void);
};
