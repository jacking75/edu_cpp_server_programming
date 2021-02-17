
#pragma once

#ifndef __LIB_WINSOCKERRCODE_H__
#define __LIB_WINSOCKERRCODE_H__

namespace NETWORKLIB
{
	struct WinSockErrorCodeTable
	{
		int code;
		const TCHAR* str;
	};

	WinSockErrorCodeTable g_ErrTable[] = 
	{
		{ WSAEINTR,					"Interrupted function call" },
		{ WSAEACCES,				"Permission denied" },
		{ WSAEFAULT,				"Bad address" },
		{ WSAEINVAL,				"Invalid argument" },
		{ WSAEMFILE,				"Too many open files" },
		{ WSAEWOULDBLOCK,			"Socket would block" },
		{ WSAEINPROGRESS,			"Operation now in progress" },
		{ WSAEALREADY,				"Operation already in progress" },
		{ WSAENOTSOCK,				"Socket operation on nonsocket" },
		{ WSAEDESTADDRREQ,			"Destination address required" },
		{ WSAEMSGSIZE,				"Message too long" },
		{ WSAEPROTOTYPE,			"Protocol wrong type for socket" },
		{ WSAENOPROTOOPT,			"Bad protocol option" },
		{ WSAEPROTONOSUPPORT,		"Protocol not supported" },
		{ WSAESOCKTNOSUPPORT,		"Socket type not supported" },
		{ WSAEOPNOTSUPP,			"Operation not supported" },
		{ WSAEPFNOSUPPORT,			"Protocol family not supported" },
		{ WSAEAFNOSUPPORT,			"Address family not supported by protocol family" },
		{ WSAEADDRINUSE,			"Address already in use" },
		{ WSAEADDRNOTAVAIL,			"Cannot assign requested address" },
		{ WSAENETDOWN,				"Network is down" },
		{ WSAENETUNREACH,			"Network is unreachable" },
		{ WSAENETRESET,				"Network dropped connection on reset" },
		{ WSAECONNABORTED,			"Software caused connection abort" },
		{ WSAECONNRESET,			"Connection reset by peer" },
		{ WSAENOBUFS,				"No buffer space available" },
		{ WSAEISCONN,				"Socket is already connected" },
		{ WSAENOTCONN,				"Socket is not connected" },
		{ WSAESHUTDOWN,				"Cannot send after socket shutdown" },
		{ WSAETIMEDOUT,				"Connection timed out" },
		{ WSAECONNREFUSED,			"Connection refused" },
		{ WSAEHOSTDOWN,				"Host is down" },
		{ WSAEHOSTUNREACH,			"No route to host" },
		{ WSAEPROCLIM,				"Too many processes" },
		{ WSASYSNOTREADY,			"Network subsystem is unavailable" },
		{ WSAVERNOTSUPPORTED,		"Winsock.dll version out of range" },
		{ WSANOTINITIALISED,		"Successful WSAStartup not yet performed" },
		{ WSAEDISCON,				"Graceful shutdown in progress" },
		{ WSATYPE_NOT_FOUND,		"Class type not found" },
		{ WSAHOST_NOT_FOUND,		"Host not found" },
		{ WSATRY_AGAIN,				"Nonauthoritative host not found" },
		{ WSANO_RECOVERY,			"This is a nonrecoverable error" },
		{ WSANO_DATA,				"Valid name, no data record of requested type" },
		{ WSA_INVALID_HANDLE,		"Specified event object handle is invalid" },
		{ WSA_INVALID_PARAMETER,	"One or more parameters are invalid" },
		{ WSA_IO_INCOMPLETE,		"Overlapped I/O event object not in signaled state" },
		{ WSA_IO_PENDING,			"Overlapped operations will complete later" },
		{ WSA_NOT_ENOUGH_MEMORY,	"Insufficient memory available" },
		{ WSA_OPERATION_ABORTED,	"Overlapped operation aborted" },
		//	{ WSAINVALIDPROCTABLE,		"Invalid procedure table from service provider" },
		//	{ WSAINVALIDPROVIDER,		"Invalid service provider version number" },
		//	{ WSAPROVIDERFAILEDINIT,	"Unable to initialize a service provider" },
		{ WSASYSCALLFAILURE,		"System call failure" }
	};

	/*
	 *	Example for use :	const TCHAR* pp = GetStringErrCode(WSAEOPNOTSUPP);
	*/
	const TCHAR* GetStringErrCode(const int nCode)
	{
		for ( int iter = 0; iter < 50; iter++ )
		{
			if ( g_ErrTable[iter].code == nCode )
				return g_ErrTable[iter].str;
		}

		return NULL;
	}
}

#endif	// __LIB_WINSOCKERRCODE_H__