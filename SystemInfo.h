/*
 * BendMX - Chat extension library for WinMX.
 * Copyright (C) 2003-2004 by Thees Ch. Winkler
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact: tcwinkler@users.sourceforge.net
 *
*/


#if !defined(AFX_SYSTEMINFO_H__3D874E31_833E_484B_ACA8_DA5467490D43__INCLUDED_)
#define AFX_SYSTEMINFO_H__3D874E31_833E_484B_ACA8_DA5467490D43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define W95_s "Windows 95"
#define W98_s "Windows 98"
#define WME_s "Windows ME"
#define NT4_s "Windows NT 4.0"
#define WKS_s " Workstation"
#define WXP_s "Windows XP"
#define W2K3_s "Windows 2003"
#define XHE_s " Home Edition"
#define XPR_s " Professional Edition"
#define W2K_s "Windows 2000"
#define PRO_s " Professional"
#define DOM_s " Domain Controller"
#define SER_s " Server"
#define ADV_s " Advanced"
#define DTC_s " DataCenter"
#define TER_s " Terminal"
#define BOF_s " BackOffice"

#define MY_VER_SERVER_NT						0x80000000
#define MY_VER_WORKSTATION_NT					0x40000000
#define MY_VER_SUITE_SMALLBUSINESS				0x00000001
#define MY_VER_SUITE_ENTERPRISE					0x00000002
#define MY_VER_SUITE_BACKOFFICE					0x00000004
#define MY_VER_SUITE_COMMUNICATIONS				0x00000008
#define MY_VER_SUITE_TERMINAL					0x00000010
#define MY_VER_SUITE_SMALLBUSINESS_RESTRICTED	0x00000020
#define MY_VER_SUITE_EMBEDDEDNT					0x00000040
#define MY_VER_SUITE_DATACENTER					0x00000080
#define MY_VER_SUITE_SINGLEUSERTS				0x00000100
#define MY_VER_SUITE_PERSONAL					0x00000200
#define MY_VER_SUITE_BLADE						0x00000400

#define MY_VER_NT_WORKSTATION					0x0000001
#define MY_VER_NT_DOMAIN_CONTROLLER				0x0000002
#define MY_VER_NT_SERVER						0x0000003

#define CPU_CONFIGURATION_KEY "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"

typedef struct _MYOSVERSIONINFOEXA {

    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    TCHAR szCSDVersion[128];
    WORD  wServicePackMajor;
    WORD  wServicePackMinor;
    WORD  wSuiteMask;
    BYTE  wProductType;
    BYTE  wReserved;
} MYOSVERSIONINFOEXA, *MYPOSVERSIONINFOEXA, *MYLPOSVERSIONINFOEXA;

typedef MYOSVERSIONINFOEXA MYOSVERSIONINFOEX;
typedef MYPOSVERSIONINFOEXA MYPOSVERSIONINFOEX;
typedef MYLPOSVERSIONINFOEXA MYLPOSVERSIONINFOEX;

class CSystemInfo  
{
public:
	CSystemInfo();
	virtual ~CSystemInfo();
	
	BOOL IsWindowsNT() { return m_bWindowsNT; };
	void DetectCPU();
	void DetectOSVer();
	void DetectNumProcessors();

	LPCTSTR GetCPUNameString() { return (LPCTSTR)m_sCPUNameString; };
	LPCTSTR GetCPUVendorIdentifier() { return (LPCTSTR)m_sCPUVendorIdentifier; };
	LPCTSTR GetCPUIdentifier() { return (LPCTSTR)m_sCPUIdentifier; };
	DWORD   GetCPUSpeed() { return m_dwCPUSpeed; };
	DWORD   GetNumProcessors() { return m_dwNumProcessors; };

	DWORD GetWinMajor() { return m_dwWinMajor; };
	DWORD GetWinMinor() { return m_dwWinMinor; };
	DWORD GetBuildNumber() { return m_dwBuildNumber; };
	DWORD GetServicePack() { return m_dwServicePack; };
	LPCTSTR GetOSType() { return (LPCTSTR)m_sOSType; };

protected:
	CString m_sWorkingDir;

	// Windows ver and CPU name
	CString m_sCPUNameString;
	CString m_sCPUIdentifier;
	CString m_sCPUVendorIdentifier;
	CString	m_sOSType;
	BOOL  m_bWindowsNT;
	DWORD m_dwCPUSpeed;
	DWORD m_dwNumProcessors;
	DWORD m_dwServicePack;
	DWORD m_dwBuildNumber;
	DWORD m_dwWinMinor;
	DWORD m_dwWinMajor;
};

#endif // !defined(AFX_SYSTEMINFO_H__3D874E31_833E_484B_ACA8_DA5467490D43__INCLUDED_)
