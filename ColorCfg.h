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

#if !defined(AFX_COLORCFG_H__83E03A95_C63A_4FC9_9938_C2CE47CCD964__INCLUDED_)
#define AFX_COLORCFG_H__83E03A95_C63A_4FC9_9938_C2CE47CCD964__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorCfg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorCfg dialog

#include "CfgDlg.h"
#include "ColorStatic.h"

class CColorCfg : public CCfgDlg
{
// Construction
public:
	CColorCfg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorCfg)
	enum { IDD = IDD_COLORS };
	CColorStatic	m_stFocus;
	CColorStatic	m_stPend;
	CColorStatic	m_stOK;
	CColorStatic	m_stErr;
	CColorStatic	m_stLine;
	CColorStatic	m_stFiles;
	CColorStatic	m_stIP;
	CColorStatic	m_stPort;
	CColorStatic	m_stBg;
	CColorStatic	m_stMsg;
	CColorStatic	m_stMsgEnc;
	CColorStatic	m_stJoin;
	CColorStatic	m_stMotd;
	CColorStatic	m_stPart;
	CColorStatic	m_stTopic;
	CColorStatic	m_stActEnc;
	CColorStatic	m_stAction;
	CColorStatic	m_stDocHi;
	CColorStatic	m_stOpMsg;
	CComboBox		m_cbFont;
	CComboBox		m_cbEncoding;
	CString	m_strActionEnd;
	CString	m_strActionFront;
	int		m_nEncoding;
	int		m_nSize;
	CString	m_strMsgEnd;
	CString	m_strMsgFront;
	CString m_strFont;
	BOOL	m_bBold;
	BOOL	m_bItalic;
	BOOL	m_bLine;
	BOOL	m_bImage;
	CString	m_strImage;
	BOOL	m_bFocus;
	//}}AFX_DATA
	void Apply();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	friend BOOL CALLBACK EnumFontsProc(LPLOGFONT lpFont, LPTEXTMETRIC lpMetric, DWORD dwType, LPARAM lpData);

	// Generated message map functions
	//{{AFX_MSG(CColorCfg)
	afx_msg void OnEncMsgColor();
	afx_msg void OnColorMessage();
	afx_msg void OnEncActionColor();
	afx_msg void OnColorAction();
	afx_msg void OnTopic();
	afx_msg void OnMotd();
	afx_msg void OnJoin();
	afx_msg void OnPart();
	afx_msg void OnBackg();
	virtual BOOL OnInitDialog();
	afx_msg void OnLine();
	afx_msg void OnFiles();
	afx_msg void OnNodeip();
	afx_msg void OnPort();
	afx_msg void OnPending();
	afx_msg void OnSuccess();
	afx_msg void OnError();
	afx_msg void OnSelectImage();
	afx_msg void OnColorfocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CColorStatic m_stName;
	afx_msg void OnBnClickedColorName();
	CColorStatic m_stTime;
	afx_msg void OnBnClickedTime();
	afx_msg void OnBnClickedHilite();
	CColorStatic m_stHiLite;
	afx_msg void OnBnClickedEdithilite();
	afx_msg void OnBnClickedDochi();
	afx_msg void OnBnClickedOpmsg();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCFG_H__83E03A95_C63A_4FC9_9938_C2CE47CCD964__INCLUDED_)
