// Smog_City_Server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "stdafx.h"
#include "ServerManager.h"

CServerManager g_ServerManager;

int main()
{
	if (g_ServerManager.StartServer() == false) {
		std::cout << "StartServer Fail in main\n";
	}

	if (g_ServerManager.StopServer() == false) {
		std::cout << "StopServer Fail in main\n";
	}

	return 0;
}