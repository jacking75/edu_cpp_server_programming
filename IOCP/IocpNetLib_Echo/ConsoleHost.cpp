// TGS_ConsoleHost.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#include <iostream>

#include "flags.h"
#include "EchoServer.h"

ServerAppConfig ParseConfig(int argc, char* argv[]);

int main(int argc, char* argv[])
{		
	auto serverConfig = ParseConfig(argc, argv);
	
	NetLib::LogFuncPtr = [](const int eLogInfoType, const char* szLogMsg) {
		UNREFERENCED_PARAMETER(eLogInfoType);
		printf("%s\r\n", szLogMsg);
	};

	EchoServer main;
	main.Init(serverConfig);

	std::thread logicThread([&]() {
		main.Run(); }
	);

	
	std::cout << "Server quits when you press (quit) !!!" << std::endl;

	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);
		
		if (inputCmd == "quit")
		{
			break;
		}
	}

	main.Stop();
	logicThread.join();
}


ServerAppConfig ParseConfig(int argc, char* argv[])
{
	auto serverConfig = ServerAppConfig();

	const flags::args args(argc, argv);
	const auto port = args.get<UINT16>("port"); 
	if (!port) { 
		std::cerr << "No Port. :(" << std::endl;
		return serverConfig;
	} 
	std::cout << "Port: " << *port << std::endl;


	const auto WorkThreadCount = args.get<INT32>("WorkThreadCount");
	if (!WorkThreadCount) {
		std::cerr << "No WorkThreadCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "WorkThreadCount: " << *WorkThreadCount << std::endl;


	const auto MaxRecvOverlappedBufferSize = args.get<INT32>("MaxRecvOverlappedBufferSize");
	if (!MaxRecvOverlappedBufferSize) {
		std::cerr << "No MaxRecvOverlappedBufferSize. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxRecvOverlappedBufferSize: " << *MaxRecvOverlappedBufferSize << std::endl;


	const auto MaxSendOverlappedBufferSize = args.get<INT32>("MaxSendOverlappedBufferSize");
	if (!MaxSendOverlappedBufferSize) {
		std::cerr << "No MaxSendOverlappedBufferSize. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxSendOverlappedBufferSize: " << *MaxSendOverlappedBufferSize << std::endl;


	const auto MaxRecvConnectionBufferCount = args.get<INT32>("MaxRecvConnectionBufferCount");
	if (!MaxRecvConnectionBufferCount) {
		std::cerr << "No MaxRecvConnectionBufferCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxRecvConnectionBufferCount: " << *MaxRecvConnectionBufferCount << std::endl;


	const auto MaxSendConnectionBufferCount = args.get<INT32>("MaxSendConnectionBufferCount");
	if (!MaxSendConnectionBufferCount) {
		std::cerr << "No MaxSendConnectionBufferCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxSendConnectionBufferCount: " << *MaxSendConnectionBufferCount << std::endl;


	const auto MaxPacketSize = args.get<INT32>("MaxPacketSize");
	if (!MaxPacketSize) {
		std::cerr << "No MaxPacketSize. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxSendConnectionBufferCount: " << *MaxSendConnectionBufferCount << std::endl;


	const auto MaxConnectionCount = args.get<INT32>("MaxConnectionCount");
	if (!MaxSendConnectionBufferCount) {
		std::cerr << "No MaxConnectionCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxConnectionCount: " << *MaxConnectionCount << std::endl;


	const auto MaxMessagePoolCount = args.get<INT32>("MaxMessagePoolCount");
	if (!MaxMessagePoolCount) {
		std::cerr << "No MaxMessagePoolCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "MaxMessagePoolCount: " << *MaxMessagePoolCount << std::endl;


	const auto ExtraMessagePoolCount = args.get<INT32>("ExtraMessagePoolCount");
	if (!ExtraMessagePoolCount) {
		std::cerr << "No ExtraMessagePoolCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "ExtraMessagePoolCount: " << *ExtraMessagePoolCount << std::endl;


	const auto startRoomNumber = args.get<INT32>("StartRoomNumber");
	if (!startRoomNumber) {
		std::cerr << "No StartRoomNumber. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "startRoomNumber: " << *startRoomNumber << std::endl;


	const auto maxRoomCount = args.get<INT32>("MaxRoomCnt");
	if (!startRoomNumber) {
		std::cerr << "No maxRoomCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "maxRoomCount: " << *maxRoomCount << std::endl;
	

	const auto maxRoomUserCount = args.get<INT32>("MaxRoomUserCnt");
	if (!maxRoomUserCount) {
		std::cerr << "No maxRoomUserCount. :(" << std::endl;
		return serverConfig;
	}
	std::cout << "maxRoomUserCount: " << *maxRoomUserCount << std::endl;


	serverConfig.PortNumber = *port;
	serverConfig.WorkThreadCount = *WorkThreadCount;
	serverConfig.MaxRecvOverlappedBufferSize = *MaxRecvOverlappedBufferSize;
	serverConfig.MaxSendOverlappedBufferSize = *MaxSendOverlappedBufferSize;
	serverConfig.ConnectionMaxRecvBufferSize = *MaxRecvConnectionBufferCount;
	serverConfig.ConnectionMaxSendBufferSize = *MaxSendConnectionBufferCount;
	serverConfig.MaxPacketSize = *MaxPacketSize;
	serverConfig.MaxConnectionCount = *MaxConnectionCount;
	serverConfig.MaxMessagePoolCount = *MaxMessagePoolCount;
	serverConfig.ExtraMessagePoolCount = *ExtraMessagePoolCount;
	serverConfig.PerformancePacketMillisecondsTime = 0;

	serverConfig.PostMessagesThreadsCount = 1;
	
	return serverConfig;
}



