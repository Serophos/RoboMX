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

#if !defined(AFX_TIMESTATUS_H__0FEB5732_F4C3_4DC3_B24F_C3C018AB4919__INCLUDED_)
#define AFX_TIMESTATUS_H__0FEB5732_F4C3_4DC3_B24F_C3C018AB4919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTimeStatus  
{
public:
	CTimeStatus();
	virtual ~CTimeStatus();

	DWORD	m_dwStartTime;
	DWORD	m_dwElapsed;
	DWORD	m_dwEstimated;
	DWORD	m_dwRemaining;
	int		m_nPrevPercent;

	void ReInit();
	CString GetElapsedString();
	CString GetEstimateString();
	CString GetRemainingString();
	void	Calculate(int nPercent);

};

#endif // !defined(AFX_TIMESTATUS_H__0FEB5732_F4C3_4DC3_B24F_C3C018AB4919__INCLUDED_)
