#include "stdafx.h"
#include ".\logfile.h"
#include "Settings.h"
#include "util.h"

extern CSettings g_sSettings;

CLogFile::CLogFile(void)
{

	m_bFileOpened = FALSE;
}

CLogFile::~CLogFile(void)
{
	
	if(m_bFileOpened){

		TRY{

			m_fLog.Flush();
			m_fLog.Close();
		}
		CATCH(CFileException, e){

		}END_CATCH;

	}
}

void CLogFile::SetRoom(CString strRoom)
{

	m_strRoom = Util::MakeValidFilename(strRoom);
}

void CLogFile::Open()
{

	ShellExecute(0, "open", 
				g_sSettings.GetLogDir() + "\\" +  m_strRoom + Util::GetDateString() + ".txt",
                0, 0, SW_SHOW);
}

void CLogFile::Log(CString strName, CString strMsg)
{

	ASSERT(!m_strRoom.IsEmpty());

	m_strFilename = g_sSettings.GetLogDir() + "\\" +  m_strRoom + Util::GetDateString() + ".txt";

	TRY{

		Util::CreateDirs(g_sSettings.GetLogDir() + "\\");
		m_bFileOpened = m_fLog.Open(m_strFilename, CFile::shareDenyNone|CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::typeText);
		if(m_bFileOpened){

			m_fLog.SeekToEnd();
			CString strTime = Util::GetMyLocalTime();
			CString strLog;

			if(strMsg.IsEmpty()){

				strLog.Format("[%s] %s\n", strTime, strName);
			}
			else{
				
				strLog.Format("[%s] %s %s\n", strTime, strName, strMsg);
			}

			m_fLog.WriteString(strLog);
			m_fLog.Flush();
			m_fLog.Close();
			m_bFileOpened = FALSE;
		}
	}
	CATCH(CFileException, e){

		m_bFileOpened = FALSE;
	}END_CATCH;
}
