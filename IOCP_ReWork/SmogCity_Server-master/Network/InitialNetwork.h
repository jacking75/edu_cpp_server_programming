
#pragma once

#ifndef __INITIALNETWORK_H__
#define __INITIALNETWORK_H__

#include "../Common/Common.h"

static bool START_NETWORK()
{
	WSADATA	wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "[WSAStartup() Error!]\n";
		return false;
	}

	std::wcout.imbue(std::locale("korean"));

	return true;
}

static bool END_NETWORK()
{
	if (WSACleanup() == SOCKET_ERROR) {
		std::cout << "[WSACleanup() Error!]\n";
		return false;
	}

	return true;
}

static void err_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	std::cout << msg;
	std::wcout << L"¿¡·¯ [" << err_no << L"]" << lpMsgBuf << "\n";

	while (true);
	LocalFree(lpMsgBuf);
}

#endif	// __INITIALNETWORK_H__