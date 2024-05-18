#include <iostream>

//#include "ConnectionManager.h"
#include "chatServer.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#include <loguru.cpp> //내부에서 Win32 API의 네트워크 함수를 포함하지 않도록 하는 것 같음. 그래서 네트워크 라이브러리 뒤에 선언을 해야 한다
#pragma warning(pop)

int main(int argc, char* argv[])
{
	loguru::init(argc, argv);
	//loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);
	LOG_F(INFO, "Hello log file!");

	//void (*CQNetLib::LogFuncPtr)(const int eLogInfoType, const char* szOutputString, ...) = [](const int eLogInfoType, const char* szOutputString, ...) {}
	CQNetLib::LogFuncPtr = [](const int eLogInfoType, const char* szOutputString, ...) {
		va_list	argptr;
		va_start(argptr, szOutputString);

		char szLogMsg[256] = { 0, };
		_vsnprintf_s(szLogMsg, 256, szOutputString, argptr);
		va_end(argptr);

		printf("%s", szLogMsg);
	};
	
	CQNetLib::INITCONFIG InitConfig;
	InitConfig.nServerPort = 32452;
	InitConfig.nRecvBufCnt = 10;
	InitConfig.nRecvBufSize = 1024;
	InitConfig.nProcessPacketCnt = 1000;
	InitConfig.nSendBufCnt = 10;
	InitConfig.nSendBufSize = 1024;
	InitConfig.nWorkerThreadCnt = 2;
	InitConfig.nProcessThreadCnt = 1;


	ChatServer server;
	server.Init(InitConfig, 10);
	server.ServerStart(InitConfig);

	//ConnectionManager connectionMgr;
	//connectionMgr.CreateConnection(InitConfig, 10, &server);
	
	//LOG(LOG_INFO_LOW, "서버 시작..");
	std::cout << "키를 누르면 종료..." << std::endl;
	
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);
		break;
		/*if (inputCmd == "kill")
		{
			break;
		}*/
	}

	
	return 0;
}

