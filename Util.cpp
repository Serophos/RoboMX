#include "stdafx.h"
#include ".\util.h"
#include ".\ini.h"
#include ".\settings.h"
#include "SystemInfo.h"
#include "Tokenizer.h"
#include "winamp.h"
#include <stdio.h>
#include <direct.h>

extern CSettings g_sSettings;
CSystemInfo  g_sSystem;

const char* line_types[] = {"Unknown", "14.4K", "28.8K", "33.3K", "56K", 
                            "64K ISDN", "128K ISDN", "Cable", "DSL", "T1", "T3"};

#pragma comment(lib, "version.lib")

#define NUM_LINES 11

WORD Util::FormatMXMessage(WORD wType, char *pBuff, char *lpszFormat, ...)
{

	// Data Format
	va_list Args;
	va_start(Args, lpszFormat);
	WORD wLen = 4;
	char *lpText = NULL;
	
	ZeroMemory(pBuff, sizeof(pBuff));
	
	while(*lpszFormat){

		switch(*lpszFormat){

		case 'S' :	// NULL Terminated String
			lpText = va_arg(Args, char *);
			lstrcpy(pBuff + wLen, lpText);
			wLen += (lstrlen(lpText) + 1);
			break;
		case 's' :	// Not NULL Terminated String
			lpText = va_arg(Args, char *);
			lstrcpy(pBuff + wLen, lpText);
			wLen += (lstrlen(lpText));
			break;
		case 'B' :	// BYTE
			pBuff[wLen] = va_arg(Args, BYTE);
			wLen += 1;
			break;
		case 'W' :	// WORD
			*((WORD *)(pBuff + wLen)) = va_arg(Args, WORD);
			wLen += 2;
			break;
		case 'D' :	// DWORD
			*((DWORD *)(pBuff + wLen)) = va_arg(Args, DWORD);
			wLen += 4;
			break;
		default :
			ASSERT(0);
			break;
	}

	lpszFormat++;
	}

	pBuff[wLen] = 0;

	va_end(Args);

	*((WORD*)pBuff) = wType;
	*((WORD*)(pBuff+2)) = wLen-4;

	return wLen;
}

int Util::ScanMessage(char *pBuff, WORD wMaxSize, char *lpszFormat, ...)
{

	ASSERT(pBuff && wMaxSize && lpszFormat);

	va_list Args;
	va_start(Args, lpszFormat);

	WORD wPos = 0;
	int iCount = 0;
	char *lpText = NULL;

	while(*lpszFormat && wPos < wMaxSize){

		switch(*lpszFormat){

		case 'S' : // string
			lpText = pBuff + wPos;
			*va_arg(Args, char **) = lpText;
			wPos += (lstrlen(lpText) + 1);
			iCount++;
			break;
		case 'B' : // byte
			*va_arg(Args, BYTE *) = *((BYTE *)(pBuff + wPos));
			wPos += 1;
			iCount++;
			break;
		case 'W' : // word
			*va_arg(Args, WORD *) = *((WORD *)(pBuff + wPos));
			wPos += 2;
			iCount++;
			break;
		case 'D' : // dword
			*va_arg(Args, DWORD *) = *((DWORD *)(pBuff + wPos));
			wPos += 4;
			iCount++;
			break;
		default :
			ASSERT(0);
			break;
		}
	
		lpszFormat++;
	}

	va_end(Args);

	return iCount;
}

CString Util::FormatIP(DWORD dwIP)
{
	
	if(dwIP == 0) return "";

	CString strReturn;
	int a = ((dwIP >> 24) & 0xff);
	int b = ((dwIP >> 16) & 0xff);
	int c = ((dwIP >> 8) & 0xff);
	int d = (dwIP & 0xff);
	strReturn.Format("%d.%d.%d.%d", d, c, b, a);
	return strReturn;
}

CString Util::FormatLine(WORD wLine)
{
	
	CString strLine;
	if((wLine >= 0) && (wLine < NUM_LINES)){
		
		strLine = line_types[wLine];
	}
	else{

		strLine = "N/A";
	}

	return strLine;
}

CString Util::FormatInt(int nNumber)
{

	CString strNum;
	strNum.Format("%d", nNumber);
	return strNum;
}

int Util::axtoi(char *hexStg, int nLen)
{

  int n = 0;         // position in string
  int m = 0;         // position in digit[] to shift
  int count;         // loop index
  int intValue = 0;  // integer value of hex string
  int *digit = new int[nLen];      // hold values to convert
  while (n < nLen) {
	if (hexStg[n]=='\0'){
     
		break;
	}
    if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ){
		//if 0 to 9
        digit[n] = hexStg[n] & 0x0f;            //convert to int
	}
    else if (hexStg[n] >='a' && hexStg[n] <= 'f'){ //if a to f

        digit[n] = (hexStg[n] & 0x0f) + 9; 
	}//convert to int
	else if (hexStg[n] >='A' && hexStg[n] <= 'F'){ //if A to F

        digit[n] = (hexStg[n] & 0x0f) + 9;  
	}//convert to int
    else break;
    n++;
  }
  count = n;
  m = n - 1;
  n = 0;
  while(n < count) {
     // digit[n] is value of hex digit at position n
     // (m << 2) is the number of positions to shift
     // OR the bits into return value
     intValue = intValue | (digit[n] << (m << 2));
     m--;   // adjust the position to set
     n++;   // next digit to process
  }

  delete digit;
  digit = 0;
  return (intValue);
}

int Util::ShellExecuteWait(const CString strExec, const CString strParam)
{

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strExec;		
	ShExecInfo.lpParameters = strParam;	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	return 0;
}

void Util::MakeValidUserName(CString& strName)
{

	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir() + "\\RoboMX.ini");
	if(ini.GetValue("General", "DontForceUserID", 0) == FALSE){

		int nTest = strName.GetLength() - strName.ReverseFind('_');
		if(nTest != 6){

			CString strID;
			strID.Format("%03d_%05d", rand() % 999, rand() % 99999);
			strName += strID;
			return;
		}
	}
}

CString Util::GetStayTime(DWORD dwJoinTime)
{

	DWORD dwMinutes = GetTickCount() - dwJoinTime;
	
	CString strTime;
	strTime.Format("%d", dwMinutes / 1000 / 60);
	
	return strTime;
}

CString Util::GetMyLocalTime()
{

	CString strTime;
	SYSTEMTIME time;
	
	GetLocalTime(&time);
	char szTime[9];
	ZeroMemory(&szTime, 9);
	GetTimeFormat(
					LOCALE_SYSTEM_DEFAULT, 
					(g_sSettings.GetTimeFmt() ? TIME_NOSECONDS : TIME_FORCE24HOURFORMAT),
					&time,
					(g_sSettings.GetTimeFmt() ? "hh':'mm tt" : "HH':'mm':'ss"),
					(char*)&szTime, 8
				 );

	strTime = szTime;
    return strTime;
}

CString Util::GetMyDate(void)
{

	CString strDate;
	SYSTEMTIME time;

	int n = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SSHORTDATE, 0, 0);

	if(n != 0){

		char *szFormat = new char[n];
		ZeroMemory(szFormat, n);

		n = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SSHORTDATE, szFormat, n);

		GetLocalTime(&time);
		
		n = GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &time, szFormat, 0, 0); 
		
		if(n != 0){

			char *szDate = new char[n];
			ZeroMemory(szDate, n);

			n = GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &time, szFormat, szDate, n);
			
			strDate = szDate;
			
			delete szDate;
			szDate   = 0;
		}

		delete szFormat;
		szFormat = 0;
	}
	return strDate;
}

CString Util::GetDateString(void)
{

	CString strData;
	SYSTEMTIME time;
	
	GetLocalTime(&time);

	strData.Format("_%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
	return strData;
}


void Util::ReplaceVars(CString &strMsg)
{

	if(strMsg.Find("%", 0) < 0 ) return;

	CString strArtist, strSong, strVersion, strPlayTime, strTotalTime, strRemTime, strSampleRate, strBitrate, strNumChannels, strStatus = "not running";
	
	strMsg.Replace(_T("%TIME%"), Util::GetMyLocalTime());
	strMsg.Replace(_T("%DATE%"), Util::GetMyDate());

	CString strTmp = Util::GetWinampSong();


	if(strMsg.Find("%WA-", 0) >= 0){


		CTokenizer token(strTmp, "-");
		token.Next(strArtist);
		strSong = token.Tail();
		strArtist.TrimRight();
		strSong.TrimLeft();
		
		HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);

		if(hwndWinamp != NULL){


			strVersion.Format("%x", ::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETVERSION));
			strVersion.SetAt(1, '.');

			int nTotal = 0, nRem = 0, nEla = 0;
			nTotal = ::SendMessage(hwndWinamp, WM_WA_IPC, 1, IPC_GETOUTPUTTIME);
			strTotalTime.Format("%02d:%02d", nTotal/60, nTotal%60);

			nEla= ::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETOUTPUTTIME) / 1000;
			strPlayTime.Format("%02d:%02d", nEla/60, nEla%60);
			
			nRem = nTotal - nEla;
			strRemTime.Format("%02d:%02d", nRem/60, nRem%60);

			strSampleRate.Format("%d", ::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_GETINFO));
			strBitrate.Format("%d", ::SendMessage(hwndWinamp, WM_WA_IPC, 1, IPC_GETINFO));
			strNumChannels = (::SendMessage(hwndWinamp, WM_WA_IPC, 2, IPC_GETINFO) == 1 ? "Mono" : "Stereo");

			switch(::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_ISPLAYING)){

			case 1: strStatus = "playing";
				break;
			case 3: strStatus = "paused";
				break;
			default: strStatus = "stopped";
			}

		}
		strMsg.Replace(_T("%WA-ARTIST%"), strArtist);
		strMsg.Replace(_T("%WA-SONG%"), strSong);
		strMsg.Replace(_T("%WA-VERSION%"), strVersion);
		strMsg.Replace(_T("%WA-ELATIME%"), strPlayTime);
		strMsg.Replace(_T("%WA-REMTIME%"), strRemTime);
		strMsg.Replace(_T("%WA-TOTALTIME%"), strTotalTime);
		strMsg.Replace(_T("%WA-SAMPLERATE%"), strSampleRate);
		strMsg.Replace(_T("%WA-BITRATE%"), strBitrate);
		strMsg.Replace(_T("%WA-CHANNELS%"), strNumChannels);
		strMsg.Replace(_T("%WA-STATUS%"), strStatus);
	}	
}

CString Util::GetUpTime()
{

	CString strUp;
	DWORD nMS = GetTickCount();
	int nSec = nMS / 1000;
	int nMin = nMS / 60000;
	int nHour = nMS / 3600000;
	strUp.Format("System Uptime %02d days %02d hours %02d minutes %02d seconds %03d milliseconds",
		nHour/24, nHour%24, nMin - nHour*60, nSec - nMin*60, nMS-nSec*1000);

	return strUp;
}

CString Util::GetWinampSong()
{

	CString strWinamp, strOut;
	HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);
	if(hwndWinamp != NULL){

		::SendMessage(hwndWinamp, WM_WA_IPC,0,IPC_UPDTITLE);
		TCHAR *buff = new TCHAR[250];
		::GetWindowText(hwndWinamp, buff, 250);
		strWinamp = buff;
		strWinamp = strWinamp.Mid(strWinamp.Find(" ", 0)+1, strWinamp.Find(" - Winamp") - strWinamp.Find(" ", 0)-1);
		delete buff;
		buff = NULL;
	}
	else{

		strWinamp = "Winamp - Not active";
	}

	return strWinamp;
}


BOOL Util::IsVideoPlaying()
{

	HWND hwndWinamp = ::FindWindow("Winamp v1.x",NULL);
	BOOL bReturn = FALSE;
	switch(::SendMessage(hwndWinamp, WM_WA_IPC, 0, IPC_IS_PLAYING_VIDEO)){

	case 0:
	case 1:
		bReturn = FALSE;
		break;
	default:
		bReturn = TRUE;
	}

	return bReturn;

}


CString Util::GetMySystemInfo()
{

	CString strInfo;
	strInfo.Format(
				"%s build=%d «» CPUs %d Speed %d MHz «» %s %s %s",
				g_sSystem.GetOSType(), g_sSystem.GetBuildNumber(),
				g_sSystem.GetNumProcessors(), g_sSystem.GetCPUSpeed(),
				g_sSystem.GetCPUIdentifier(), g_sSystem.GetCPUVendorIdentifier(), g_sSystem.GetCPUNameString()
			);
	
	return strInfo;
}

BOOL Util::CheckRichEdit(_TCHAR *lpszModuleName)
{


	BOOL bReturn = FALSE;
    LPBYTE  lpVersionData; 
    DWORD   dwLangCharset; 

 	/*_TCHAR lpszModuleName[ MAX_PATH + 1 ] = { '\0',};
	
	// Get Comctl32.dll product version
	GetSystemDirectory( lpszModuleName,	MAX_PATH );
	
	_tcscat(lpszModuleName, _T("\\RICHED32.DLL"));	  */


	DWORD dwHandle;     
    DWORD dwDataSize = ::GetFileVersionInfoSize(lpszModuleName, &dwHandle); 
    if ( dwDataSize == 0 ){

        return FALSE;
	}

    lpVersionData = new BYTE[dwDataSize]; 
    if(!::GetFileVersionInfo((LPTSTR)lpszModuleName, dwHandle, dwDataSize, (void**)lpVersionData)){

		delete[] lpVersionData; 
		lpVersionData = NULL;
		dwLangCharset = 0;

        return FALSE;
    }

    UINT nQuerySize;
    DWORD* pTransTable;
    if (!::VerQueryValue(lpVersionData, _T("\\VarFileInfo\\Translation"),
                         (void **)&pTransTable, &nQuerySize)){

		delete[] lpVersionData; 
		lpVersionData = NULL;
		dwLangCharset = 0;

        return FALSE;
    }

    // Swap the words to have lang-charset in the correct format
    dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

    // Query version information value
    LPVOID lpData;
    CString strVersion, strBlockName;

    strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), dwLangCharset, _T("FileVersion"));

    if(::VerQueryValue((void **)lpVersionData, strBlockName.GetBuffer(0), &lpData, &nQuerySize)){

        strVersion = (LPCTSTR)lpData;
	}

    strBlockName.ReleaseBuffer();

	float fVersion = 0.0f;
	_stscanf(strVersion, _T("%f"), & fVersion);

	bReturn = (fVersion >= 5.0f);

	delete[] lpVersionData; 
	lpVersionData = NULL;
	dwLangCharset = 0;

	return bReturn;
}

BOOL Util::FileExists(LPCTSTR lpszFile)
{

	TRY{
		
		CFileStatus rStatus;
		if(CFile::GetStatus(lpszFile, rStatus)){

			return TRUE;
		}
		else{

			return FALSE;
		}
	}
	CATCH(CFileException, e){

		TRACE0("File access exception/violation in Util::FileExist\n");
		return TRUE;
	}
	END_CATCH;

	return FALSE;
}

BOOL Util::CreateDirs(CString strFilename)
{

	int nStart = 3;
	int nEnd = 0;
	
	CString validated = strFilename.Left(2);
	_tchdir(validated);
	
	while(TRUE){

		nEnd = strFilename.Find('\\', nStart);
		if(nEnd == -1){

			return TRUE;
		}

		if(_tchdir(validated + "\\" + strFilename.Mid(nStart, nEnd - nStart)) != 0){

			_tchdir(validated);
			if(_tmkdir(strFilename.Mid(nStart, nEnd - nStart)) != 0){

				return FALSE;
			}
			
		}
		validated += "\\" + strFilename.Mid(nStart, nEnd - nStart);
		nStart = nEnd + 1;		
	}

	return FALSE;
}

CString Util::MakeValidFilename(CString strName)
{

	strName.Remove('?');
	strName.Remove(':');
	strName.Remove(',');
	strName.Remove('\\');
	strName.Remove('/');
	strName.Remove('<');
	strName.Remove('>');
	strName.Remove('\"');
	strName.Remove('*');
	strName.Remove('|');
	strName.Replace(1, '-');
	
	return strName;
}

CString Util::GetIPFromRoom(CString strRoom)
{
	
	int nIndex = strRoom.ReverseFind('_')+1;

	if(nIndex <= 0) return "";

	CString strTmp = strRoom.Mid(nIndex, 8);
	
	int nA = 0, nB = 0, nC = 0, nD = 0;
	
	nA = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(0,2), 2);
	nB = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(2,2), 2);
	nC = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(4,2), 2);
	nD = Util::axtoi((LPSTR)(LPCSTR)strTmp.Mid(6,2), 2);

	strTmp.Format("%d.%d.%d.%d", nD, nC, nB, nA);
	return strTmp;
}

LPCTSTR _tcsistr(LPCTSTR pszString, LPCTSTR pszPattern)
{
	LPCTSTR pptr, sptr, start;
	DWORD slen, plen;

	for (	start	= pszString,
			pptr	= pszPattern,
			slen	= _tcslen( pszString ),
			plen	= _tcslen( pszPattern ) ;
			slen >= plen ; start++, slen-- )
	{
		while ( toupper( *start ) != toupper( *pszPattern ) )
		{
			start++;
			slen--;

			if ( slen < plen ) return NULL;
		}

		sptr = start;
		pptr = pszPattern;

		while ( toupper( *sptr ) == toupper( *pptr ) )
		{
			sptr++;
			pptr++;

			if ( ! *pptr) return start;
		}
	}

	return NULL;
}
