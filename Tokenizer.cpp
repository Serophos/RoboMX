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

#include "StdAfx.h"
#include "Tokenizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTokenizer::CTokenizer(const CString& str, const CString& strDelim):
	m_str(str),
	m_nCurPos(0)
{
	SetDelimiters(strDelim);
}

void CTokenizer::SetDelimiters(const CString& strDelim)
{
	for(int i = 0; i < strDelim.GetLength(); ++i){

		m_bsDelim.set(static_cast<BYTE>(strDelim[i]));
	}
}

BOOL CTokenizer::Next(CString& str)
{
	str.Empty();

	while((m_nCurPos < m_str.GetLength()) && m_bsDelim[static_cast<BYTE>(m_str[m_nCurPos])]){

		++m_nCurPos;
	}

	if(m_nCurPos >= m_str.GetLength()){

		return FALSE;
	}

	int nStartPos = m_nCurPos;
	while((m_nCurPos < m_str.GetLength()) && !m_bsDelim[static_cast<BYTE>(m_str[m_nCurPos])]){

		++m_nCurPos;
	}
	
	str = m_str.Mid(nStartPos, m_nCurPos - nStartPos);

	return TRUE;
}

CString	CTokenizer::Tail() const
{
	int nCurPos = m_nCurPos;

	while((nCurPos < m_str.GetLength()) && m_bsDelim[static_cast<BYTE>(m_str[nCurPos])]){

		++nCurPos;
	}

	CString strResult;

	if(nCurPos < m_str.GetLength()){

		strResult = m_str.Mid(nCurPos);
	}

	return strResult;
}
