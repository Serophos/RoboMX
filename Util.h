#pragma once

class Util
{
public:
	static int ScanMessage(char *pBuff, WORD wMaxSize, char *lpszFormat, ...);
	static WORD FormatMXMessage(WORD wType, char *pBuff, char *lpszFormat, ...);
	static CString FormatIP(DWORD dwIP);
	static CString FormatLine(WORD wLine);
	static CString FormatInt(int nNumber);
	static int axtoi(char *hexStg, int nLen);
	static int ShellExecuteWait(const CString strExec, const CString strParam);
	static void MakeValidUserName(CString& strName);

	static CString GetStayTime(DWORD dwJoinTime);
	static CString GetMyLocalTime(void);
	static void ReplaceVars(CString& strMsg);
	static CString GetMySystemInfo();
	static CString GetWinampSong();
	static BOOL    IsVideoPlaying();
	static CString GetUpTime();

	static BOOL CheckRichEdit(_TCHAR *lpszModuleName);
	static BOOL FileExists(LPCTSTR lpszFile);
};
