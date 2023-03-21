#include <iostream>

#include "echoServer.h"


int main()
{
	/*CQNetLib::sLogConfig LogConfig;
	strncpy(LogConfig.s_szLogFileName, "ChatServer", MAX_FILENAME_LENGTH);
	LogConfig.s_nLogInfoTypes[STORAGE_OUTPUTWND] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[STORAGE_WINDOW] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[STORAGE_FILE] = LOG_ERROR_ALL;
	INIT_LOG(LogConfig);*/

	CQNetLib::INITCONFIG InitConfig;
	InitConfig.nServerPort = 32452;
	InitConfig.nRecvBufCnt = 10;
	InitConfig.nRecvBufSize = 1024;
	InitConfig.nProcessPacketCnt = 1000;
	InitConfig.nSendBufCnt = 10;
	InitConfig.nSendBufSize = 1024;
	InitConfig.nWorkerThreadCnt = 2;
	InitConfig.nProcessThreadCnt = 1;


	EchoServer server;
	server.Init(InitConfig, 10);
	server.ServerStart(InitConfig);
		
	//LOG(LOG_INFO_LOW, "서버 시작..");
	std::cout << "키를 누르면 종료..." << std::endl;

	getchar();

	//CLOSE_LOG();

	return 0;
}