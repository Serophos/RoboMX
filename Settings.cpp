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
#include "Settings.h"
#include "Ini.h"
#include ".\settings.h"
#include "Tokenizer.h"
#include "RichElement.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUm_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CSettings::CSettings()
{


	m_bMsBold		= TRUE;
	m_bMsItalic		= FALSE;
	m_bMsLine		= FALSE;
	m_bColorAcName	= FALSE;
	m_bTime			= TRUE;
	m_nDepth		= 50;
	m_bHistory		= TRUE;
	m_nFontSize		= 12;
	m_bBlockHttp	= FALSE;
	m_bBlockMotd	= FALSE;
	m_bBlockPrivate = FALSE;
	m_bBlockTopic	= FALSE;
	m_bBlockEmpty	= FALSE;
	m_bBlockAscii	= FALSE;
	m_bBlockAdmin	= FALSE;
	m_bFilterXtrem  = FALSE;
	m_bFilterIgnore = FALSE;
	m_bFilterNormal = FALSE;
	m_bUpdate		= TRUE;
	m_bAutoList		= TRUE;
	m_bMaxi			= FALSE;	
	m_bDisplayNode	= FALSE;
	m_strFont		= "Arial";
	m_bBarTop		= TRUE;

	m_dwFiles		= 1;
	m_wLine			= 0;

	m_dwTextStyle = retfColor|retfBold;

	m_strGodName	= "Master";
	m_dwGodFiles	= 0;
	m_wGodLine		= 0;
	m_bGodVisible	= FALSE;
	m_bModerated	= FALSE;
	m_dwLimit		= 40;
	m_bMultiIPOk	= TRUE;
	m_bBLockNushi	= FALSE;
	m_bLocalIsOp	= TRUE;
	
	m_crBg = RGB(255,255,255);

	m_FA = "";
	m_FM = "";
	m_EA = "";
	m_EM = ":";

	m_bRetry = TRUE;
	m_nRetries = 3;
}

CSettings::~CSettings()
{

}

void CSettings::Load()
{

	TCHAR szBuffer[_MAX_PATH]; 
	::GetModuleFileName(AfxGetInstanceHandle(), szBuffer, _MAX_PATH);
	m_strWd.Format("%s", szBuffer);
	m_strWd = m_strWd.Left(m_strWd.ReverseFind('\\'));


	CIni ini;
	ini.SetIniFileName(m_strWd + "\\RoboMX.ini");

	m_strLanguage = ini.GetValue("General", "Language", "English");
	m_strNickname = ini.GetValue("UserInfo", "Nickname", "RoboMX.494_12345");
	m_dwFiles	  = ini.GetValue("UserInfo", "Files", 1);
	m_wLine		  = ini.GetValue("UserInfo", "Line", 0);

	if(m_wLine > 8) m_wLine = 0;
	if(m_dwFiles > 65535) m_dwFiles = 1;
	
	m_bUpdate		= ini.GetValue("General", "CheckUpdate", TRUE);
	m_strLogDir     = ini.GetValue("General", "LogDir", m_strWd + "\\Logs");
	m_bLog			= ini.GetValue("General", "Log", 0);
	m_bRetry		= ini.GetValue("General", "ConnectionRetry", TRUE);
	m_nRetries		= ini.GetValue("General", "ConnectionRetries", 3);

	m_bAutoList		= ini.GetValue("Look & Feel", "AutoList", TRUE);
	m_bMaxi			= ini.GetValue("Look & Feel", "AlwaysMaximizeMDI", FALSE);	
	m_bHideSystem	= ini.GetValue("Look & Feel", "AutoHideSystem", TRUE);

	m_bMiniTray		= ini.GetValue("Look & Feel", "Mini2Tray", FALSE);
	m_bTime			= ini.GetValue("Look & Feel", "Timestamp", TRUE);
	m_bBarTop		= ini.GetValue("Look & Feel", "BarTop", TRUE);

	m_FA    = ini.GetValue("Look & Feel", "EnActFront", "* ");
	m_FM    = ini.GetValue("Look & Feel", "EnMsgFront", "");
	m_EA    = ini.GetValue("Look & Feel", "EnActEnd", "");
	m_EM    = ini.GetValue("Look & Feel", "EnMsgEnd", " :");

	m_FA.Replace("�", " ");  // replace Alt+0160 with regular space
	m_FM.Replace("�", " ");  // replace Alt+0160 with regular space
	m_EA.Replace("�", " ");  // replace Alt+0160 with regular space
	m_EM.Replace("�", " ");  // replace Alt+0160 with regular space


    m_bSoundFX = ini.GetValue("Look & Feel", "SoundFX", TRUE);
	m_bImage   = ini.GetValue("Look & Feel", "UseImage", TRUE);
	m_strImage = ini.GetValue("Look & Feel", "BG-Image", m_strWd + "\\gfx\\robomx.bmp");
	m_bPing	   = ini.GetValue("Look & Feel", "ShowPing", FALSE);
	m_bDisplayNode = ini.GetValue("Look & Feel", "DisplayNode", FALSE);

	m_nTimeFormat = ini.GetValue("Look & Feel", "TimeFormat", 0);
	m_bHiliteUsers = ini.GetValue("Look & Feel", "HiliteUsers", FALSE);
	m_bEmoticons   = ini.GetValue("Look & Feel", "Emoticons", TRUE);

	m_nDepth    = ini.GetValue("History", "Depth", 50);
	m_bHistory  = ini.GetValue("History", "Enable", TRUE);

	m_bScroller = ini.GetValue("Messages", "EnableScroller", TRUE);
	m_strVideoMsg = ini.GetValue("Messages", "WinampVideo", "/me watches '%WA-ARTIST% - %WA-SONG%'");
	m_strWinampMsg = ini.GetValue("Messages", "Winamp", "/me listens to '%WA-ARTIST% - %WA-SONG%'");
	m_bOnEnter     = ini.GetValue("Messages", "OnEnterEnable", FALSE);
	m_strEnterMsg  = ini.GetValue("Messages", "OnEnterMsg", "Hello.");
	m_strJoin      = ini.GetValue("Messages", "JoinMessage", ">>> Joins: %NAME% (%LINE%, %FILES% files%)");
	m_strPart     = ini.GetValue("Messages", "PartMessage", "<<< Parts: %NAME%");
	m_strEnterMsg.Replace("\\rtf", "****");

	m_bBlockHttp = ini.GetValue("Filter", "BlockURL", FALSE);
	m_bBlockMotd = ini.GetValue("Filter", "BlockMotd", FALSE);
	m_bBlockPrivate = ini.GetValue("Filter", "BlockPrivate", FALSE);
	m_bBlockTopic = ini.GetValue("Filter", "BlockTopic", FALSE);
	m_bBlockEmpty = ini.GetValue("Filter", "BlockEmpty", FALSE);
	m_bBlockAscii = ini.GetValue("Filter", "BlockAscii", FALSE);
	m_bBlockAdmin = ini.GetValue("Filter", "BlockAdmin", FALSE);
	m_bFilterXtrem = ini.GetValue("Filter", "FilerXtrem", FALSE);
	m_bFilterIgnore = ini.GetValue("Filter", "FilterIngore", FALSE);
	m_bFilterNormal = ini.GetValue("Filter", "FilterNormal", FALSE);


	// FOrmat stuff
	m_bMsBold   = ini.GetValue("Font", "MessageBold", TRUE);
	m_bMsItalic = ini.GetValue("Font", "MessageItalic", FALSE);
	m_bMsLine   = ini.GetValue("Font", "MessageLine", FALSE);
	m_strFont   = ini.GetValue("Font", "ChatFont",   "Arial");
	m_nFontSize = ini.GetValue("Font", "ChatFontSize", 12);
	
	switch(ini.GetValue("Font", "Charset", 0)){

	case 1:
		m_nCharset = BALTIC_CHARSET;
		break;
	case 2:
		m_nCharset = CHINESEBIG5_CHARSET;
		break;
	case 3:
		m_nCharset = DEFAULT_CHARSET;
		break;
	case 4:
		m_nCharset = EASTEUROPE_CHARSET;
		break;
	case 5:
		m_nCharset = GB2312_CHARSET;
		break;
	case 6:
		m_nCharset = GREEK_CHARSET;
		break;
	case 7:
		m_nCharset = HANGEUL_CHARSET;
		break;
	case 8:
		m_nCharset = MAC_CHARSET;
		break;
	case 9:
		m_nCharset = OEM_CHARSET;
		break;
	case 10:
		m_nCharset = RUSSIAN_CHARSET;
		break;
	case 11:
		m_nCharset = SHIFTJIS_CHARSET;
		break;
	case 12:
		m_nCharset = SYMBOL_CHARSET;
		break;
	case 13:
		m_nCharset = TURKISH_CHARSET;
		break;
	case 0:
	default:
		m_nCharset = ANSI_CHARSET;
		break;
	}

	m_bColorAcName = ini.GetValue("Colors", "ColorActionName", FALSE);

	m_dwTextStyle = retfColor;
	if(m_bMsBold)	m_dwTextStyle |= retfBold;
	if(m_bMsItalic) m_dwTextStyle |= retfItalic;
	if(m_bMsLine)	m_dwTextStyle |= retfUnderline;

	m_crFocus    = ini.GetValue("Colors", "FocusColor", RGB(255, 245, 210));
	m_bFocus      = ini.GetValue("Colors", "UseFocusColor", 1);

	m_crActionBr = ini.GetValue("Colors", "ActionNameEnc", RGB(0, 91, 183));
	m_crAction   = ini.GetValue("Colors", "ActionText", RGB(0, 91, 183));

	m_crNickBr   = ini.GetValue("Colors", "MessageNameEnc", RGB(255, 0, 0));
	m_crMessage  = ini.GetValue("Colors", "MessageText", RGB(0, 0, 0));

	m_crNick     = ini.GetValue("Colors", "NormalNick", RGB(255, 0, 0));

	m_crJoin  = ini.GetValue("Colors", "Join", RGB(255, 150, 0));
	m_crPart  = ini.GetValue("Colors", "Part", RGB(120, 0, 0));
	m_crTopic = ini.GetValue("Colors", "Topic", RGB(0, 0, 120));
	m_crMotd  = ini.GetValue("Colors", "Motd", RGB(0, 0, 180));

	m_crFiles  = ini.GetValue("Colors", "Files", RGB(200, 0, 0));
	m_crLine  = ini.GetValue("Colors", "Line", RGB(0, 150, 0));
	m_crIP  = ini.GetValue("Colors", "IP", RGB(0, 0, 150));
	m_crPort  = ini.GetValue("Colors", "Port", RGB(0, 0, 180));
	m_crPend  = ini.GetValue("Colors", "Pend", RGB(254, 128, 64));
	m_crOK  = ini.GetValue("Colors", "OK", RGB(0, 150, 0));
	m_crErr  = ini.GetValue("Colors", "Err", RGB(150, 0, 0));
	m_crTime = ini.GetValue("Colors", "Time", RGB(255,255,255));
	m_crHiLite	 = ini.GetValue("Colors", "HiLite", RGB(150, 200, 255));
	m_crDocHiLite = ini.GetValue("Colors", "DocHiLite", RGB(0, 0, 255));
	m_crBg = ini.GetValue("Colors", "Background", RGB(255, 255, 255));
	m_crOp = ini.GetValue("Colors", "OpMsgs", RGB(55, 170, 100));

	m_strSfxJoin		= ini.GetValue("SoundFX", "Join", m_strWd + "\\sfx\\Connect.wav");
	m_strSfxPart		= ini.GetValue("SoundFX", "Part", m_strWd + "\\sfx\\Disconnect.wav");
	m_strSfxRedirect	= ini.GetValue("SoundFX", "Redirect", m_strWd + "\\sfx\\Redirect.wav");
	m_strSfxTopic		= ini.GetValue("SoundFX", "Topic", m_strWd + "\\sfx\\Topic.wav");
	m_strSfxMotd		= ini.GetValue("SoundFX", "Motd", m_strWd + "\\sfx\\Motd.wav");
	m_strSfxStart		= ini.GetValue("SoundFX", "Start", m_strWd + "\\sfx\\Start.wav");
	m_strSfxError		= ini.GetValue("SoundFX", "Error", m_strWd + "\\sfx\\Error.wav");
	m_bChatSfx			= ini.GetValue("SoundFX", "ChatSfx", TRUE);
	m_bSoundFX			= ini.GetValue("SoundFX", "Sfx", TRUE);


	m_dwNodeIP			= ini.GetValue("Node", "IP", 2130706433);
	m_dwNodePort		= ini.GetValue("Node", "Port", 14223);

	m_strTopic.LoadString(IDS_DEFAULT_TOPIC);
	m_strMotd.LoadString(IDS_DEFAULT_MOTD);
	m_strKeyMsg.LoadString(IDS_DEFAULT_KEYMSG);

	m_strTopic		= ini.GetValue("Server", "Topic", m_strTopic);
	m_strMotd		= ini.GetValue("Server", "Motd", m_strMotd);
	m_strKeyMsg		= ini.GetValue("Server", "KeyMsg", m_strKeyMsg);
	m_strGodName	= ini.GetValue("Server", "GodName", m_strGodName);
	m_dwGodFiles	= ini.GetValue("Server", "GodFiles", 0);
	m_wGodLine		= ini.GetValue("Server", "GodLine", 0);
	m_bGodVisible	= ini.GetValue("Server", "GodVisible", 0);
	m_bModerated	= ini.GetValue("Server", "Moderated", 0);
	m_dwLimit		= ini.GetValue("Server", "Limit", 40);
	m_bMultiIPOk	= ini.GetValue("Server", "MultiIPOk", 1);
	m_bBLockNushi	= ini.GetValue("Server", "BlockNushi", 0);
	m_bLocalIsOp	= ini.GetValue("Server", "LocalIsOp", 1);

	LoadHiLite();
	LoadRCMS();
	LoadRooms();
	LoadSounds();

}

void CSettings::Save()
{

	ASSERT(!m_strWd.IsEmpty());

	CIni ini;
	ini.SetIniFileName(m_strWd + "\\RoboMX.ini");

	ini.SetValue("General", "Language", m_strLanguage);
	ini.SetValue("UserInfo", "Nickname", m_strNickname);
	ini.SetValue("UserInfo", "Files", m_dwFiles);
	ini.SetValue("UserInfo", "Line", m_wLine);

	ini.SetValue("General", "CheckUpdate", m_bUpdate);
	ini.SetValue("General", "LogDir", m_strLogDir);
	ini.SetValue("General", "Log", m_bLog);
	ini.SetValue("General", "ConnectionRetry", m_bRetry);
	
	if((m_nRetries < 0) || (m_nRetries > 5)) m_nRetries = 3;
	ini.SetValue("General", "ConnectionRetries", m_nRetries);


	ini.SetValue("Look & Feel", "AutoList", m_bAutoList);
	ini.SetValue("Look & Feel", "AlwaysMaximizeMDI", m_bMaxi);	

	ini.SetValue("Look & Feel", "Timestamp", m_bTime);

	m_FA.Replace(" ", "�");  // replace space with Alt+0160 because the windows ini function will trimm spaces :rolleyes:
	m_FM.Replace(" ", "�");  
	m_EA.Replace(" ", "�");  
	m_EM.Replace(" ", "�");  
	ini.SetValue("Look & Feel", "EnActFront", m_FA);
	ini.SetValue("Look & Feel", "EnMsgFront", m_FM);
	ini.SetValue("Look & Feel", "EnActEnd", m_EA);
	ini.SetValue("Look & Feel", "EnMsgEnd", m_EM);
	ini.SetValue("Look & Feel", "Mini2Tray", m_bMiniTray);
	ini.SetValue("Look & Feel", "UseImage", m_bImage);
	ini.SetValue("Look & Feel", "BG-Image", m_strImage);
	ini.SetValue("Look & Feel", "ShowPing", m_bPing);
	ini.SetValue("Look & Feel", "TimeFormat", m_nTimeFormat);
	ini.SetValue("Look & Feel", "HiliteUsers", m_bHiliteUsers);
	ini.SetValue("Look & Feel", "Emoticons", m_bEmoticons);
	ini.SetValue("Look & Feel", "DisplayNode", m_bDisplayNode);
	ini.SetValue("Look & Feel", "AutoHideSystem", m_bHideSystem);
	ini.SetValue("Look & Feel", "BarTop", m_bBarTop);
	ini.SetValue("History", "Depth", m_nDepth);
	ini.SetValue("History", "Enable", m_bHistory);


	ini.SetValue("Messages", "EnableScroller", m_bScroller);
	ini.SetValue("Messages", "WinampVideo", m_strVideoMsg);
	ini.SetValue("Messages", "Winamp", m_strWinampMsg);
	ini.SetValue("Messages", "OnEnterEnable", m_bOnEnter);
	m_strEnterMsg.Replace("\\rtf", "****");
	ini.SetValue("Messages", "OnEnterMsg", m_strEnterMsg);
	ini.SetValue("Messages", "JoinMessage", m_strJoin);
	ini.SetValue("Messages", "PartMessage", m_strPart);

	ini.SetValue("Filter", "BlockURL", m_bBlockHttp);
	ini.SetValue("Filter", "BlockMotd", m_bBlockMotd);
	ini.SetValue("Filter", "BlockPrivate", m_bBlockPrivate);
	ini.SetValue("Filter", "BlockTopic", m_bBlockTopic);
	ini.SetValue("Filter", "BlockEmpty", m_bBlockEmpty);
	ini.SetValue("Filter", "BlockAscii", m_bBlockAscii);
	ini.SetValue("Filter", "BlockAdmin", m_bBlockAdmin);
	ini.SetValue("Filter", "FilerXtrem", m_bFilterXtrem);
	ini.SetValue("Filter", "FilterIngore", m_bFilterIgnore);
	ini.SetValue("Filter", "FilterNormal", m_bFilterNormal);


	ini.SetValue("Font", "MessageBold", m_bMsBold);
	ini.SetValue("Font", "MessageItalic", m_bMsItalic);
	ini.SetValue("Font", "MessageLine", m_bMsLine);
	ini.SetValue("Font", "ChatFont",   m_strFont);
	ini.SetValue("Font", "ChatFontSize", m_nFontSize);

	ini.SetValue("Colors", "ColorActionName", m_bColorAcName);
	ini.SetValue("Colors", "NormalNick", m_crNick);
	ini.SetValue("Colors", "Time", m_crTime);
	
	ini.SetValue("Colors", "ActionNameEnc", m_crActionBr);
	ini.SetValue("Colors", "ActionText", m_crAction);

	ini.SetValue("Colors", "MessageNameEnc", m_crNickBr);
	ini.SetValue("Colors", "MessageText", m_crMessage);

	ini.SetValue("Colors", "Join", m_crJoin);
	ini.SetValue("Colors", "Part", m_crPart);
	ini.SetValue("Colors", "Topic", m_crTopic);
	ini.SetValue("Colors", "Motd", m_crMotd);

	ini.SetValue("Colors", "Files", m_crFiles);
	ini.SetValue("Colors", "Line", m_crLine);
	ini.SetValue("Colors", "IP", m_crIP);
	ini.SetValue("Colors", "Port", m_crPort);
	ini.SetValue("Colors", "Pend", m_crPend);
	ini.SetValue("Colors", "OK", m_crOK);
	ini.SetValue("Colors", "Err", m_crErr);
	ini.SetValue("Colors", "FocusColor", m_crFocus);
	ini.SetValue("Colors", "UseFocusColor", m_bFocus);
	ini.SetValue("Colors", "HiLite", m_crHiLite);
	ini.SetValue("Colors", "DocHiLite", m_crDocHiLite);
	ini.SetValue("Colors", "Background", m_crBg);
	ini.SetValue("Colors", "OpMsgs", m_crOp);

	switch(m_nCharset){

	case BALTIC_CHARSET:
		ini.SetValue("Font", "Charset", 1);
		break;
	case CHINESEBIG5_CHARSET:
		ini.SetValue("Font", "Charset", 2);
		break;
	case DEFAULT_CHARSET:
		ini.SetValue("Font", "Charset", 3);
		break;
	case EASTEUROPE_CHARSET:
		ini.SetValue("Font", "Charset", 4);
		break;
	case GB2312_CHARSET:
		ini.SetValue("Font", "Charset", 5);
		break;
	case GREEK_CHARSET:
		ini.SetValue("Font", "Charset", 6);
		break;
	case HANGEUL_CHARSET:
		ini.SetValue("Font", "Charset", 7);
		break;
	case MAC_CHARSET:
		ini.SetValue("Font", "Charset", 8);
		break;
	case OEM_CHARSET:
		ini.SetValue("Font", "Charset", 9);
		break;
	case RUSSIAN_CHARSET:
		ini.SetValue("Font", "Charset", 10);
		break;
	case SHIFTJIS_CHARSET:
		ini.SetValue("Font", "Charset", 11);
		break;
	case SYMBOL_CHARSET:
		ini.SetValue("Font", "Charset", 12);
		break;
	case TURKISH_CHARSET:
		ini.SetValue("Font", "Charset", 13);
		break;
	case ANSI_CHARSET:
	default:
		ini.SetValue("Font", "Charset", 0);
		break;
	}

	ini.SetValue("SoundFX", "Join", m_strSfxJoin);
	ini.SetValue("SoundFX", "Part", m_strSfxPart);
	ini.SetValue("SoundFX", "Redirect", m_strSfxRedirect);
	ini.SetValue("SoundFX", "Topic", m_strSfxTopic);
	ini.SetValue("SoundFX", "Motd", m_strSfxMotd);
	ini.SetValue("SoundFX", "Start", m_strSfxStart);
	ini.SetValue("SoundFX", "Error", m_strSfxError);
	ini.SetValue("SoundFX", "ChatSfx", m_bChatSfx);
	ini.SetValue("SoundFX", "Sfx", m_bSoundFX);
	
	ini.SetValue("Node", "IP", m_dwNodeIP);
	ini.SetValue("Node", "Port", m_dwNodePort);

	ini.SetValue("Server", "Topic", m_strTopic);
	ini.SetValue("Server", "Motd", m_strMotd);
	ini.SetValue("Server", "KeyMsg", m_strKeyMsg);
	ini.SetValue("Server", "GodName", m_strGodName);
	ini.SetValue("Server", "GodFiles", m_dwGodFiles);
	ini.SetValue("Server", "GodLine", m_wGodLine);
	ini.SetValue("Server", "GodVisible", m_bGodVisible);
	ini.SetValue("Server", "Moderated", m_bModerated);
	ini.SetValue("Server", "Limit", m_dwLimit);
	ini.SetValue("Server", "MultiIPOk", m_bMultiIPOk);
	ini.SetValue("Server", "BlockNushi", m_bBLockNushi);
	ini.SetValue("Server", "LocalIsOp", m_bLocalIsOp);
}


void CSettings::SetCharSet(int nValue)
{

	switch(nValue){

	case 1:
		m_nCharset = BALTIC_CHARSET;
		break;
	case 2:
		m_nCharset = CHINESEBIG5_CHARSET;
		break;
	case 3:
		m_nCharset = DEFAULT_CHARSET;
		break;
	case 4:
		m_nCharset = EASTEUROPE_CHARSET;
		break;
	case 5:
		m_nCharset = GB2312_CHARSET;
		break;
	case 6:
		m_nCharset = GREEK_CHARSET;
		break;
	case 7:
		m_nCharset = HANGEUL_CHARSET;
		break;
	case 8:
		m_nCharset = MAC_CHARSET;
		break;
	case 9:
		m_nCharset = OEM_CHARSET;
		break;
	case 10:
		m_nCharset = RUSSIAN_CHARSET;
		break;
	case 11:
		m_nCharset = SHIFTJIS_CHARSET;
		break;
	case 12:
		m_nCharset = SYMBOL_CHARSET;
		break;
	case 13:
		m_nCharset = TURKISH_CHARSET;
		break;
	case 0:
	default:
		m_nCharset = ANSI_CHARSET;
		break;
	}
}

int CSettings::GetCharSet(BOOL bIndexOnly)
{

	int nReturn = 0;
	if(!bIndexOnly){

		nReturn = m_nCharset;
	}
	else{

		switch(m_nCharset){

		case BALTIC_CHARSET:
			nReturn = 1;
			break;
		case CHINESEBIG5_CHARSET:
			nReturn = 2;
			break;
		case DEFAULT_CHARSET:
			nReturn = 3;
			break;
		case EASTEUROPE_CHARSET:
			nReturn = 4;
			break;
		case GB2312_CHARSET:
			nReturn = 5;
			break;
		case GREEK_CHARSET:
			nReturn = 6;
			break;
		case HANGEUL_CHARSET:
			nReturn = 7;
			break;
		case MAC_CHARSET:
			nReturn = 8;
			break;
		case OEM_CHARSET:
			nReturn = 9;
			break;
		case RUSSIAN_CHARSET:
			nReturn = 10;
			break;
		case SHIFTJIS_CHARSET:
			nReturn = 11;
			break;
		case SYMBOL_CHARSET:
			nReturn = 12;
			break;
		case TURKISH_CHARSET:
			nReturn = 13;
			break;
		case ANSI_CHARSET:
		default:
			nReturn = 0;
			break;
		}
	}
	return nReturn;
}


CString CSettings::GetWorkingDir(BOOL bCached)
{


	if(bCached){

		return m_strWd;
	}

	TCHAR  pszAppPath[ MAX_PATH + 1 ];
	TCHAR* pszSearch = NULL;

	memset( pszAppPath, 0, sizeof( pszAppPath ) );

	GetModuleFileName( NULL, pszAppPath, sizeof( pszAppPath ) );

	// find last \ character
	pszSearch = _tcsrchr( pszAppPath, _T( '\\' ) );

	if ( pszSearch )
	{
		// strip everything after the last \ char, \ char including 
		pszSearch[ 0 ] = '\0';
	}

	return pszAppPath;
}

void CSettings::LoadHiLite(void)
{

	CString strIniFile = GetWorkingDir() + "\\hilite.ini";
	BOOL bReturn = TRUE;
	CStdioFile ini;
	CString strBuffer;
	m_aHilite.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				if(strBuffer[0] != ';'){

					m_aHilite.Add(strBuffer);
				}
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);
	}END_CATCH;
}

void CSettings::LoadQuickCmds()
{

	CString strIniFile = GetWorkingDir() + "\\quick.ini";
	BOOL bReturn = TRUE;
	CStdioFile ini;
	CString strBuffer;
	m_aQuick.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aQuick.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);
	}END_CATCH;
}

void CSettings::LoadRCMS()
{

	CString strIniFile = GetWorkingDir() + "\\RCMS.ini";
	CStdioFile ini;
	CString strBuffer;
	m_aRCMSCommands.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aRCMSCommands.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);
	}END_CATCH;

	///////////////////////////
	// Load WinMX 3.52 commands
	strIniFile = GetWorkingDir() + "\\WinMX.ini";
	m_aWinMXCommands.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aWinMXCommands.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

	}END_CATCH;

	///////////////////////////
	// Load WinMX 3.52 commands
	strIniFile = GetWorkingDir() + "\\RMX.ini";
	m_aRoboMXCommands.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aRoboMXCommands.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

	}END_CATCH;
	///////////////////////
	// Load quick commands
	strIniFile = GetWorkingDir() + "\\quick.ini";
	m_aQuick.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aQuick.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);
	}END_CATCH;
}

void CSettings::LoadRooms(void)
{

	CString strIniFile = GetWorkingDir() + "\\rooms.ini";
	BOOL bReturn = TRUE;
	CStdioFile ini;
	CString strBuffer;
	m_aRooms.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aRooms.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);
	}END_CATCH;

	strIniFile = GetWorkingDir() + "\\hello.ini";
	m_aGreetings.RemoveAll();

	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText|CFile::shareExclusive);

		while(ini.ReadString(strBuffer)){

			if(!strBuffer.IsEmpty()){

				m_aGreetings.Add(strBuffer);
			}
		}
		ini.Close();
		
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);
	}END_CATCH;
}

void CSettings::LoadSounds(void)
{

	CString strIniFile = GetWorkingDir() + "\\MXSound.ini";

	CStdioFile ini;
	CString strBuffer;
	int nMode = -1;
	
	m_aSounds.RemoveAll();

	CString strSection;
	TRY{

		ini.Open(strIniFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead|CFile::typeText);

		while(ini.ReadString(strBuffer)){

			strBuffer.TrimLeft();
			strBuffer.TrimRight();

			if(strBuffer.Left(2) == "//") continue;
			if(strBuffer.Find("[Sounds]", 0) == 0){
				
				nMode = 0;
				continue;
			}
			if(strBuffer.Left(1) == "#"){
				
				strBuffer = strBuffer.Mid(1);
				if(strBuffer.IsEmpty() || (nMode != 0)){

					continue;
				}

				CString strEvent, strResp;
				
				CTokenizer token(strBuffer, "�");

				if(!token.Next(strEvent) || !token.Next(strResp)) continue;
					
				if(nMode == 0){
				
					
					strEvent.MakeLower();
					if(strResp.GetLength() < 8){

						strResp = GetWorkingDir() + "\\" + strResp;
					}
					else if(strResp.GetAt(1) != ':'){

						strResp = GetWorkingDir() + "\\" + strResp;
					}

					SOUND s;
					s.strTrigger = strEvent;
					s.strSound = strResp;
					m_aSounds.Add(s);

				}
				else{

					TRACE("HELP\n");

				}

			}
		}
		ini.Close();
	}
	CATCH(CFileException, e){

		AfxMessageBox(IDS_ERROR_FILE_GENERIC, MB_OK+MB_ICONSTOP);

	}END_CATCH;
}

