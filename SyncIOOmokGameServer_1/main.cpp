#include <iostream>
#include <thread>

#include "ChatServer.h"

int main()
{
	ChatServerLib::ChatServer chatServer;
	
	NServerNetLib::ServerConfig m_pServerConfig = NServerNetLib::ServerConfig();
	m_pServerConfig.Port = 32452;
	m_pServerConfig.MaxClientCount = 1000;
	m_pServerConfig.ExtraClientCount = 64;

	m_pServerConfig.MaxClientRecvBufferSize = 8192;
	m_pServerConfig.MaxClientSendBufferSize = 8192;
	m_pServerConfig.MaxRoomUserCount = 4;
	m_pServerConfig.MaxRoomCountByLobby = 5;
	
	std::thread logicThread([&]() 
		{	

		if (chatServer.Init(m_pServerConfig) != ChatServerLib::ERROR_CODE::NONE)
		{
			std::cout << "Init Fail";
			return 0;
		}

		chatServer.Run();

		}
	);

	std::cout << "press any key to exit..."<<std::endl;

	getchar();

	chatServer.Stop();
	logicThread.join();

	return 0;
}

