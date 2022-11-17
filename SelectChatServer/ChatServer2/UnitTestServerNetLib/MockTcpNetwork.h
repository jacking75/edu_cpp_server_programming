#pragma once

#include "../ServerNetLib/TcpNetwork.h"

using namespace NServerNetLib;

class MockTcpNetwork : public TcpNetwork
{
public:
	MockTcpNetwork() { CreateConfig(); }

	void CreateConfig()
	{
		m_Config.MaxClientCount = 4;
		m_Config.ExtraClientCount = 2;
		m_Config.MaxClientSockOptRecvBufferSize = 1000;
		m_Config.MaxClientSockOptSendBufferSize = 100;
		m_Config.MaxClientRecvBufferSize = 128;
		m_Config.MaxClientSendBufferSize = 128;
		m_Config.MaxLobbyCount = 2;
		m_Config.MaxLobbyUserCount = 10;
		m_Config.MaxRoomCountByLobby = 4;
		m_Config.MaxRoomUserCount = 3;
	}

	void CreateSessionPool(const int maxClientCount) { __super::CreateSessionPool(maxClientCount);  }

	std::vector<ClientSession>& GetClientSession() { return m_ClientSessionPool; }
};