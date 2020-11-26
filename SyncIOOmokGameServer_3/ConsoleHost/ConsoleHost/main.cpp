#include <iostream>
#include <thread>

#include "../../LogicLib/LogicLib/OmokServer.h"

int main()
{
	ChatServerLib::OmokServer omokServer;
	
	NServerNetLib::ServerConfig m_pServerConfig = NServerNetLib::ServerConfig();
	m_pServerConfig.Port = 32452;
	m_pServerConfig.MaxClientCount = 1000;
	m_pServerConfig.ExtraClientCount = 64;

	m_pServerConfig.BackLogCount = 128;

	m_pServerConfig.MaxClientRecvBufferSize = 8192;
	m_pServerConfig.MaxClientSendBufferSize = 8192;
	m_pServerConfig.MaxRoomUserCount = 4;
	m_pServerConfig.MaxRoomCountByLobby = 5;
	

	if (omokServer.Init(m_pServerConfig) != ChatServerLib::ERROR_CODE::NONE)
	{
		std::cout << "Init Fail";
		return 0;
	}

	if (omokServer.Run() != ChatServerLib::ERROR_CODE::NONE)
	{
		std::cout << "Run Fail";
		return 0;
	}
	
	return 0;
}

