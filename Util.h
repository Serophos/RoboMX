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
};
