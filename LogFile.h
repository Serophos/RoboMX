#pragma once

class CLogFile
{
public:
	CLogFile(void);
	~CLogFile(void);

public:
	void SetRoom(CString strRoom);
	void Open();
	void Log(CString strName, CString strMsg);

protected:
	CStdioFile	m_fLog;
	CString		m_strFilename;
	CString		m_strRoom;
	BOOL		m_bFileOpened;
};
