/*
** Copyright (C) 2002 Thees Winkler
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
#include "ping.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_ICMP_PACKET_SIZE 8    //minimum 8 byte icmp packet (just header)
#define MAX_ICMP_PACKET_SIZE 1024 //Maximum icmp packet size

BOOL				CPing::sm_bAttemptedIcmpInitialise	= FALSE;
lpIcmpCreateFile	CPing::sm_pIcmpCreateFile			= NULL;
lpIcmpSendEcho		CPing::sm_pIcmpSendEcho				= NULL;
lpIcmpCloseHandle	CPing::sm_pIcmpCloseHandle			= NULL;
__int64				CPing::sm_TimerFrequency			= 0;


class _CPING
{
public:
	_CPING();
	~_CPING();

protected:

	HINSTANCE sm_hIcmp;
	friend class CPing;
};

_CPING::_CPING()
{

  sm_hIcmp = NULL;
}

_CPING::~_CPING()
{

	if (sm_hIcmp){

		FreeLibrary(sm_hIcmp);
		sm_hIcmp = NULL;
	}
}

static _CPING _cpingData;

BOOL CPing::Initialise() const
{

	if(!sm_bAttemptedIcmpInitialise){

		sm_bAttemptedIcmpInitialise = TRUE;

		//Load up the ICMP library
		_cpingData.sm_hIcmp = LoadLibrary("ICMP.DLL");
		if(_cpingData.sm_hIcmp == NULL){

			TRACE("Could not load up the ICMP DLL\n");
			return FALSE;
		}

		//Retrieve pointers to the functions in the ICMP dll
		sm_pIcmpCreateFile  = (lpIcmpCreateFile)GetProcAddress(_cpingData.sm_hIcmp, "IcmpCreateFile");
		sm_pIcmpSendEcho	= (lpIcmpSendEcho)GetProcAddress(_cpingData.sm_hIcmp, "IcmpSendEcho" );
		sm_pIcmpCloseHandle = (lpIcmpCloseHandle)GetProcAddress(_cpingData.sm_hIcmp, "IcmpCloseHandle");

		if((sm_pIcmpCreateFile == NULL) ||( sm_pIcmpSendEcho == NULL) || (sm_pIcmpCloseHandle == NULL)){

		  TRACE(_T("Could not find ICMP functions in the ICMP DLL\n"));
		}
	}

	return ((sm_pIcmpCreateFile != NULL) && (sm_pIcmpSendEcho != NULL) &&	(sm_pIcmpCloseHandle != NULL));
}

BOOL CPing::Ping(LPCTSTR pszHostName, CPingReply& pr, UCHAR nTTL, DWORD dwTimeout, UCHAR nPacketSize) const
{

	USES_CONVERSION;

	if (!Initialise()) return FALSE;

	ASSERT(_cpingData.sm_hIcmp); //ICMP dll must be open

	LPSTR lpszAscii = T2A((LPTSTR) pszHostName);

	//Convert from dotted notation if required
	unsigned long	addr = inet_addr(lpszAscii);
	if(addr == INADDR_NONE){

		//Not a dotted address, then do a lookup of the name
		hostent* hp = gethostbyname(lpszAscii);
		if(hp){

			memcpy(&addr, hp->h_addr, hp->h_length);
		}
		else{

			TRACE(_T("Could not resolve the host name %s\n"), pszHostName);
			return FALSE;
		}
	}

	//Create the ICMP handle
	HANDLE hIP = sm_pIcmpCreateFile();
	if(hIP == INVALID_HANDLE_VALUE){

		TRACE(_T("Could not get a valid ICMP handle\n"));
		return FALSE;
	}

	//Set up the option info structure
	IP_OPTION_INFORMATION OptionInfo;
	ZeroMemory(&OptionInfo, sizeof(IP_OPTION_INFORMATION));
	OptionInfo.Ttl = nTTL;

	//Set up the data which will be sent
	unsigned char* pBuf = new unsigned char[nPacketSize];
	memset(pBuf, 'E', nPacketSize);

	//Do the actual Ping
	int nReplySize = sizeof(ICMP_ECHO_REPLY) + max(MIN_ICMP_PACKET_SIZE, nPacketSize);
	unsigned char* pReply = new unsigned char[nReplySize];
	ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)pReply;
	
	DWORD nRecvPackets = sm_pIcmpSendEcho(hIP, addr, pBuf, nPacketSize, &OptionInfo, pReply, nReplySize, dwTimeout);

	//Check we got the packet back
	BOOL bSuccess = (nRecvPackets == 1);

	//Check the IP status is OK (O is IP Success)
	if(bSuccess && (pEchoReply->Status != 0)){

		bSuccess = FALSE;
		SetLastError(pEchoReply->Status);
	}

	//Check we got the same amount of data back as we sent
	if(bSuccess){

		bSuccess = (pEchoReply->DataSize == nPacketSize);
		if(!bSuccess){

			SetLastError(ERROR_UNEXP_NET_ERR);
		}
	}

	//Check the data we got back is what was sent
	if(bSuccess) {

		char* pReplyData = (char*) pEchoReply->Data;
		for(int i = 0; i < nPacketSize && bSuccess; i++){

			bSuccess = (pReplyData[i] == 'E');
		}

		if(!bSuccess){

			SetLastError(ERROR_UNEXP_NET_ERR);
		}
	}

	//Close the ICMP handle
	sm_pIcmpCloseHandle(hIP);

	if(bSuccess){

		//Ping was successful, copy over the pertinent info
		//into the return structure
		pr.Address.S_un.S_addr = pEchoReply->Address;
		pr.RTT = pEchoReply->RoundTripTime;
	}

	delete [] pBuf;
	delete [] pReply;

	return bSuccess; 
}


