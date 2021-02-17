// SimpleEchoClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include "../MGServerLib/AsynchIOService.h"
#include "MGClientReceiver.h"
#include "../MGServerLib/Acceptor.h"

#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "MyServerLib.lib") 

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	WORD wVer = MAKEWORD(2,2);    
	if (WSAStartup(wVer,&wsaData) != NO_ERROR)
		return 0;

	if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return 0;
	}

	MGClientReceiver Receiver;
	AsynchIOService MGService(&Receiver, 1024, 1, "MGServer");

	AsynchSocket* pProtoType = AsynchSocket::GetASPrototype();
	MGService.connectSocket(1, pProtoType->clone(), "127.0.0.1", 3210);
	MGService.start();

	while(1)
	{
		Sleep(1000);
		Receiver.ProcessEcho();
	}

	WSACleanup();

	return 0;
}
