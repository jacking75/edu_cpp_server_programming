
#pragma once

#ifndef __LIB_SERVERSYSTEM_H__
#define __LIB_SERVERSYSTEM_H__

#include "Common.h"

namespace COMMONLIB
{
	const TCHAR NAME_TESTSERVER[MAX_STRING]	= _T("TestServer");

	class LIB_SERVERINFO
	{
	public :
		LIB_SERVERINFO()
		{
			dwServerID		= 0;
			dwWorldID		= 0;
			dwSession		= 0;
			dwTCPPort		= 0;
			dwUDPPort		= 0;
			dwServerType	= 0;
            ZeroMemory(&szServerIP, MIN_STRING);
			ZeroMemory(&szServerName, MIN_STRING);
		}
		~LIB_SERVERINFO()	{}
	public :
		DWORD	dwServerID;
		DWORD	dwWorldID;
		DWORD	dwSession;
		DWORD	dwTCPPort;
		DWORD	dwUDPPort;
		DWORD	dwServerType;
		TCHAR	szServerIP[MIN_STRING];
		TCHAR	szServerName[MIN_STRING];
	};
}

#endif	// __LIB_SERVERSYSTEM_H__