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

#if !defined(AFX_SETTINGS_H__25ADA47A_C2DF_42B2_B3AE_6C12BA658D11__INCLUDED_)
#define AFX_SETTINGS_H__25ADA47A_C2DF_42B2_B3AE_6C12BA658D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct TAG_MXSOUND{

	CString strTrigger;
	CString strSound;
} SOUND, *PSOUND;


class CSettings  
{
public:
	int GetCharSet(BOOL bIndexOnly = FALSE);
	void SetCharSet(int nValue);
	void Save();
	void Load();
	CSettings();
	virtual ~CSettings();

	// Set Functions
	void    SetLogDir(CString strValue){ m_strLogDir = strValue; }
	void	SetLog(BOOL bValue){m_bLog = bValue; }
	void    SetEnableScroller(BOOL bValue){ m_bScroller = bValue; }
	void	SetMessageItalic(BOOL bValue){ m_bMsItalic = bValue; }
	void	SetMessageLine(BOOL bValue){ m_bMsLine = bValue; }
	void	SetMessageBold(BOOL bValue){ m_bMsBold = bValue; }
	void	SetPrintTime(BOOL bValue){ m_bTime = bValue; }
	void    SetHistoryDepth(int nValue){ m_nDepth = nValue; }
	void    SetSaveHistory(int nValue){ m_bHistory = nValue; }
	void    SetJoin(CString strValue){ m_strJoin = strValue; }
	void    SetPart(CString strValue){ m_strPart = strValue; }
	void	SetBlockHttp(BOOL bValue){ m_bBlockHttp = bValue; }
	void	SetBlockMotd(BOOL bValue){ m_bBlockMotd = bValue; }
	void	SetBlockPrivate(BOOL bValue){ m_bBlockPrivate = bValue; }
	void	SetBlockTopic(BOOL bValue){ m_bBlockTopic = bValue; }
	void	SetBlockEmpty(BOOL bValue){ m_bBlockEmpty = bValue; }
	void	SetBlockAscii(BOOL bValue){ m_bBlockAscii = bValue; }
	void	SetBlockAdmin(BOOL bValue){ m_bBlockAdmin = bValue; }
	void	SetFilterXtrem(BOOL bValue){ m_bFilterXtrem = bValue; }
	void	SetFilterIgnore(BOOL bValue){ m_bFilterIgnore = bValue; }
	void	SetFilterNormal(BOOL bValue){ m_bFilterNormal = bValue; }
	void	SetEmoticons(BOOL bValue){ m_bEmoticons = bValue; }
	void	SetHiliteUsers(BOOL bValue){ m_bHiliteUsers = bValue; }
	void    SetHideSystem(BOOL bValue){ m_bHideSystem = bValue; }
	void	SetTopic(CString strValue){ m_strTopic = strValue; }
	void	SetMotd(CString strValue){ m_strMotd = strValue; }
	void	SetKeyMsg(CString strValue){ m_strKeyMsg = strValue; }
	void	SetGodName(CString strValue){ m_strGodName = strValue; }
	void	SetGodFiles(DWORD dwValue){ m_dwGodFiles = dwValue; }
	void	SetGodLine(WORD wValue){ m_wGodLine = wValue; }
	void	SetGodVisible(BOOL bValue){ m_bGodVisible = bValue; }
	void	SetModerated(BOOL bValue){ m_bModerated = bValue; }
	void	SetLimit(DWORD dwValue){ m_dwLimit = dwValue; }
	void	SetMultiIPOk(BOOL bValue){ m_bMultiIPOk = bValue; }
	void	SetBlockNushi(BOOL bValue){ m_bBLockNushi = bValue; }
	void	SetLocalIsOp(BOOL bValue){ m_bLocalIsOp = bValue; }

	void	SetDefaulStyle(DWORD dwValue){ m_dwTextStyle = dwValue; }
	void	SetRGBNormalName(COLORREF crValue){ m_crNick = crValue; }
	void	SetRGBNormalMsg(COLORREF crValue){ m_crMessage = crValue; }
	void	SetRGBActionMsg(COLORREF crValue){ m_crAction = crValue; }
	void	SetRGBJoin(COLORREF crValue){ m_crJoin = crValue; }
	void	SetRGBPart(COLORREF crValue){ m_crPart = crValue; }
	void	SetRGBTopic(COLORREF crValue){ m_crTopic = crValue; }
	void	SetRGBMotd(COLORREF crValue){ m_crMotd = crValue; }
	void	SetRGBBrMessage(COLORREF crValue){ m_crNickBr = crValue; }
	void	SetRGBBrAction(COLORREF crValue){ m_crActionBr = crValue; }
	void	SetRGBBg(COLORREF crValue){ m_crBg = crValue; }
	void	SetRGBFiles(COLORREF crValue){  m_crFiles = crValue; }
	void	SetRGBLine(COLORREF crValue){  m_crLine = crValue; }
	void	SetRGBIP(COLORREF crValue){  m_crIP = crValue; }
	void	SetRGBPort(COLORREF crValue){  m_crPort = crValue; }
	void	SetRGBPend(COLORREF crValue){  m_crPend = crValue; }
	void	SetRGBOK(COLORREF crValue){  m_crOK = crValue; }
	void	SetRGBErr(COLORREF crValue){  m_crErr = crValue; }
	void	SetRGBFocus(COLORREF crValue){ m_crFocus = crValue; }
	void    SetRGBTime(COLORREF crValue){ m_crTime = crValue; }
	void    SetRGBHiLite(COLORREF crValue){ m_crHiLite = crValue; }
	void	SetRGBDocHiLite(COLORREF crValue){ m_crDocHiLite = crValue; }
	void	SetRGBOp(COLORREF crValue){m_crOp = crValue; }

	void    SetUseImage(BOOL bValue){ m_bImage = bValue; }
	void	SetImage(CString strValue){ m_strImage = strValue; }

	void	SetWorkingDir(CString strValue){ m_strWd = strValue; }
	void	SetBrActionFront(CString strValue){ m_FA = strValue; }
	void	SetBrMsgFront(CString strValue){ m_FM = strValue; }
	void	SetBrActionEnd(CString strValue){ m_EA = strValue; }
	void	SetBrMsgEnd(CString strValue){ m_EM = strValue; }
	void	SetFont(CString strValue){ m_strFont = strValue; }
	void    SetFontSize(int nValue){ m_nFontSize = nValue; }

	void	SetFiles(DWORD dwValue){m_dwFiles = dwValue; }
	void	SetLine(WORD wValue){ m_wLine = wValue; }
	void    SetNickname(CString strValue){m_strNickname = strValue; }
	void	SetWinampMsg(CString strValue){ m_strWinampMsg = strValue; }
	void	SetVideoMsg(CString strValue){ m_strVideoMsg = strValue; }
	void	SetEnterMsg(CString strValue){ m_strEnterMsg = strValue; }
	void	SetDoEnterMsg(BOOL bValue){ m_bOnEnter = bValue; }
	void	SetLanguage(CString strValue){ m_strLanguage = strValue; }
	void	SetColorAcName(BOOL bValue){ m_bColorAcName = bValue; }
	void    SetMiniTray(BOOL bValue){ m_bMiniTray = bValue; }
	void	SetPing(BOOL bValue){ m_bPing = bValue; }
	void	SetTimeFmt(int nValue){ m_nTimeFormat = nValue; }
	void	SetFocus(BOOL bValue){ m_bFocus = bValue; }
	void	SetUpdate(BOOL bValue){ m_bUpdate = bValue; }
	void	SetAutoList(BOOL bValue){ m_bAutoList = bValue; }
	void	SetMaxi(BOOL bValue){ m_bMaxi = bValue; }
	void	SetBarTop(BOOL bValue){ m_bBarTop = bValue; }
	void	SetRetry(BOOL bValue){ m_bRetry = bValue; }
	void	SetRetries(BOOL bValue){ m_nRetries = bValue; }

	// Serve shit
	void	SetNodeIP(DWORD dwValue){  m_dwNodeIP = dwValue; }
	void	SetNodePort(DWORD dwValue){  m_dwNodePort = dwValue; }

	// Sound stuff
	void    SetSfxJoin(CString strValue){ m_strSfxJoin = strValue; }
	void	SetSfxPart(CString strValue){ m_strSfxPart = strValue; }
	void	SetSfxRedirect(CString strValue){ m_strSfxRedirect = strValue; }
	void	SetSfxTopic(CString strValue){ m_strSfxTopic = strValue; }
	void	SetSfxMotd(CString strValue){ m_strSfxMotd = strValue; }
	void	SetSfxStart(CString strValue){ m_strSfxStart = strValue; }
	void	SetSfxError(CString strValue){ m_strSfxError = strValue; }
	void	SetSfxChatSfx(BOOL bValue){ m_bChatSfx = bValue; }
	void	SetSoundFX(BOOL bValue){ m_bSoundFX = bValue; }

	// Access functions
    CString GetWorkingDir(BOOL bCached = TRUE);
	CString GetLogDir(){return m_strLogDir;}
	BOOL    GetLog(){return m_bLog;}
	CString GetLanguage(){ return m_strLanguage; }
	BOOL    GetUseImage(){ return m_bImage; }
	CString GetImage(){ return m_strImage; }
	BOOL	GetBlockHttp(){ return m_bBlockHttp; }
	BOOL	GetBlockMotd(){ return m_bBlockMotd; }
	BOOL	GetBlockPrivate(){ return m_bBlockPrivate; }
	BOOL	GetBlockTopic(){ return m_bBlockTopic; }
	BOOL	GetBlockEmpty(){ return m_bBlockEmpty; }
	BOOL	GetBlockAscii(){ return m_bBlockAscii; }
	BOOL	GetBlockAdmin(){ return m_bBlockAdmin; }
	BOOL	GetFilterXtrem(){ return m_bFilterXtrem; }
	BOOL	GetFilterIgnore(){ return m_bFilterIgnore; }
	BOOL	GetFilterNormal(){ return m_bFilterNormal; }
	BOOL	GetEmoticons(){ return m_bEmoticons; }
	BOOL	GetHiliteUsers(){ return m_bHiliteUsers; }
	DWORD	GetFiles(){ return m_dwFiles; }
	WORD	GetLine(){ return m_wLine; }
	BOOL	GetDisplayNode(){ return m_bDisplayNode; }
	BOOL	GetHideSystem(){ return m_bHideSystem; }
	BOOL	GetBarTop(){ return m_bBarTop; }
 	BOOL	GetRetry(){ return m_bRetry; }
	int 	GetRetries(){ return m_nRetries; }

	CString    GetTopic(){ return m_strTopic; }
	CString    GetMotd(){ return m_strMotd; }
	CString    GetKeyMsg(){ return m_strKeyMsg; }
	CString    GetGodName(){ return m_strGodName; }
	DWORD	   GetGodFiles(){ return m_dwGodFiles; }
	WORD	   GetGodLine(){ return m_wGodLine; }
	BOOL       GetGodVisible(){ return m_bGodVisible; }
	BOOL       GetModerated(){ return m_bModerated; }
	DWORD	   GetLimit(){ return m_dwLimit; }
	BOOL       GetMultiIPOk(){ return m_bMultiIPOk; }
	BOOL       GetBlockNushi(){ return m_bBLockNushi; }
	BOOL       GetLocalIsOp(){ return m_bLocalIsOp; }

	BOOL	GetMessageItalic(){ return m_bMsItalic; }
	BOOL	GetMessageLine(){ return m_bMsLine; }
	BOOL	GetMessageBold(){ return m_bMsBold; }
	BOOL	GetPrintTime(){ return m_bTime; }
	int     GetHistoryDepth(){ return m_nDepth; }
	int     GetSaveHistory(){ return m_bHistory; }
	CString GetNickname(){ return m_strNickname; }
	BOOL	GetFocus(){ return m_bFocus; }

	CString GetJoin(BOOL bReturn = FALSE){ 
	
		CString strTmp = m_strJoin;
		if(bReturn) strTmp.Replace("\\n", "\r\n");
		return strTmp; 
	}
	CString GetPart(BOOL bReturn = FALSE){ 
		
		CString strTmp = m_strPart;
		if(bReturn) strTmp.Replace("\\n", "\r\n");
		return strTmp; 
	}

	DWORD		GetDefaultStyle(){return m_dwTextStyle; }
	COLORREF	GetRGBTime(){ return m_crTime; }
	COLORREF	GetRGBNormalName(){ return m_crNick; }
	COLORREF	GetRGBNormalMsg(){ return m_crMessage; }
	COLORREF	GetRGBActionMsg(){ return m_crAction; }
	COLORREF	GetRGBJoin(){ return m_crJoin; }
	COLORREF	GetRGBPart(){ return m_crPart; }
	COLORREF	GetRGBTopic(){ return m_crTopic; }
	COLORREF	GetRGBMotd(){ return m_crMotd; }
	COLORREF	GetRGBBrMessage(){ return m_crNickBr; }
	COLORREF	GetRGBBrAction(){ return m_crActionBr; }
	COLORREF	GetRGBBg(){ return m_crBg; }
	COLORREF	GetRGBFiles(){ return m_crFiles; }
	COLORREF	GetRGBLine(){ return m_crLine; }
	COLORREF	GetRGBIP(){ return m_crIP; }
	COLORREF	GetRGBPort(){ return m_crPort; }
	COLORREF	GetRGBPend(){ return m_crPend; }
	COLORREF	GetRGBOK(){ return m_crOK; }
	COLORREF	GetRGBErr(){ return m_crErr; }
	COLORREF	GetRGBFocus(){ return m_crFocus; }
	COLORREF	GetRGBHiLite(){ return m_crHiLite; }
	COLORREF	GetRGBDocHiLite() { return m_crDocHiLite; }
	COLORREF	GetRGBOp(){return m_crOp; }

	CString		GetBrActionFront(){ return m_FA; }
	CString		GetBrMsgFront(){ return m_FM; }
	CString		GetBrActionEnd(){ return m_EA; }
	CString		GetBrMsgEnd(){ return m_EM; }
	CString		GetFont(){ return m_strFont; }
	int         GetFontSize(){ return m_nFontSize; }
	
	BOOL        GetEnableScroller(){ return m_bScroller; }
	CString		GetWinampMsg(){ return m_strWinampMsg; }
	CString		GetVideoMsg(){ return m_strVideoMsg; }
	CString		GetEnterMsg(){ return m_strEnterMsg; }
	BOOL		GetDoEnterMsg(){ return m_bOnEnter; }
	BOOL		GetColorAcName(){ return m_bColorAcName; }
	BOOL		GetMiniTray(){ return m_bMiniTray; }
	BOOL		GetPing(){ return m_bPing; }
	int			GetTimeFmt(){ return m_nTimeFormat; }
	BOOL		GetUpdate(){ return m_bUpdate; }
	BOOL		GetAutoList(){ return m_bAutoList; }
	BOOL		GetMaxi(){ return m_bMaxi; }

	// Sound stuff
	CString     GetSfxJoin(){ return m_strSfxJoin; }
	CString	    GetSfxPart(){ return m_strSfxPart; }
	CString		GetSfxRedirect(){ return m_strSfxRedirect; }
	CString		GetSfxTopic(){ return m_strSfxTopic; }
	CString		GetSfxMotd(){ return m_strSfxMotd; }
	CString		GetSfxStart(){ return m_strSfxStart; }
	CString		GetSfxError(){ return m_strSfxError; }
	BOOL		GetSfxChatSfx(){ return m_bChatSfx; }
	BOOL		GetSoundFX(){ return m_bSoundFX; }

	DWORD      GetNodeIP(){ return m_dwNodeIP; }
	DWORD	   GetNodePort(){ return m_dwNodePort; }

// Loading stuff
	void LoadRooms(void);
	void LoadSounds(void);
	void LoadRCMS(void);
	void LoadQuickCmds(void);

public: // public attributes
	CStringArray m_aHilite;
	CStringArray m_aIgnored;
	CStringArray m_aRooms;
	CStringArray m_aGreetings;
	CStringArray m_aQuick;
	CStringArray m_aRCMSCommands;
	CStringArray m_aWinMXCommands;
	CStringArray m_aRoboMXCommands;
	CArray<SOUND, SOUND> m_aSounds;


protected:
	CString m_strWd;
	CString m_strLogDir;

	BOOL    m_bLog;
	BOOL	m_bMsItalic;
	BOOL	m_bMsLine;
	BOOL	m_bMsBold;
	BOOL	m_bTime;
	BOOL	m_bOnEnter;
	BOOL    m_bColorAcName;
	BOOL    m_bPing;
	int     m_nFontSize;
	int		m_nCharset;
	int     m_nDepth;//   = NULL;
	int     m_bHistory;// = TRUE;
	int     m_nTimeFormat;
	
	BOOL	m_bBlockHttp;
	BOOL	m_bBlockMotd;
	BOOL	m_bBlockPrivate;
	BOOL	m_bBlockTopic;
	BOOL	m_bBlockEmpty;
	BOOL	m_bBlockAscii;
	BOOL	m_bBlockAdmin;
	BOOL	m_bFilterXtrem;
	BOOL	m_bFilterIgnore;
	BOOL	m_bFilterNormal;
	BOOL	m_bImage;
	BOOL    m_bMiniTray;
	BOOL	m_bFocus;
	BOOL	m_bHiliteUsers;
	BOOL	m_bEmoticons;
	BOOL	m_bUpdate;
	BOOL	m_bAutoList;
	BOOL	m_bMaxi;
	BOOL    m_bScroller;
	BOOL	m_bDisplayNode;
	BOOL    m_bHideSystem;
	BOOL    m_bBarTop;
	BOOL	m_bRetry;
	int		m_nRetries;

	COLORREF	m_crFocus;
	COLORREF	m_crNick;
	COLORREF	m_crMessage;
	COLORREF	m_crAction;
	COLORREF	m_crJoin;
	COLORREF	m_crPart;
	COLORREF	m_crTopic;
	COLORREF	m_crMotd;
	COLORREF	m_crNickBr;
	COLORREF	m_crActionBr;
	COLORREF	m_crFiles;
	COLORREF	m_crLine;
	COLORREF	m_crIP;
	COLORREF	m_crPort;
	COLORREF	m_crPend;
	COLORREF	m_crOK;
	COLORREF	m_crErr;
	COLORREF	m_crBg;
	COLORREF    m_crTime;
	COLORREF    m_crHiLite;
	COLORREF	m_crDocHiLite;
	COLORREF    m_crOp;
	CString		m_FA;
	CString		m_FM;
	CString		m_EA;
	CString		m_EM;
	CString		m_strFont;
	CString		m_strEnterMsg;
	CString		m_strWinampMsg;
	CString		m_strVideoMsg;
	CString     m_strLanguage;
	CString	    m_strJoin;
	CString		m_strNickname;
	CString     m_strPart;
	CString     m_strImage;

	DWORD		m_dwFiles;
	WORD		m_wLine;

	// Sound stuff
	CString     m_strSfxJoin;
	CString	    m_strSfxPart;
	CString		m_strSfxRedirect;
	CString		m_strSfxTopic;
	CString		m_strSfxMotd;
	CString		m_strSfxStart;
	CString		m_strSfxError;
	BOOL		m_bChatSfx;
	BOOL		m_bSoundFX;
		
	DWORD      m_dwNodeIP;
	DWORD	   m_dwNodePort;
	
	// Server shit
	CString    m_strTopic;
	CString    m_strMotd;
	CString    m_strKeyMsg;
	CString    m_strGodName;
	DWORD	   m_dwGodFiles;
	WORD	   m_wGodLine;
	BOOL       m_bGodVisible;
	BOOL       m_bModerated;
	DWORD	   m_dwLimit;
	BOOL       m_bMultiIPOk;
	BOOL       m_bBLockNushi;
	BOOL       m_bLocalIsOp;

	DWORD	   m_dwTextStyle;
public:
	void LoadHiLite(void);
};

#endif // !defined(AFX_SETTINGS_H__25ADA47A_C2DF_42B2_B3AE_6C12BA658D11__INCLUDED_)
