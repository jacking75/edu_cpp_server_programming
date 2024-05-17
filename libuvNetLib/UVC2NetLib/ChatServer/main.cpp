#include "ChatServer.h"

#include <iostream>
#include <thread>

int main()
{
	UVC2NetLibz::NetConfig config;
	config.IP = "0.0.0.0";
	config.PortNumber = 11021;
	config.Backlog = 64;
	config.MaxConnectionCount = 2;
	config.Conn.MaxRecvPacketSize = 1024;
	config.Conn.MaxTry1RecvBufferSize = 4096;
	config.Conn.MAXRecvCountPer1Sec = 5;
	config.Conn.MaxRecvBufferSize = 4096 * 2;
	config.Conn.MaxSendPacketSize = 1024;
	config.Conn.MAXSendCountPer1Sec = 5;
	config.Conn.MaxSendBufferSize = 4096 * 2;

	char logmsg[256] = { 0, };

	ChatServer server;
	
	auto ret = server.Init(config);
	if (ret != 0)
	{
		//sprintf_s(logmsg, "ChatServer::Init() Error: %d", ret);
		//ChatServer::WriteLog((int)UVC2NetLibz::LogLevel::error, logmsg);
		ChatServer::WriteLog2((int)UVC2NetLibz::LogLevel::error, "ChatServer::Init() Error: %d", ret);
		return ret;
	}
	ChatServer::WriteLog2((int)UVC2NetLibz::LogLevel::error, "ChatServer::Init() Error: %d", ret);
	ChatServer::WriteLog((int)UVC2NetLibz::LogLevel::info, "ChatServer::Start()");
	server.StartAsync();
	
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

	server.End();
	return 0;
}