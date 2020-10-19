#include <iostream>
#include <thread>
#include "TcpNetwork.h"
#include "ErrorCode.h"

int main()
{
	NServerNetLib::TcpNetwork serverNet;
	
	NServerNetLib::ServerConfig* m_pServerConfig = new NServerNetLib::ServerConfig();
	m_pServerConfig->Port = 32452;
	m_pServerConfig->MaxClientCount = 1000;

	m_pServerConfig->MaxClientSockOptRecvBufferSize = 10240;
	m_pServerConfig->MaxClientSockOptSendBufferSize = 10240;
	m_pServerConfig->MaxClientRecvBufferSize = 8192;
	m_pServerConfig->MaxClientSendBufferSize = 8192;

	m_pServerConfig->ExtraClientCount = 64;


	if (serverNet.Init(m_pServerConfig) != NServerNetLib::NET_ERROR_CODE::NONE) {
		std::cout << "Init Fail";
		return 0;
	}

	std::thread logicThread([&]() {	
			serverNet.Run();
		}
	);

	std::cout << "press any key to exit...";
	getchar();

	serverNet.Stop();
	logicThread.join();

	delete m_pServerConfig;
	return 0;
}

