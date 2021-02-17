
#pragma once

#ifndef __INITIALNETWORK_H__
#define __INITIALNETWORK_H__

#include "../Common/Common.h"

namespace NETWORKLIB
{
	static BOOL START_NETWORK()
	{
		WSADATA	wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return FALSE;

		return TRUE;
	}

	static BOOL END_NETWORK()
	{
		if (WSACleanup() == SOCKET_ERROR)
			return FALSE;

		return TRUE;
	}
}

#endif	// __INITIALNETWORK_H__