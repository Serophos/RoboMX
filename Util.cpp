#include "stdafx.h"
#include ".\util.h"
#include ".\ini.h"
#include ".\settings.h"

extern CSettings g_sSettings;

const char* line_types[] = {"Unknown", "14.4K", "28.8K", "33.3K", "56K", 
                            "64K ISDN", "128K ISDN", "Cable", "DSL", "T1", "T3"};


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

	strReturn.Format("%d.%d.%d.%d", ((dwIP >> 24) & 0xff), ((dwIP >> 16) & 0xff), ((dwIP >> 8) & 0xff), (dwIP & 0xff));
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
