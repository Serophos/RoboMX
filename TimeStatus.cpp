/*
** Copyright (C) 2002 Thees Winkler
**  
** Parts of this codes are based on code from CDEx (c) 1999-2002 by Albert L. Faber 
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "stdafx.h"
#include "stdafx.h"
#include "TimeStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CTimeStatus::CTimeStatus()
{

	ReInit();
}

CTimeStatus::~CTimeStatus()
{

}

void CTimeStatus::ReInit()
{

	m_dwStartTime=GetTickCount();
	m_dwElapsed=0;
	m_dwEstimated=0;
	m_dwRemaining=0;
	m_nPrevPercent=0;
}


CString CTimeStatus::GetElapsedString()
{

	CString strTmp;
	strTmp.Format(_T("%02d:%02d"),m_dwElapsed/60,m_dwElapsed%60);
	return strTmp;
}

CString CTimeStatus::GetEstimateString()
{

	CString strTmp;
	strTmp.Format(_T("%02d:%02d"),m_dwEstimated/60,m_dwEstimated%60);
	return strTmp;
}

CString CTimeStatus::GetRemainingString()
{

	CString strTmp;
	strTmp.Format(_T("%02d:%02d"),m_dwRemaining/60,m_dwRemaining%60);
	return strTmp;
}


void CTimeStatus::Calculate(int nPercent)
{

	m_dwElapsed=(GetTickCount()-m_dwStartTime)/1000;

	// Update every two percent
	if ((nPercent-m_nPrevPercent)>=3)
	{
		if (nPercent)
		{
			m_dwEstimated=(DWORD)max(0,m_dwElapsed*100.0/nPercent);
			m_nPrevPercent=nPercent;
		}
	}
	m_dwEstimated=(DWORD)max(m_dwEstimated, m_dwElapsed);
	m_dwRemaining=(DWORD)max(0,(double)m_dwEstimated-(double)m_dwElapsed);
}

