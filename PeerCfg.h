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
	enum { IDD = IDD_SERVERCFG };
	CString m_strTopic;
	CString m_strMotd;
	CString m_strKeyMsg;
	CString m_strGodName;
	DWORD   m_dwLimit;
	BOOL    m_bModerated;
	BOOL    m_bMultiIPOk;
	BOOL    m_bLocalIsOp;
	BOOL	m_bBlockNushi;
	BOOL	m_bGodVisible;
	DWORD	m_dwGodFiles;
	int		m_nLine;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);

	DECLARE_MESSAGE_MAP()
public:
	virtual void Apply(void);
};
