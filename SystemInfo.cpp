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

#include "stdafx.h"
#include "Metis3.h"
#include "SystemInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSystemInfo::CSystemInfo()
{

	if(GetVersion() < 0x80000000){

		m_bWindowsNT = TRUE;

	} 
	else{ 

		m_bWindowsNT=FALSE;
	}

	m_dwWinMajor		= 0;
	m_dwWinMinor		= 0;
	m_dwBuildNumber		= 0;
	m_dwServicePack		= 0;
	m_dwNumProcessors	= 0;
	m_dwCPUSpeed		= 0;

	DetectOSVer();
	DetectNumProcessors();
	DetectCPU();

}

CSystemInfo::~CSystemInfo()
{

}

void CSystemInfo::DetectOSVer()
{

	BOOL bOSINFOEXWorking=FALSE;

	OSVERSIONINFO v;
	ZeroMemory(&v,sizeof(OSVERSIONINFO));
	v.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx((LPOSVERSIONINFO)&v);

	m_dwWinMajor = v.dwMajorVersion;
	m_dwWinMinor = v.dwMinorVersion;
	m_dwBuildNumber = v.dwBuildNumber;

	if(!IsWindowsNT()){

		// Crap Windows
		if(m_dwWinMinor == 0){ 
			
			m_sOSType = W95_s; 
		}
		if(m_dwWinMinor == 10){ 
			
			m_sOSType = W98_s; 
		}
		if(m_dwWinMinor == 90){ 
			
			m_sOSType = WME_s; 
		}
		m_sOSType += v.szCSDVersion;
		return;  // this is crap windows. we are done
	};

	// Real OS :-)
	// Get Service Pack number
	int n = 0, j = -1;
	CString sSvcPack; 
	TCHAR szNum[10];

	sSvcPack = v.szCSDVersion;

	for(n = 1; n <= 9; n++){

		_stprintf(szNum, "%d",n);
		j = sSvcPack.Find(szNum);
		if (j>=0) break;
	}

	if(j >= 0){
		
		m_dwServicePack = n;
	}

	// Check for OSVERSIONINFOEX-compatibility
	// It works only on Windows NT 4.0 Service Pack 6 or later -sigh-
	if((m_dwWinMajor>=5) || (m_dwServicePack>=6)){ 
	
		bOSINFOEXWorking = TRUE;
	}

	if(m_dwWinMajor == 4){

		// Windows NT 4.0
		m_sOSType=_T(NT4_s);
	}
	else{

		if(m_dwWinMajor>=5){

			if (m_dwWinMinor == 0){ 
				
				m_sOSType = W2K_s; 
			}  // Windows 2000
			if(m_dwWinMinor == 1){ 
				
				m_sOSType = WXP_s;
			};  // Windows XP
			if(m_dwWinMinor == 2){

				m_sOSType = W2K3_s;
			}
		}
	}

	if(bOSINFOEXWorking){ // decent os ;)

		MYOSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		ZeroMemory(&osvi, sizeof(MYOSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(MYOSVERSIONINFOEX);
		bOsVersionInfoEx = GetVersionEx((LPOSVERSIONINFO)&osvi);

		if(bOsVersionInfoEx){

			if(osvi.wProductType==MY_VER_NT_WORKSTATION){

				if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 2)){

					m_sOSType += W2K3_s;
				}
				if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 1)){

					// Windows XP
					if(osvi.wSuiteMask & MY_VER_SUITE_PERSONAL){
						
						m_sOSType += XHE_s; 
					}
					else{ 

						m_sOSType += XPR_s; 
					}
				}

				if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0)){

					// Windows 2000 Professional
					m_sOSType+=_T(PRO_s);
				}

				if(osvi.dwMajorVersion==4){

					// Windows NT 4.0 Workstation
					m_sOSType+=_T(WKS_s);
				}
			}

			if((osvi.wProductType == MY_VER_NT_SERVER) || 
				(osvi.wProductType == MY_VER_NT_DOMAIN_CONTROLLER)){

				if(osvi.dwMajorVersion == 5){

					if(osvi.wSuiteMask & MY_VER_SUITE_ENTERPRISE){ 
						
						m_sOSType += ADV_s; 
					}
					if(osvi.wSuiteMask & MY_VER_SUITE_DATACENTER){ 
						
						m_sOSType += DTC_s; 
					}
					if(osvi.wSuiteMask & MY_VER_SUITE_TERMINAL){ 
						
						m_sOSType += TER_s;
					}
					if(osvi.wSuiteMask & MY_VER_SUITE_BACKOFFICE){ 
						
						m_sOSType += BOF_s; 
					}
					m_sOSType += SER_s;
				}
				if (osvi.dwMajorVersion==4 && osvi.dwMinorVersion==0){ 
				
					m_sOSType += SER_s; 
				}
			}

			if (osvi.wProductType == MY_VER_NT_DOMAIN_CONTROLLER){ 
				
				m_sOSType+=_T(DOM_s); 
			}
		}
	}

	if (m_dwServicePack>0){

		m_sOSType += " ";
		m_sOSType += v.szCSDVersion;
	}
}

void CSystemInfo::DetectNumProcessors()
{
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	m_dwNumProcessors = sinfo.dwNumberOfProcessors;
}

void CSystemInfo::DetectCPU()
{

    LONG	lresult;
    HKEY	hKey;

    lresult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T(CPU_CONFIGURATION_KEY),
        0,KEY_EXECUTE,&hKey);

    if (ERROR_SUCCESS != lresult) return;		// key not found

    TCHAR szKeyValue[100]; memset(szKeyValue,0,100);
    DWORD dwType=REG_SZ; DWORD dwSize=100;

    lresult = RegQueryValueEx(hKey, "Identifier", NULL,
                               &dwType, (LPBYTE)szKeyValue, &dwSize);

	if((lresult == ERROR_SUCCESS) && (dwSize > 0)){
		
		m_sCPUIdentifier=szKeyValue; 
	}

	memset(szKeyValue,0,100); 
	dwType = REG_SZ; 
	dwSize = 100;
    
	lresult = RegQueryValueEx(hKey, "VendorIdentifier", NULL,
                               &dwType, (LPBYTE)szKeyValue, &dwSize);
	
	if((lresult == ERROR_SUCCESS) && (dwSize > 0)){ 
		
		m_sCPUVendorIdentifier=szKeyValue; 
	}

	memset(szKeyValue,0,100);
	dwType = REG_SZ;
	dwSize = 100;

    lresult = RegQueryValueEx(hKey, "ProcessorNameString", NULL,
		                       &dwType, (LPBYTE)szKeyValue, &dwSize);

	if((lresult == ERROR_SUCCESS) && (dwSize>0)){
		
		m_sCPUNameString=szKeyValue; 
	}

	DWORD dwData = 0; 
	dwType = REG_DWORD; 
	dwSize = sizeof(dwData);

    lresult = RegQueryValueEx(hKey, "~MHz", NULL,
                               &dwType,(LPBYTE)(&dwData),&dwSize);

	if((lresult == ERROR_SUCCESS) && (dwSize>0)){ 
		
		m_dwCPUSpeed = dwData; 
	}

	RegCloseKey(hKey);
}
