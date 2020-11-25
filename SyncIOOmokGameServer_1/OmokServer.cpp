#include "OmokServer.h"
#include <iostream>

namespace ChatServerLib
{
	OmokServer::OmokServer()
	{

	}

	OmokServer::~OmokServer()
	{
		if (m_pNetwork) 
		{
			m_pNetwork->Release();
		}
	}

	ERROR_CODE OmokServer::Init(const NServerNetLib::ServerConfig Config)
	{

		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();
		auto result = m_pNetwork->Init(Config);

		if (result != NServerNetLib::NET_ERROR_CODE::NONE)
		{
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}

		m_pUserMgr = std::make_unique<UserManager>();
		m_pUserMgr->Init(Config.MaxClientCount);

		m_pRoomMgr = std::make_unique<RoomManager>();
		m_pRoomMgr->Init(Config.MaxRoomCountByLobby, m_pNetwork.get());

		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(),m_pRoomMgr.get(), Config);

		m_IsRun = true;

		return ERROR_CODE::NONE;
	}
	
	void OmokServer::Stop()
	{
		m_IsRun = false;
	}

	void OmokServer::Run()
	{
		while (m_IsRun)
		{
			m_pNetwork->Run();

			while (true) 
			{
				auto packetInfo = m_pNetwork->GetReceivePacket();
				
				if (packetInfo.has_value() == false) 
				{
					break;
				}
				else 
				{
					m_pPacketProc->Process(packetInfo.value());
				}
			}
		}
	}
}