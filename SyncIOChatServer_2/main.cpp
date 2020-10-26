#include <iostream>
#include <thread>

#include "ChatServer.h"

int main()
{
	ChatServerLib::ChatServer chatServer;
	
	NServerNetLib::ServerConfig* m_pServerConfig = new NServerNetLib::ServerConfig();
	m_pServerConfig->Port = 32452;
	m_pServerConfig->MaxClientCount = 1000;
	m_pServerConfig->ExtraClientCount = 64;

	m_pServerConfig->MaxClientRecvBufferSize = 8192;
	m_pServerConfig->MaxClientSendBufferSize = 8192;

	//TODO : 두개 합치기 
	if (chatServer.Init(m_pServerConfig) != NServerNetLib::ERROR_CODE::NONE) {
		std::cout << "Init Fail";
		return 0;
	}
	

	std::thread logicThread([&]() {	
		chatServer.Run();
		}
	);

	std::cout << "press any key to exit..."<<std::endl;

	getchar();

	chatServer.Stop();
	logicThread.join();

	delete m_pServerConfig;
	return 0;
}

