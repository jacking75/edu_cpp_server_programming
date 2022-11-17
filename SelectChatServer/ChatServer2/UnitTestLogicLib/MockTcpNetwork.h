#pragma once

#include <vector>
#include "../ServerNetLib/TcpNetwork.h"

using namespace NServerNetLib;

struct SendPacketInfo
{
	int SessionIndex = 0;
	char data[1024] = { 0, };
};

class MockTcpNetwork : public TcpNetwork
{
public:
	MockTcpNetwork() {}

	void Init(int maxClientCount)
	{
		CreateConfig();

		m_Config.MaxClientCount = maxClientCount;

		CreateSessionPool(m_Config.MaxClientCount);
	}

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

	NET_ERROR_CODE SendData(const int sessionIndex, const short packetId, const short size, const char* pMsg) override
	{
		SendPacketInfo packet;
		packet.SessionIndex = sessionIndex;

		PacketHeader pktHeader{ packetId, size };
		memcpy(packet.data, (char*)&pktHeader, PACKET_HEADER_SIZE);
		if (size > 0) {
			memcpy(&packet.data[PACKET_HEADER_SIZE], pMsg, size);
		}		
		
		m_PacketList.push_back(packet);

		return NET_ERROR_CODE::NONE;
	}


	std::vector<SendPacketInfo> m_PacketList;
	
	
	
	/*void CreateSessionPool(const int maxClientCount) { __super::CreateSessionPool(maxClientCount);  }

	std::vector<ClientSession>& GetClientSession() { return m_ClientSessionPool; }*/
};

