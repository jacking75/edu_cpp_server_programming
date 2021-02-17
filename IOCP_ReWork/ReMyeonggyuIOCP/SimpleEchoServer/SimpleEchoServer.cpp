// SimpleEchoServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <WinSock2.h>
#include "../MGServerLib/AsynchIOService.h"
#include "MGServerReceiver.h"
#include "../MGServerLib/Acceptor.h"

#pragma comment(lib, "ws2_32.lib") 
//#pragma comment(lib, "MyServerLib.lib") 

int _tmain(int argc, _TCHAR* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	WSADATA wsaData;
	WORD wVer = MAKEWORD(2,2);    
	if (WSAStartup(wVer,&wsaData) != NO_ERROR)
		return 0;

	if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return 0;
	}

	MGServerReceiver Receiver;
	AsynchIOService MGServer(&Receiver, 1024, 5, "MGServer");

	AsynchSocket* pProtoType = AsynchSocket::GetASPrototype();
	Acceptor MGAcceptor(&MGServer, pProtoType, "127.0.0.1", 23452);
	MGServer.start();
	MGAcceptor.Start();

	while(1)
	{
		Sleep(1000);
		Receiver.Process();
	}

	WSACleanup();

	return 0;
}

